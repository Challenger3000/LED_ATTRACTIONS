#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "color_selection.h"

const char* ssid = "DRSSTC II";
const char* password = "";

AsyncWebServer server(80);

void wifi_init(){    
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
    currentMode = SOLID;


    // Send a response back to the client
    request->send(200, "text/plain", "Received action and RGB values");
  });

  server.on("/gay", HTTP_GET, [](AsyncWebServerRequest* request) {
  request->send_P(200, "text/html", gayPage);
  });


  // Handle /actionGAY
  server.on("/actionGAY", HTTP_GET, [](AsyncWebServerRequest* request) {
      speed = request->getParam("s")->value().toInt();
      density = request->getParam("d")->value().toInt();
      repeat = request->getParam("r")->value().toInt();
      uniformity = request->getParam("u")->value().toInt();
      brightness = request->getParam("b")->value().toInt();
      currentMode = GAY;


      // Print slider values to Serial
      Serial.printf("Speed: %d, Density: %d, Repeat: %d, Uniformity: %d, Brightness: %d\n",
                    speed, density, repeat, uniformity, brightness);

      request->send(200, "text/plain", "Values received");

      recalcCornerXYZ();

  });

  // Start the server
  server.begin();
}