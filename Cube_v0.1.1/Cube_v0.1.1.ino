#include <LiteLED.h>

// LED strip type configuration
#define LED_TYPE        LED_STRIP_SK6812
#define LED_TYPE_IS_RGBW 0   // 0 = RGB, 1 = RGBW
#define LED_BRIGHT  255
static const crgb_t L_WHITE = 0xFFFFFF;

#define LED_GPIO    15
#define NUM_LEDS    13
LiteLED myLED(LED_TYPE, LED_TYPE_IS_RGBW);

#define LED_GPI1    16
#define NUM_LEDS1    13
LiteLED myLED1(LED_TYPE, LED_TYPE_IS_RGBW);





void setup() {
  myLED.begin(LED_GPIO, NUM_LEDS);
  myLED1.begin(LED_GPI1, NUM_LEDS1);

  myLED.brightness(LED_BRIGHT);
  myLED1.brightness(LED_BRIGHT);
  
  myLED.clear(true);
  myLED1.clear(true);
}

void loop() {
  // Loop through each LED (0 to 12)
  for (int i = 0; i < NUM_LEDS; i++) {
    myLED.clear(false);
    myLED1.clear(false);
    
    myLED.setPixel(i, L_WHITE, false);
    myLED1.setPixel(i, L_WHITE, false);

    myLED.show();
    myLED1.show();
    
    delay(50);
  }
}
