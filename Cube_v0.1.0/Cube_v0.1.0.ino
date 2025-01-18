#include <LiteLED.h>

// LED strip type configuration
#define LED_TYPE        LED_STRIP_SK6812
#define LED_TYPE_IS_RGBW 0   // 0 = RGB, 1 = RGBW

#define LED_GPIO    15
#define LED_BRIGHT  255
#define NUM_LEDS    13

// Define white color (you can adjust to pure white = 0xFFFFFF if you like)
static const crgb_t L_WHITE = 0xe0e0e0;

// Create the LiteLED object
LiteLED myLED(LED_TYPE, LED_TYPE_IS_RGBW);

void setup() {
  // Initialize the LED strip with 13 LEDs
  myLED.begin(LED_GPIO, NUM_LEDS);

  // Set brightness
  myLED.brightness(LED_BRIGHT);
  
  // Clear all LEDs at the start
  myLED.clear(true);
}

void loop() {
  // Loop through each LED (0 to 12)
  for (int i = 0; i < NUM_LEDS; i++) {
    // Turn all LEDs off without showing immediately
    myLED.clear(false);
    
    // Set the i-th LED to white
    myLED.setPixel(i, L_WHITE, false);

    // Now show the updated LED states
    myLED.show();
    
    // Wait 30 ms before moving on
    delay(30);
  }
  
}
