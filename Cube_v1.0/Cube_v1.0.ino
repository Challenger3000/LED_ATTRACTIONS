#include <LiteLED.h>
#include <math.h>

int speed = 40;
int density = 100;
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

#define LED_GPI1   17
#define NUM_LEDS1  20
LiteLED myLED1(LED_STRIP_WS2812, 0, RMT_CHANNEL_0);

#define LED_GPI2   16
#define NUM_LEDS2  46
LiteLED myLED2(LED_STRIP_WS2812, 0, RMT_CHANNEL_1);

#define LED_GPI3   15
#define NUM_LEDS3  20
LiteLED myLED3(LED_STRIP_WS2812, 0, RMT_CHANNEL_2);

#define LED_GPI4   18
#define NUM_LEDS4  46
LiteLED myLED4(LED_STRIP_WS2812, 0, RMT_CHANNEL_3);

// #define CALCULATED_LEDS  132  // total of both strips
#define CALCULATED_LEDS  (NUM_LEDS1 + NUM_LEDS2 + NUM_LEDS3 + NUM_LEDS4)

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

    LED3D() : x(0.0f), y(0.0f), z(0.0f), strip(0), localIndex(0), color(0) {}

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
            myLED1.setPixel(localIndex, color, false /*no immediate show*/);
        }
        else if (strip == 1) {
            myLED2.setPixel(localIndex, color, false /*no immediate show*/);
        }
        else if (strip == 2) {
            myLED3.setPixel(localIndex, color, false /*no immediate show*/);
        }
        else if (strip == 3) {
            myLED4.setPixel(localIndex, color, false /*no immediate show*/);
        }
    }

};

LED3D leds[CALCULATED_LEDS];


// -----------------------------------------------------------------------------
// Define our 3D LED layout
// -----------------------------------------------------------------------------
// LED3D leds[CALCULATED_LEDS] = {
//     // ---------- Strip 0: 20 LEDs (localIndex 0..19) ----------
//     LED3D(0.0f, 0.0f, 0.0f, 0,  0),
//     LED3D(0.0f, 0.0f, 0.0f, 0,  1),
//     LED3D(0.0f, 0.0f, 0.0f, 0,  2),
//     LED3D(0.0f, 0.0f, 0.0f, 0,  3),
//     LED3D(0.0f, 0.0f, 0.0f, 0,  4),
//     LED3D(0.0f, 0.0f, 0.0f, 0,  5),
//     LED3D(0.0f, 0.0f, 0.0f, 0,  6),
//     LED3D(0.0f, 0.0f, 0.0f, 0,  7),
//     LED3D(0.0f, 0.0f, 0.0f, 0,  8),
//     LED3D(0.0f, 0.0f, 0.0f, 0,  9),
//     LED3D(0.0f, 0.0f, 0.0f, 0, 10),
//     LED3D(0.0f, 0.0f, 0.0f, 0, 11),
//     LED3D(0.0f, 0.0f, 0.0f, 0, 12),
//     LED3D(0.0f, 0.0f, 0.0f, 0, 13),
//     LED3D(0.0f, 0.0f, 0.0f, 0, 14),
//     LED3D(0.0f, 0.0f, 0.0f, 0, 15),
//     LED3D(0.0f, 0.0f, 0.0f, 0, 16),
//     LED3D(0.0f, 0.0f, 0.0f, 0, 17),
//     LED3D(0.0f, 0.0f, 0.0f, 0, 18),
//     LED3D(0.0f, 0.0f, 0.0f, 0, 19),

//     // ---------- Strip 1: 46 LEDs (localIndex 0..45) ----------
//     LED3D(0.0f, 0.0f, 0.0f, 1,  0),
//     LED3D(0.0f, 0.0f, 0.0f, 1,  1),
//     LED3D(0.0f, 0.0f, 0.0f, 1,  2),
//     LED3D(0.0f, 0.0f, 0.0f, 1,  3),
//     LED3D(0.0f, 0.0f, 0.0f, 1,  4),
//     LED3D(0.0f, 0.0f, 0.0f, 1,  5),
//     LED3D(0.0f, 0.0f, 0.0f, 1,  6),
//     LED3D(0.0f, 0.0f, 0.0f, 1,  7),
//     LED3D(0.0f, 0.0f, 0.0f, 1,  8),
//     LED3D(0.0f, 0.0f, 0.0f, 1,  9),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 10),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 11),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 12),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 13),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 14),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 15),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 16),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 17),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 18),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 19),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 20),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 21),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 22),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 23),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 24),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 25),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 26),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 27),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 28),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 29),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 30),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 31),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 32),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 33),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 34),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 35),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 36),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 37),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 38),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 39),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 40),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 41),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 42),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 43),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 44),
//     LED3D(0.0f, 0.0f, 0.0f, 1, 45),

