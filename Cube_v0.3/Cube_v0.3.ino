#include <LiteLED.h>
#include <math.h>

int speed = 50;
int density = 75;
int repeat = 100;
int uniformity = 0;
int brightness = 255;

uint8_t r = 255;
uint8_t b = 255;
uint8_t g = 255;

// -----------------------------------------------------------------------------
// Configuration
// -----------------------------------------------------------------------------
float cornerX = 0.0f;
float cornerY = 0.0f;
float cornerZ = 0.0f;
float OFFSET  = 2.0f * M_PI / 3.0f;

#define LED_TYPE          LED_STRIP_WS2812
#define LED_TYPE_IS_RGBW  0
#define LED_BRIGHT        255

#define LED_GPI1   15
#define NUM_LEDS1  13
LiteLED myLED1(LED_STRIP_WS2812, 0, RMT_CHANNEL_0);

#define LED_GPI2   16
#define NUM_LEDS2  13
LiteLED myLED2(LED_STRIP_WS2812, 0, RMT_CHANNEL_1);

#define CALCULATED_LEDS  26  // total of both strips

// Simple clamp helper
float clamp(float value, float minVal, float maxVal) {
    return fmaxf(fminf(value, maxVal), minVal);
}

// logic
enum LEDMode {
    SOLID,
    GAY
};
LEDMode currentMode = GAY;

// -----------------------------------------------------------------------------
// LED3D: A class for an LED in 3D space
// -----------------------------------------------------------------------------
class LED3D {
public:
    float x, y, z;
    uint8_t strip;         // Which strip does this LED belong to? (0 or 1)
    uint8_t localIndex;    // The index within that strip
    uint32_t color;        // 24-bit color (R,G,B)

    LED3D(float x_, float y_, float z_, uint8_t s_, uint8_t idx_)
        : x(x_), y(y_), z(z_), strip(s_), localIndex(idx_), color(0) {}

    void updateColor(float phase) 
    {      
      switch (currentMode) {
        case SOLID:        
          this->color = ((uint32_t)r << 16) |
                        ((uint32_t)g <<  8)  |
                        (uint32_t)b;
          break;

        case GAY:  // 1) Convert both LED position and corner to the same scaled+offset space.
          float scaledX = x * (density / 10.0f);
          float scaledY = y * (density / 10.0f);
          float scaledZ = z * (density / 10.0f);

          float dx = scaledX - (cornerX * (density / 10.0f));
          float dy = scaledY - (cornerY * (density / 10.0f));
          float dz = scaledZ - (cornerZ * (density / 10.0f));

          // 2) If repeat > 100, apply the repeat factor, else just straight distance
          float dist = sqrtf(dx*dx + dy*dy + dz*dz);
          if (repeat > 100) {
              dist *= ((float)repeat / 700.0f) * (sinf(phase) + 1.0f);
          }

          // 3) Sine wave color components: produce a float from 0..255
          float wave0f = (sinf(dist + phase) + 1.0f) * 128.0f;
          float wave1f = (sinf(dist + phase + OFFSET) + 1.0f) * 128.0f;
          float wave2f = (sinf(dist + phase + 2.0f*OFFSET) + 1.0f) * 128.0f;

          // 4) Clamp to [0..255] and cast
          uint8_t wave0 = (uint8_t)fmaxf(fminf(wave0f, 255.0f), 0.0f);
          uint8_t wave1 = (uint8_t)fmaxf(fminf(wave1f, 255.0f), 0.0f);
          uint8_t wave2 = (uint8_t)fmaxf(fminf(wave2f, 255.0f), 0.0f);

          // 5) Apply global brightness scaling
          wave0 = (wave0 * brightness) >> 8;
          wave1 = (wave1 * brightness) >> 8;
          wave2 = (wave2 * brightness) >> 8;

          // 6) Pack into 24-bit color
          this->color = ((uint32_t)wave0 << 16) |
                        ((uint32_t)wave1 <<  8)  |
                        (uint32_t)wave2;

          break;
      }

    }

    // This function applies the current color to the underlying LiteLED object.
    // It does NOT call show() — we'll do that once, after all LEDs are set.
    void applyColor()
    {
        if (strip == 0) {
            // Send color to myLED1 using localIndex
            myLED1.setPixel(localIndex, color, false /*no immediate show*/);
        } else {
            // Send color to myLED2 using localIndex
            myLED2.setPixel(localIndex, color, false /*no immediate show*/);
        }
    }
};

