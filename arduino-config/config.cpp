#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// WiFi
const char *ssid = "WIFI_NAME";
const char *password = "WIFI_PASS";

// API
const char *serverName = "http://IP_ADDRESS:8000/predict";

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

void setup()
{
  Serial.begin(115200);

  // WiFi connect
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");

  dht.begin();

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

    int aq = analogRead(MQ135_PIN);
    float temp = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(temp) || isnan(humidity))
    {
      Serial.println("DHT error");
      return;
    }

    Serial.println("AQ: " + String(aq));
    Serial.println("Temp: " + String(temp));
    Serial.println("Hum: " + String(humidity));

    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");

    // JSON create
    StaticJsonDocument<200> doc;
    doc["aq"] = aq;
    doc["temp"] = temp;
    doc["humidity"] = humidity;

    String jsonString;
    serializeJson(doc, jsonString);

    int httpResponseCode = http.POST(jsonString);

    String message = "No response";

    if (httpResponseCode > 0)
    {
      String response = http.getString();
      Serial.println(response);

      // Parse JSON
      StaticJsonDocument<200> resDoc;
      deserializeJson(resDoc, response);

      message = resDoc["message"].as<String>();
    }
    else
    {
      Serial.println("Error sending request");
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
    display.println(class, message);

    display.display();
  }

  delay(5000); // every 5 sec
}