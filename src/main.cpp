#include <functions.h>

#define RXD2 16 // Define RX pin for Serial2
#define TXD2 17 // Define TX pin for Serial2

void setup() {
    Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2); // Initiate the Serial2 UART connection at 9600 baud rate
    init(); // Call the init function to set up WiFi, SPIFFS, and the web server
}

// Function that runs when a new value is received at the SSE listener
void wsUpdate(int value)
{
    if (Serial2.availableForWrite()) // Check if Serial2 is available for writing
        Serial2.write(value); // Write the received value to Serial2
    Serial.println("WS(in): " + String(value)); // Print the received value to the Serial monitor
}

// Function that runs when a new value is received from Serial2
void rxUpdate()
{
    espRX = Serial2.read(); // Read the value from Serial2
    if (espRX != pEspRX) // Check if the received value is different from the previous value
    {
        rxEvent.send(String(espRX).c_str()); // Send the new value to the SSE clients
        pEspRX = espRX; // Update the previous value
    }
}

void loop() {
    if (Serial2.available() > 0) // Check if there is data available to read from Serial2
        rxUpdate(); // Call the rxUpdate function to handle the received data
}