// -----------------------------------------------------------------------------
// Define our 3D LED layout
// -----------------------------------------------------------------------------
LED3D leds[CALCULATED_LEDS] = {

    // ----- Strip 0: 13 LEDs (localIndex = 0..12) -----
    // Example: line along X from 0..1 in 13 steps:
    // i-th LED at x = (float)i/12, y=0, z=0
    // Feel free to adjust the actual coordinates
    LED3D(0.0f/12.0f ,   0.0f, 0.0f,  0, 0),
    LED3D(1.0f/12.0f ,   0.0f, 0.0f,  0, 1),
    LED3D(2.0f/12.0f ,   0.0f, 0.0f,  0, 2),
    LED3D(3.0f/12.0f ,   0.0f, 0.0f,  0, 3),
    LED3D(4.0f/12.0f ,   0.0f, 0.0f,  0, 4),
    LED3D(5.0f/12.0f ,   0.0f, 0.0f,  0, 5),
    LED3D(6.0f/12.0f ,   0.0f, 0.0f,  0, 6),
    LED3D(7.0f/12.0f ,   0.0f, 0.0f,  0, 7),
    LED3D(8.0f/12.0f ,   0.0f, 0.0f,  0, 8),
    LED3D(9.0f/12.0f ,   0.0f, 0.0f,  0, 9),
    LED3D(10.0f/12.0f,   0.0f, 0.0f,  0, 10),
    LED3D(11.0f/12.0f,   0.0f, 0.0f,  0, 11),
    LED3D(12.0f/12.0f,   0.0f, 0.0f,  0, 12),

    // ----- Strip 1: 13 LEDs (localIndex = 0..12) -----
    // Example: line along Y from 0..1 in 13 steps (or some pattern in Y/Z):
    LED3D(0.0f, 0.0f/12.0f  , 0.0f, 1, 0),
    LED3D(0.0f, 1.0f/12.0f  , 0.0f, 1, 1),
    LED3D(0.0f, 2.0f/12.0f  , 0.0f, 1, 2),
    LED3D(0.0f, 3.0f/12.0f  , 0.0f, 1, 3),
    LED3D(0.0f, 4.0f/12.0f  , 0.0f, 1, 4),
    LED3D(0.0f, 5.0f/12.0f  , 0.0f, 1, 5),
    LED3D(0.0f, 6.0f/12.0f  , 0.0f, 1, 6),
    LED3D(0.0f, 7.0f/12.0f  , 0.0f, 1, 7),
    LED3D(0.0f, 8.0f/12.0f  , 0.0f, 1, 8),
    LED3D(0.0f, 9.0f/12.0f  , 0.0f, 1, 9),
    LED3D(0.0f, 10.0f/12.0f , 0.0f, 1, 10),
    LED3D(0.0f, 11.0f/12.0f , 0.0f, 1, 11),
    LED3D(0.0f, 12.0f/12.0f , 0.0f, 1, 12)
};

void recalcCornerXYZ()
{

    float frac = (float)uniformity / 100.0f;

    cornerX = frac;
    cornerY = frac;
    cornerZ = frac;

    // Optionally print for debugging:
    Serial.print("cornerX="); Serial.print(cornerX);
    Serial.print(", cornerY="); Serial.print(cornerY);
    Serial.print(", cornerZ="); Serial.println(cornerZ);
}


void setup() {
  Serial.begin(115200);

  wifi_init();

  // Initialize both LED strips
  myLED1.begin(LED_GPI1, NUM_LEDS1);
  myLED1.brightness(LED_BRIGHT);

  myLED2.begin(LED_GPI2, NUM_LEDS2);
  myLED2.brightness(LED_BRIGHT);

  // Optionally set an initial phase
  float initialPhase = 0.0f;

  // Update color of each LED once at startup, then apply it
  for (int i = 0; i < CALCULATED_LEDS; i++) {
      leds[i].updateColor(initialPhase);
      leds[i].applyColor();  // sets color on correct strip, no show
  }
  // Now show on each strip
  myLED1.show();
  myLED2.show();
}

void loop() {
  static float phase = 0.0f;
  phase += (float)speed/500;  // Adjust speed as you like

  // 1) Update all LED color values
  for (int i = 0; i < CALCULATED_LEDS; i++) {
      leds[i].updateColor(phase);
      leds[i].applyColor();  // sets pixel color on the underlying strip
  }

  // 2) Then show on each strip once all are updated
  myLED1.show();
  myLED2.show();

  delay(10);
}
