#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>

// WiFi Credentials
const char* ssid = "P";
const char* password = "stranger";

// Backend endpoints
const String verifyUrl = "http://192.168.75.29:5000/verify_order";
const String skipUrl = "http://192.168.75.29:5000/proceed_next_order";

// RC522 Pins
#define SS_PIN D4   // GPIO2
#define RST_PIN D3  // GPIO0
MFRC522 rfid(SS_PIN, RST_PIN);

// IO Pins
#define GREEN_LED D1    // GPIO5
#define RED_LED D8      // GPIO15
#define BUTTON_PIN D0   // GPIO16 (no interrupt)
#define BUZZER_PIN D2   // GPIO4

unsigned long lastButtonPress = 0;
const unsigned long debounceDelay = 500;

void setup() {
  Serial.begin(115200);
  SPI.begin();       // RC522 uses D5 (SCK), D6 (MISO), D7 (MOSI)
  rfid.PCD_Init();

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // Active LOW

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  // WiFi Connect
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nConnected! IP: " + WiFi.localIP().toString());
}

void loop() {
  // Check for RFID tag
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String rfid_uid = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      rfid_uid += String(rfid.uid.uidByte[i], HEX);
    }
    rfid_uid.toUpperCase();
    Serial.println("Scanned RFID: " + rfid_uid);

    // Send to backend for verification
    sendVerification(rfid_uid);

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    delay(2000);  // Delay between scans
  }

// Debounced button press to skip order
static bool buttonPreviouslyPressed = false;
bool buttonPressed = (digitalRead(BUTTON_PIN) == LOW);
unsigned long currentTime = millis();

if (buttonPressed && !buttonPreviouslyPressed && (currentTime - lastButtonPress > debounceDelay)) {
  // Button just pressed and debounce delay passed
  Serial.println("Skip button pressed!");
  skipToNextOrder();
  lastButtonPress = currentTime;
  buttonPreviouslyPressed = true;
} else if (!buttonPressed && buttonPreviouslyPressed) {
  // Button released
  buttonPreviouslyPressed = false;
}

}

void sendVerification(String uid) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected.");
    return;
  }

  HTTPClient http;
  WiFiClient client;
  http.begin(client, verifyUrl);
  http.addHeader("Content-Type", "application/json");

  String payload = "{\"rfid\":\"" + uid + "\"}";
  int httpCode = http.POST(payload);

  if (httpCode > 0) {
    String response = http.getString();
    Serial.println("Server Response: " + response);

    if (httpCode == 200) {
  Serial.println("Verification successful");
  blinkGreenLED();
}else {
      errorFeedback();    // red LED blink
    }
  } else {
    Serial.println("HTTP Request failed: " + String(httpCode));
    errorFeedback();
  }
  http.end();
}

void skipToNextOrder() {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  WiFiClient client;
  http.begin(client, skipUrl);
  int httpCode = http.POST("");  // No payload needed

  if (httpCode > 0) {
    Serial.println("Skip Request Sent. Code: " + String(httpCode));
  } else {
    Serial.println("Failed to send skip request.");
  }
  http.end();
}

void successFeedback() {
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(500);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BUZZER_PIN, LOW);
}
void blinkGreenLED() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(GREEN_LED, HIGH);
    delay(150);
    digitalWrite(GREEN_LED, LOW);
    delay(150);
  }
}
void errorFeedback() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(RED_LED, HIGH);
    delay(200);
    digitalWrite(RED_LED, LOW);
    delay(200);
  }
}