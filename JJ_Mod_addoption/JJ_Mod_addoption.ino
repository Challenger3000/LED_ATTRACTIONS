#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "color_selection.h"

const char* ssid = "ESP_TEST2";
const char* password = "";

AsyncWebServer server(80);

void setup() {
    Serial.begin(115200);

    // Set up Access Point
    WiFi.softAP(ssid, password);
    Serial.println("Access Point started");
    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP());

    // Serve the main index page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send_P(200, "text/html", indexx);
    });

    // Serve the color selection page
    server.on("/buttons", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send_P(200, "text/html", buttons);
    });

    // Serve the update page
    server.on("/update", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send_P(200, "text/html", update);
    });

    // Serve the JavaScript file for color picker
    server.on("/java_colors.js", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send_P(200, "application/javascript", java_colors);
    });

    server.on("/action", HTTP_GET, [](AsyncWebServerRequest* request) {
      String action = "";
      int r = -1, g = -1, b = -1;

      // Check if 'act' parameter exists and retrieve its value
      if (request->hasParam("act")) {
          action = request->getParam("act")->value();
      }

      // Check if 'r', 'g', and 'b' parameters exist and retrieve their values
      if (request->hasParam("r")) {
          r = request->getParam("r")->value().toInt();
      }
      if (request->hasParam("g")) {
          g = request->getParam("g")->value().toInt();
      }
      if (request->hasParam("b")) {
          b = request->getParam("b")->value().toInt();
      }

      // Log the received values to Serial
      Serial.printf("Action: %s, R: %d, G: %d, B: %d\n", action.c_str(), r, g, b);

      // Send a response back to the client
      request->send(200, "text/plain", "Received action and RGB values");
    });

    server.on("/gay", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/html", gayPage);
    });


    // Handle /actionGAY
    server.on("/actionGAY", HTTP_GET, [](AsyncWebServerRequest* request) {
        int speed = request->getParam("s")->value().toInt();
        int density = request->getParam("d")->value().toInt();
        int repeat = request->getParam("r")->value().toInt();
        int uniformity = request->getParam("u")->value().toInt();
        int brightness = request->getParam("b")->value().toInt();

        // Print slider values to Serial
        Serial.printf("Speed: %d, Density: %d, Repeat: %d, Uniformity: %d, Brightness: %d\n",
                      speed, density, repeat, uniformity, brightness);

        request->send(200, "text/plain", "Values received");
    });

    // Start the server
    server.begin();
}

void loop() {
    // Nothing to do here, handled by AsyncWebServer
    delay(10);
}
