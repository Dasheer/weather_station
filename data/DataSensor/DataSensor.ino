#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <elapsedMillis.h>
#include "DHT.h"

const char *ssid = "KBLEX.4.241";
const char *password = "73746683";

// const char *ssid = "9_Pro";
// const char *password = "isc2023p";

// const char *serverName = "http://localhost:5000/api/v1/sensor/dht";
// const char *serverName = "http://192.168.1.131:5000/api/v1/sensor/dht/stream";
const char *serverName = "http://192.168.1.131:5000/api/v1/sensor/dht";

#define DHT22_PIN 14  // Digital pin connected to the DHT sensor

// Initialize DHT sensor.
DHT dht22(DHT22_PIN, DHT22);

elapsedMillis timeElapsed;
unsigned long intervalSaveRequest = 10000;

// Forward declaration of saving sensor readings
void saveSensorRecord(const char *, DHT);

void setup() {
  // Set Baud Rate
  Serial.begin(115200);

  // Begin WiFi connection
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  // Begin communicating with DHT sensors
  dht22.begin();
  // dht11.begin();
}

void loop() {
  if (timeElapsed >= intervalSaveRequest) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Saving sensor records...");
      // Save sensor records, first parameter is the name or position of the sensor
      saveSensorRecord("dht", dht22);
    } else {
      Serial.println("WiFi Disconnected");
    }
    timeElapsed = 0;
  }
}

void saveSensorRecord(const char *sensor_type, DHT dht) {

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float dhtHumidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float dhtTempInC = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  // float dhtTempInF = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(dhtHumidity) || isnan(dhtTempInC)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  // float dhtHiF = dht.computeHeatIndex(dhtTempInF, dhtHumidity);
  // Compute heat index in Celsius (isFahreheit = false)
  float dhtHiC = dht.computeHeatIndex(dhtTempInC, dhtHumidity, false);

  WiFiClient client;
  HTTPClient http;
  //  Begin connection with your REST server
  http.begin(client, serverName);
  //  Set content type to JSON
  http.addHeader("Content-Type", "application/json");

  // Write JSON of sensor readings
  StaticJsonDocument<200> doc;
  doc["sensor_type"] = sensor_type;
  doc["temperature"] = dhtTempInC;
  // doc["temperature_in_f"] = dhtTempInF;
  doc["humidity"] = dhtHumidity;
  // doc["heat_index_in_c"] = dhtHiC;
  // doc["heat_index_in_f"] = dhtHiF;
  String requestBody;
  serializeJson(doc, requestBody);

  // HTTP Post to REST server
  int httpResponseCode = http.POST(requestBody);

  if (httpResponseCode > 0) {

    String response = http.getString();  // Get the response to the request
    Serial.println(httpResponseCode);    // Print return code
    Serial.println(response);            // Print request answer

    if (httpResponseCode == 200) {
      Serial.println("Registro del sensor enviado con éxito");
    } else {
      Serial.println("Error al enviar el POST");
    }
  } else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}