//     // ---------- Strip 2: 20 LEDs (localIndex 0..19) ----------
//     LED3D(0.0f, 0.0f, 0.0f, 2,  0),
//     LED3D(0.0f, 0.0f, 0.0f, 2,  1),
//     LED3D(0.0f, 0.0f, 0.0f, 2,  2),
//     LED3D(0.0f, 0.0f, 0.0f, 2,  3),
//     LED3D(0.0f, 0.0f, 0.0f, 2,  4),
//     LED3D(0.0f, 0.0f, 0.0f, 2,  5),
//     LED3D(0.0f, 0.0f, 0.0f, 2,  6),
//     LED3D(0.0f, 0.0f, 0.0f, 2,  7),
//     LED3D(0.0f, 0.0f, 0.0f, 2,  8),
//     LED3D(0.0f, 0.0f, 0.0f, 2,  9),
//     LED3D(0.0f, 0.0f, 0.0f, 2, 10),
//     LED3D(0.0f, 0.0f, 0.0f, 2, 11),
//     LED3D(0.0f, 0.0f, 0.0f, 2, 12),
//     LED3D(0.0f, 0.0f, 0.0f, 2, 13),
//     LED3D(0.0f, 0.0f, 0.0f, 2, 14),
//     LED3D(0.0f, 0.0f, 0.0f, 2, 15),
//     LED3D(0.0f, 0.0f, 0.0f, 2, 16),
//     LED3D(0.0f, 0.0f, 0.0f, 2, 17),
//     LED3D(0.0f, 0.0f, 0.0f, 2, 18),
//     LED3D(0.0f, 0.0f, 0.0f, 2, 19),

//     // ---------- Strip 3: 46 LEDs (localIndex 0..45) ----------
//     LED3D(0.0f, 0.0f, 0.0f, 3,  0),
//     LED3D(0.0f, 0.0f, 0.0f, 3,  1),
//     LED3D(0.0f, 0.0f, 0.0f, 3,  2),
//     LED3D(0.0f, 0.0f, 0.0f, 3,  3),
//     LED3D(0.0f, 0.0f, 0.0f, 3,  4),
//     LED3D(0.0f, 0.0f, 0.0f, 3,  5),
//     LED3D(0.0f, 0.0f, 0.0f, 3,  6),
//     LED3D(0.0f, 0.0f, 0.0f, 3,  7),
//     LED3D(0.0f, 0.0f, 0.0f, 3,  8),
//     LED3D(0.0f, 0.0f, 0.0f, 3,  9),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 10),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 11),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 12),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 13),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 14),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 15),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 16),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 17),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 18),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 19),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 20),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 21),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 22),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 23),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 24),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 25),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 26),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 27),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 28),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 29),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 30),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 31),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 32),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 33),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 34),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 35),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 36),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 37),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 38),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 39),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 40),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 41),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 42),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 43),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 44),
//     LED3D(0.0f, 0.0f, 0.0f, 3, 45)
// };


