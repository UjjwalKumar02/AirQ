#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

float normalizeMQ(int rawValue)
{
  // Map 0–4095 → 0–400
  return (rawValue / 4095.0) * 400.0;
}

// WiFi
const char *ssid = "WIFI_NAME";
const char *password = "WIFI_PASS";

// API
const char *serverName = "http://IP_ADDRESS/predict";

// MQ135
#define MQ135_PIN 34

// DHT22
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Variables
String airClass = "Unknown";
String message = "Starting...";

void setup()
{
  Serial.begin(115200);

  // I2C init
  Wire.begin(21, 22);

  // WiFi connect
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");

  // Sensors init
  dht.begin();

  // Warm-up time
  delay(5000);

  // OLED init
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("OLED failed");
    while (true)
      ;
  }

  display.clearDisplay();
}

void loop()
{
  if (WiFi.status() == WL_CONNECTED)
  {

    // Read sensors
    int aqReading = analogRead(MQ135_PIN);
    float temp = dht.readTemperature();
    float humidity = dht.readHumidity();

    // Retry DHT if failed
    if (isnan(temp) || isnan(humidity))
    {
      Serial.println("DHT error, retrying...");
      delay(2000);

      temp = dht.readTemperature();
      humidity = dht.readHumidity();

      if (isnan(temp) || isnan(humidity))
      {
        Serial.println("DHT failed again, using default values");
        temp = 0;
        humidity = 0;
      }
    }

    float aq = normalizeMQ(aqReading);

    // Debug prints
    Serial.println("AQ: " + String(aq));
    Serial.println("Raw MQ: " + String(aqReading));
    Serial.println("Temp: " + String(temp));
    Serial.println("Hum: " + String(humidity));

    // HTTP request
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");

    // Create JSON
    StaticJsonDocument<200> doc;
    doc["aq"] = aq;
    doc["temp"] = temp;
    doc["humidity"] = humidity;

    String jsonString;
    serializeJson(doc, jsonString);

    int httpResponseCode = http.POST(jsonString);

    Serial.println("HTTP Code: " + String(httpResponseCode));

    if (httpResponseCode > 0)
    {
      String response = http.getString();
      Serial.println("Response: " + response);

      // Parse JSON
      StaticJsonDocument<200> resDoc;
      DeserializationError error = deserializeJson(resDoc, response);

      if (!error)
      {
        airClass = resDoc["class"].as<String>();
        message = resDoc["message"].as<String>();
      }
      else
      {
        Serial.println("JSON parse failed");
        airClass = "Error";
        message = "Parse failed";
      }
    }
    else
    {
      Serial.println("Error sending request");
      airClass = "No API";
      message = "Check server";
    }

    http.end();

    // OLED display
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);

    display.setCursor(0, 0);
    display.println("AQ: " + String(aq));

    display.setCursor(0, 10);
    display.println("Temp: " + String(temp));

    display.setCursor(0, 20);
    display.println("Hum: " + String(humidity));

    display.setCursor(0, 35);
    display.println("Status:");

    display.setCursor(0, 45);
    display.println(airClass);
    display.println(message);

    display.display();
  }
  else
  {
    Serial.println("WiFi Disconnected");
  }

  delay(5000); // every 5 sec
}