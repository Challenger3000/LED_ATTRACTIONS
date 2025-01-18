#define CALCULATED_LEDS 13

#include <LiteLED.h>
#include <math.h>

// -----------------------------------------------------------------------------
// Configuration
// -----------------------------------------------------------------------------
static const float cornerX = 0.5f;
static const float cornerY = 0.5f;
static const float cornerZ = 0.5f;
static const float OFFSET = 2.0f * M_PI / 3.0f;
#define LED_TYPE          LED_STRIP_WS2812
#define LED_TYPE_IS_RGBW  0
#define LED_BRIGHT        255

#define LED_GPI1          15
#define NUM_LEDS1          13
LiteLED myLED1(LED_TYPE, LED_TYPE_IS_RGBW);

#define LED_GPI2          16
#define NUM_LEDS2         13 
LiteLED myLED2(LED_TYPE, LED_TYPE_IS_RGBW);

#define LED_COUNT 26

float clamp(float value, float minVal, float maxVal) {
    return max(min(value, maxVal), minVal);
}

// -----------------------------------------------------------------------------
// LED3D: A class for an LED in 3D space
// -----------------------------------------------------------------------------
class LED3D {
public:
    float x, y, z;
    crgb_t color;
    uint8_t strip;

    LED3D(float x_, float y_, float z_, uint8_t s_)
        : x(x_), y(y_), z(z_), strip(s_), color(0) {}

    void updateColor(float phase) 
    {
        // 1) Compute the distance from a corner (cornerX, cornerY, cornerZ)
        float dx = x - cornerX;
        float dy = y - cornerY;
        float dz = z - cornerZ;
        float dist = sqrtf(dx*dx + dy*dy + dz*dz)*8;
        // float dist = sqrtf(dx*dx + dy*dy + dz*dz) * (5.0*(sinf(phase)+1));

        float wave0 = clamp((sinf(dist + phase) + 1) * 128, 0, 255);
        float wave1 = clamp((sinf(dist + phase + OFFSET) + 1) * 128, 0, 255);
        float wave2 = clamp((sinf(dist + phase + 2.0f * OFFSET) + 1) * 128, 0, 255);

        // Serial.println(wave);

        this->color = ((uint32_t) wave0 << 16) | ((uint32_t) wave1 << 8) | (uint32_t)wave2;
    }
};


LED3D leds[LED_COUNT] = {
    // --- First 13 LEDs: along X from 0 to 1, Y=0, Z=0 ---
    // Indices: 0..12
    // We'll increment X in steps of 1/12 so that the 13th LED is exactly at X=1.0
    LED3D( 0.0f/12.0f, 0.0f, 0.0f, 0 ),  // LED index 0, x=0
    LED3D( 1.0f/12.0f, 0.0f, 0.0f, 0 ),
    LED3D( 2.0f/12.0f, 0.0f, 0.0f, 0 ),
    LED3D( 3.0f/12.0f, 0.0f, 0.0f, 0 ),
    LED3D( 4.0f/12.0f, 0.0f, 0.0f, 0 ),
    LED3D( 5.0f/12.0f, 0.0f, 0.0f, 0 ),
    LED3D( 6.0f/12.0f, 0.0f, 0.0f, 0 ),
    LED3D( 7.0f/12.0f, 0.0f, 0.0f, 0 ),
    LED3D( 8.0f/12.0f, 0.0f, 0.0f, 0 ),
    LED3D( 9.0f/12.0f, 0.0f, 0.0f, 0 ),
    LED3D(10.0f/12.0f, 0.0f, 0.0f, 0 ),
    LED3D(11.0f/12.0f, 0.0f, 0.0f, 0 ),
    LED3D(12.0f/12.0f, 0.0f, 0.0f, 0 ),  // LED index 12, x=1.0

    // --- Next 13 LEDs: along Y from 0 to 1, X=0, Z=0 ---
    // Indices: 13..25
    LED3D( 0.0f,  12.0f/12.0f,  0.0f/12.0f, 1 ), // LED index 13, y=0
    LED3D( 0.0f,  11.0f/12.0f,  1.0f/12.0f, 1 ),
    LED3D( 0.0f,  10.0f/12.0f,  2.0f/12.0f, 1 ),
    LED3D( 0.0f,  9.0f/12.0f,  3.0f/12.0f, 1 ),
    LED3D( 0.0f,  8.0f/12.0f,  4.0f/12.0f, 1 ),
    LED3D( 0.0f,  7.0f/12.0f,  5.0f/12.0f, 1 ),
    LED3D( 0.0f,  6.0f/12.0f,  6.0f/12.0f, 1 ),
    LED3D( 0.0f,  5.0f/12.0f,  7.0f/12.0f, 1 ),
    LED3D( 0.0f,  4.0f/12.0f,  8.0f/12.0f, 1 ),
    LED3D( 0.0f,  3.0f/12.0f,  9.0f/12.0f, 1 ),
    LED3D( 0.0f, 2.0f/12.0f, 10.0f/12.0f, 1 ),
    LED3D( 0.0f, 1.0f/12.0f, 11.0f/12.0f, 1 ),
    LED3D( 0.0f, 0.0f/12.0f, 12.0f/12.0f, 1 )  // LED index 25, y=1.0
};


void setPixelMultiStrip(int ledIndex)
{
    // Grab a reference to the LED3D object
    LED3D &theLed = leds[ledIndex];

    // Decide which physical strip to use
    if (theLed.strip == 0) {
        // For strip 0, we can use 'ledIndex' directly,
        // assuming these first 13 go from 0..12
        myLED1.setPixel(ledIndex, theLed.color, false);
        myLED1.show();
    } else {
        // For strip 1, the local index should start at 0
        // If your global 'ledIndex' for these is 13..25, subtract 13
        int localIndex = ledIndex - 13; 
        myLED2.setPixel(localIndex, theLed.color, false);
        myLED2.show();
    }
}


void setup() {
  // Serial.begin(115200);

  myLED1.begin(LED_GPI1, NUM_LEDS1);
  myLED1.brightness(LED_BRIGHT);
  
  myLED2.begin(LED_GPI2, NUM_LEDS2);
  myLED2.brightness(LED_BRIGHT);

  float initialPhase = 0.0f;
  for (int i = 0; i < CALCULATED_LEDS; i++) {
      leds[i].updateColor(initialPhase);
      setPixelMultiStrip(i);
      // myLED1.setPixel(i, leds[i].color, false);
  }
  myLED1.show();
  myLED2.show();
}

void loop() {

  static float phase = 0.0f;
  phase += 0.05f;             // speed

  for (int i = 0; i < CALCULATED_LEDS; i++) {
      leds[i].updateColor(phase);
      setPixelMultiStrip(i);
      // myLED1.setPixel(i, leds[i].color, false);
  }
  // myLED1.show();
  // myLED2.show();

  delay(10);
}
