#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>

// Define RC522 pins
#define SS_PIN D4  // SDA on RC522 to D4 on NodeMCU (GPIO2)
#define RST_PIN D3 // RST on RC522 to D3 on NodeMCU (GPIO0)

// Define buzzer pin
#define BUZZER_PIN D2  // Connect active buzzer to D2 (GPIO4)

// Create RFID reader instance
MFRC522 rfid(SS_PIN, RST_PIN);

// WiFi credentials
const char* ssid = "P";
const char* password = "stranger";

// Flask backend endpoint
const String serverUrl = "http://192.168.75.29:5000/scan_entry";  // Make sure this matches your local Flask IP

void setup() {
  Serial.begin(115200);
  SPI.begin();            // SCK = D5, MOSI = D7, MISO = D6
  rfid.PCD_Init();        // Init MFRC522
  Serial.println("RFID Scanner Ready");

  pinMode(BUZZER_PIN, OUTPUT);   // Setup buzzer
  digitalWrite(BUZZER_PIN, LOW); // Make sure buzzer is off

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Wait for a new RFID card
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    delay(100);
    return;
  }

  // Read UID from card
  String rfid_uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    rfid_uid += String(rfid.uid.uidByte[i], HEX);
  }
  rfid_uid.toUpperCase(); // Make UID uppercase for consistency

  Serial.println("RFID Scanned: " + rfid_uid);

  // Send UID to Flask server
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client;

    http.begin(client, serverUrl); // Use WiFiClient with begin
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"rfid\":\"" + rfid_uid + "\"}";
    int httpCode = http.POST(payload);

    if (httpCode > 0) {
      Serial.println("Response Code: " + String(httpCode));
      String response = http.getString();
      Serial.println("Response: " + response);

      // ✅ Beep for 2 seconds if response is successful (200 OK)
      if (httpCode == 201) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(100);
        digitalWrite(BUZZER_PIN, LOW);
      }

    } else {
      Serial.println("Failed to send request. HTTP Code: " + String(httpCode));
    }

    http.end();
  } else {
    Serial.println("WiFi disconnected. Trying to reconnect...");
    WiFi.begin(ssid, password);
  }

  delay(2000); // Wait before next scan
  rfid.PICC_HaltA();        // Halt PICC
  rfid.PCD_StopCrypto1();   // Stop encryption
}