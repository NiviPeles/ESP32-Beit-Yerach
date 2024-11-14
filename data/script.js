const textOutput1 = document.getElementById("textOutput1");

/**
 * Sends a control value to the ESP32 through the sendToESP endpoint.
 * @param {number} value - The control value to send.
 */
function sendValue(value) {
  if (value !== "") {
    sendDataToESP(value); // Calls the function to send data to ESP
    printToLog(value, "esp-direct-out"); // Logs the action for the user
  }
}

/**
 * Sends data to ESP32 at the specified IP address.
 * @param {number} value - The control value to be sent to the ESP.
 */
function sendDataToESP(value) {
  fetch(`http://192.168.4.1/sendToESP`, { // Endpoint for sending data to ESP32
    method: "POST",
    body: new URLSearchParams({ value: value }),
    headers: {
      "Content-Type": "application/x-www-form-urlencoded",
    },
  }).catch(error => {
    printToLog("Failed to send data: " + error.message, "error"); // Error handling
  });
}

/**
 * Sets up an EventSource to receive data from ESP32 at /rx.
 */
var rxSource = new EventSource(`http://192.168.4.1/rx`); // SSE endpoint for receiving data

/**
 * Handles incoming messages from ESP32 through the EventSource connection.
 */
rxSource.onmessage = function (event) {
  const localRX = event.data;
  textOutput1.textContent = localRX; // Updates the display with received data
  printToLog(localRX, "esp-direct-in"); // Logs the received data
};