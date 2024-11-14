#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <settings.h>  // Include custom settings for WiFi credentials (apSSID, apPassword)

int espRX;       // Variable to store RX data
int pEspRX = 0;  // Previous RX data for change detection

// WebServer setup
AsyncWebServer server(80);       // Set up server on port 80
AsyncEventSource rxEvent("/rx"); // Event source endpoint for server-sent events

// Function declarations
void wsUpdate(int value); // Function to handle WebSocket data updates
void rxUpdate();          // Function to handle RX data updates

// Initialize everything: WiFi, SPIFFS, and Web server
void init()
{
    Serial.begin(115200); // Start serial communication at 115200 baud

    // Initialize SPIFFS for file storage
    if (!SPIFFS.begin(true)) {
        Serial.println("An Error has occurred while mounting SPIFFS"); // Error handling if SPIFFS fails
        return;
    }

    // Set up WiFi Access Point with credentials from settings.h
    WiFi.softAP(apSSID, apPassword);
    IPAddress IP = WiFi.softAPIP(); // Get the IP address of the ESP32 in AP mode
    Serial.print("AP IP Address: ");
    Serial.println(IP); // Print AP IP address to serial

    // Serve static files from SPIFFS (e.g., HTML, CSS, JavaScript)
    server.serveStatic("/", SPIFFS, "/")
        .setDefaultFile("index.html") // Default file to serve when accessing root URL
        .setCacheControl("max-age=600"); // Set caching for static files to 10 minutes

    // Server-Sent Events (SSE) handling for connected clients
    rxEvent.onConnect([](AsyncEventSourceClient *client) {
        client->send("Connected to RX", NULL, millis(), 1000); // Send initial message upon connection
    });
    server.addHandler(&rxEvent); // Add event source handler to the server

    // Endpoint to receive HTTP POST requests for value updates
    server.on("/sendToESP", HTTP_POST, [](AsyncWebServerRequest *request)
    {
        if (request->hasParam("value", true)) // Check if "value" parameter exists in POST request
        {
            String value = request->getParam("value", true)->value(); // Retrieve "value" parameter
            wsUpdate(value.toInt()); // Process the value received
        }
        request->send(200, "text/plain", "Value received"); // Respond to the client with a success message
    });

    // Start the web server to begin handling requests
    server.begin();
}
