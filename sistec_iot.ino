// ============================================
// SISTec IoT Application 2026
// ESP8266 Arduino Code
// DHT11 Sensor + 16x2 LCD (I2C)
// ============================================

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ---- WiFi Credentials ----
const char* ssid     = "Ansh";       // Change this
const char* password = "123456789";   // Change this

// ---- Render Server URL ----
// Replace with your actual Render deployment URL
const char* serverURL = "https://newiot.onrender.com";

// ---- DHT11 Sensor Settings ----
#define DHTPIN D5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ---- LCD Settings ----
// SDA = D2, SCL = D1
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---- Degree symbol for LCD ----
byte degreeChar[8] = {
  0b00110,
  0b01001,
  0b01001,
  0b00110,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Setup LCD
  Wire.begin(D2, D1); // SDA=D2, SCL=D1
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, degreeChar); // Register degree symbol

  // ---- Screen 1: Connecting to WiFi ----
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CONNECTING TO");
  lcd.setCursor(0, 1);
  lcd.print("WiFi");

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    lcd.print(".");
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");

  // ---- Screen 2: Connected ----
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CONNECTED TO");
  lcd.setCursor(0, 1);
  lcd.print("WiFi --WELCOME--");
  delay(2000);
}

void loop() {
  // Read DHT11 Sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Check if sensor reading is valid
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("DHT11 read failed!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SENSOR ERROR");
    delay(2000);
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" C | Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // ---- Screen 3: Temperature ----
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TEMPERATURE");
  lcd.setCursor(0, 1);
  lcd.print(temperature, 1);
  lcd.write(0); // Degree symbol
  lcd.print("C");
  delay(2000);

  // ---- Screen 4: Humidity ----
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("HUMIDITY");
  lcd.setCursor(0, 1);
  lcd.print(humidity, 1);
  lcd.print(" %");
  delay(2000);

  // ---- Screen 5: Fetch LCD Text from Server ----
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SISTec DISPLAY");
  lcd.setCursor(0, 1);
  lcd.print("Fetching...");

  String lcdText = fetchLCDText();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SISTec DISPLAY");
  lcd.setCursor(0, 1);
  lcd.print(lcdText);
  delay(3000);

  // ---- Screen 6: Sending Data ----
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SENDING DATA TO");
  lcd.setCursor(0, 1);
  lcd.print("WEB SERVER....");

  // Send sensor data to server
  bool sent = sendSensorData(temperature, humidity);

  // ---- Screen 7: Data Sent ----
  lcd.clear();
  lcd.setCursor(0, 0);
  if (sent) {
    lcd.print("DATA SENT...!!");
  } else {
    lcd.print("SEND FAILED!");
  }
  delay(1000);
}


// ============================================
// FUNCTION: Send Temperature & Humidity to Server
// ============================================
bool sendSensorData(float temperature, float humidity) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected!");
    return false;
  }

  // Build URL: /api/save?temperature=25.5&humidity=56.2
  String url = String(serverURL) + "/api/save?temperature=" + String(temperature, 1) + "&humidity=" + String(humidity, 1);
  Serial.println("Sending to: " + url);

  // Use BearSSL for HTTPS (Render uses HTTPS only)
  BearSSL::WiFiClientSecure client;
  client.setInsecure(); // Skip SSL certificate verification (easier for beginners)

  HTTPClient https;
  https.begin(client, url);

  int httpCode = https.GET();
  String response = https.getString();
  https.end();

  Serial.print("HTTP Code: ");
  Serial.println(httpCode);
  Serial.println("Response: " + response);

  return (httpCode == 200);
}


// ============================================
// FUNCTION: Fetch LCD Text from Server
// ============================================
String fetchLCDText() {
  if (WiFi.status() != WL_CONNECTED) {
    return "No WiFi";
  }

  String url = String(serverURL) + "/api/lcd";
  Serial.println("Fetching LCD text from: " + url);

  BearSSL::WiFiClientSecure client;
  client.setInsecure(); // Skip SSL verification

  HTTPClient https;
  https.begin(client, url);

  int httpCode = https.GET();
  String lcdText = "No Data";

  if (httpCode == 200) {
    lcdText = https.getString();
    lcdText.trim();
    // Limit to 16 characters for LCD
    if (lcdText.length() > 16) {
      lcdText = lcdText.substring(0, 16);
    }
  }
  https.end();

  Serial.println("LCD Text: " + lcdText);
  return lcdText;
}