void defineCubeMapping()
{
    int idx = 0;

    // === Strip 0 (20 LEDs): front-left vertical
    // from (0,0,0)->(0,0,1)
    for (int i = 0; i < 20; i++) {
        float t = i / 19.0f; // fraction from 0..1
        leds[idx++] = LED3D(
            0.0f,        // x
            0.0f,        // y
            t,           // z
            0,           // strip #0
            i            // local index
        );
    }

    // === Strip 1 (46 LEDs): front-right vertical (20) + top-front horizontal (26)

    // 1A) front-right vertical: (1,0,0)->(1,0,1)
    for (int i = 0; i < 20; i++) {
        float t = i / 19.0f;
        leds[idx++] = LED3D(
            1.0f,        // x
            0.0f,        // y
            t,           // z
            1,           // strip #1
            i            // local index
        );
    }
    // 1B) top-front horizontal: (1,0,1)->(0,0,1)
    // 26 LEDs => i from 0..25 => fraction = i/25
    // x goes from 1.0 down to 0.0, y=0, z=1
    for (int i = 0; i < 26; i++) {
        float t = i / 25.0f;      // fraction 0..1
        float xPos = 1.0f - t;    // moves from x=1 to x=0
        int localIdx = 20 + i;    // these 26 come after the first 20
        leds[idx++] = LED3D(
            xPos,       // x
            0.0f,       // y
            1.0f,       // z
            1,          // strip #1
            localIdx
        );
    }

    // === Strip 2 (20 LEDs): back-right vertical: (1,1,0)->(1,1,1)
    for (int i = 0; i < 20; i++) {
        float t = i / 19.0f;
        leds[idx++] = LED3D(
            1.0f,       // x
            1.0f,       // y
            t,          // z
            2,          // strip #2
            i
        );
    }

    // === Strip 3 (46 LEDs): back-left vertical (20) + top-back horizontal (26)

    // 3A) back-left vertical: (0,1,0)->(0,1,1)
    for (int i = 0; i < 20; i++) {
        float t = i / 19.0f;
        leds[idx++] = LED3D(
            0.0f,       // x
            1.0f,       // y
            t,          // z
            3,          // strip #3
            i
        );
    }

    // 3B) top-back horizontal: (0,1,1)->(1,1,1)
    // 26 LEDs => i from 0..25 => fraction = i/25
    // x goes from 0.0 -> 1.0, y=1, z=1
    for (int i = 0; i < 26; i++) {
        float t = i / 25.0f;
        float xPos = t;   // from 0.0 to 1.0
        int localIdx = 20 + i;  // these 26 come after the first 20
        leds[idx++] = LED3D(
            xPos,
            1.0f,
            1.0f,
            3,
            localIdx
        );
    }

    // At this point, 'idx' should equal CALCULATED_LEDS (132).
}

void recalcCornerXYZ()
{

    float frac = (float)uniformity / 100.0f;

    cornerX = frac;
    cornerY = frac;
    cornerZ = frac;

    // Optionally print for debugging:
    // Serial.print("cornerX="); // Serial.print(cornerX);
    // Serial.print(", cornerY="); // Serial.print(cornerY);
    // Serial.print(", cornerZ="); // Serial.println(cornerZ);
}


void setup() {
  // // Serial.begin(115200);

  wifi_init();

  defineCubeMapping();

  // Initialize both LED strips
  myLED1.begin(LED_GPI1, NUM_LEDS1);
  myLED1.brightness(LED_BRIGHT);
  // myLED1.setPixel(0, 0xFFFFFF, false /*no immediate show*/);

  myLED2.begin(LED_GPI2, NUM_LEDS2);
  myLED2.brightness(LED_BRIGHT);
  // myLED2.setPixel(0, 0xFFFFFF, false /*no immediate show*/);
  // myLED2.setPixel(1, 0xFFFFFF, false /*no immediate show*/);
  
  myLED3.begin(LED_GPI3, NUM_LEDS3);
  myLED3.brightness(LED_BRIGHT);
  // myLED3.setPixel(0, 0xFFFFFF, false /*no immediate show*/);
  // myLED3.setPixel(1, 0xFFFFFF, false /*no immediate show*/);
  // myLED3.setPixel(2, 0xFFFFFF, false /*no immediate show*/);
  
  myLED4.begin(LED_GPI4, NUM_LEDS4);
  myLED4.brightness(LED_BRIGHT);
  // myLED4.setPixel(0, 0xFFFFFF, false /*no immediate show*/);
  // myLED4.setPixel(1, 0xFFFFFF, false /*no immediate show*/);
  // myLED4.setPixel(2, 0xFFFFFF, false /*no immediate show*/);
  // myLED4.setPixel(3, 0xFFFFFF, false /*no immediate show*/);
  
  // myLED1.show();
  // myLED2.show();
  // myLED3.show();
  // myLED4.show();

  // while(true){
  //   delay(50);
  // }

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
  myLED3.show();
  myLED4.show();
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
  myLED3.show();
  myLED4.show();

  delay(10);
}
