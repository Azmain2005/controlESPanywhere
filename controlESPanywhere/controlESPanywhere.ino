#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Azmain's megamind";
const char* password = "Abusk.Megamind";

// Replace this with your server's local IP address and path
const char* serverName = "http://192.168.31.168/espTest/control_led.php";

const int LED_BUILTIN = 2; // GPIO pin for the built-in LED

void setup() {
  Serial.begin(115200);
  delay(100); // Short delay for serial monitor to initialize

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to WiFi...");
    delay(1000);
  }
  Serial.println("Connected to WiFi");

  // Set built-in LED pin as output
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Begin HTTP connection
    if (http.begin(serverName)) {
      // Send HTTP GET request
      int httpResponseCode = http.GET();

      if (httpResponseCode > 0) {
        String payload = http.getString();
        Serial.print("Received payload: ");
        Serial.println(payload);

        // Update LED state based on the server response
        if (payload == "FEED") {
          digitalWrite(LED_BUILTIN, HIGH);
          Serial.println("LED turned ON");
          delay(2000); // Wait for 2 seconds
          digitalWrite(LED_BUILTIN, LOW);
          Serial.println("LED turned OFF");

          // Clear the state in the database
          http.begin(serverName);
          http.addHeader("Content-Type", "application/x-www-form-urlencoded");
          int clearResponse = http.POST("led_state=");
          Serial.print("Clear response: ");
          Serial.println(clearResponse);
          http.end();
        } else {
          Serial.println("Invalid payload");
        }
      } else {
        Serial.print("Error on HTTP request: ");
        Serial.println(httpResponseCode);
        Serial.println(http.errorToString(httpResponseCode).c_str());
      }

      // End HTTP connection
      http.end();
    } else {
      Serial.println("Unable to connect to server");
    }
  } else {
    Serial.println("Disconnected from WiFi");
    // Attempt to reconnect WiFi if disconnected (optional)
    WiFi.reconnect();
  }

  // Wait for 10 seconds before the next request
  delay(100); // Adjust delay as needed for your application
}
