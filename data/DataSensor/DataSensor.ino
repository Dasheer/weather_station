#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <SFE_BMP180.h>
#include <MechaQMC5883.h>
#include <Wire.h>
#include <elapsedMillis.h>
#include "DHT.h"

WiFiClient client;
HTTPClient http;
// Inicialización
// const char *ssid = "KBLEX.4.241";
// const char *password = "73746683";

const char *ssid = "9_Pro";
const char *password = "isc2023p";

// const char *serverDht = "http://192.168.1.131:5000/api/v1/sensor/dht";
// const char *serverPressure = "http://192.168.1.131:5000/api/v1/sensor/pressure";
// const char *serverAnemometro = "http://192.168.1.131:5000/api/v1/sensor/anemometro";
// const char *serverAnemometro = "http://192.168.1.131:5000/api/v1/sensor/wind_velocity";

const char *serverDht = "http://192.168.25.219:5000/api/v1/sensor/dht";
const char *serverPressure = "http://192.168.25.219:5000/api/v1/sensor/pressure";
const char *serverAnemometro = "http://192.168.25.219:5000/api/v1/sensor/anemometro";
const char *serverWindVelocity = "http://192.168.25.219:5000/api/v1/sensor/wind_velocity";

#define DHT22_PIN 14  // Digital pin connected to the DHT sensor
#define ALTITUDE 10.0

//Variables para dirección de viento
MechaQMC5883 qmc;  // crear objeto

int x, y, z;                // variables de los 3 ejes
String direction;           //Dirección
float acimut, geografico;   // variables para acimut magnetico y geografico
float declinacion = -0.58;  // declinacion desde pagina: http://www.magnetic-declination.com/

// Inicialización sensor DHT.
DHT dht22(DHT22_PIN, DHT22);

SFE_BMP180 pressure;
elapsedMillis timeElapsed;
unsigned long intervalSaveRequest = 10000;

//Variables para el Anemometro
#define sensorPin A0

// delay
#define delayMillis 1000

// Declaración de métodos
void saveSensorRecord(const char *, DHT);
void savePressure(const char *, SFE_BMP180);
void saveAnemometro(const char *, float sendVelocity);
void saveWindDirection(const char *, MechaQMC5883);


void setup() {
  // Set Baud Rate
  Wire.begin();
  Serial.begin(115200);
  Serial.println("REBOOT");

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

  if (pressure.begin()) {
    Serial.println("BMP180 init success");
  } else {
    Serial.println("BMP180 init failt");
    while (1)
      ;
  }

  analogReference(sensorPin);
  qmc.init();

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
      savePressure("pressure", pressure);
      saveAnemometro("anemometro");
      saveWindDirection("wind_direction", qmc);
    } else {
      Serial.println("WiFi Disconnected");
    }
    timeElapsed = 0;
  }
}

void saveSensorRecord(const char *sensor_type, DHT dht) {

  float dhtHumidity = dht.readHumidity();
  // Leer la temperatura en Celsius
  float dhtTempInC = dht.readTemperature();

  if (isnan(dhtHumidity) || isnan(dhtTempInC)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  float dhtHiC = dht.computeHeatIndex(dhtTempInC, dhtHumidity, false);

  //  Iniciamos la conexión al api
  http.begin(client, serverDht);
  //  Set content type to JSON
  http.addHeader("Content-Type", "application/json");

  // Write JSON of sensor readings
  StaticJsonDocument<200> doc;
  doc["sensor_type"] = sensor_type;
  doc["temperature"] = dhtTempInC;
  doc["humidity"] = dhtHumidity;
  String requestBody;
  serializeJson(doc, requestBody);

  // HTTP Post
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

void savePressure(const char *sensor_type, SFE_BMP180 pressure) {
  char status;
  double T, P, p0, a;

  float absolute_pressure;
  float compensated_pressure;
  float computed_altitude;

  status = pressure.startTemperature();
  if (status != 0) {
    delay(status);
    status = pressure.getTemperature(T);
    if (status != 0) {
      status = pressure.startPressure(3);
      if (status != 0) {
        delay(status);
        status = pressure.getPressure(P, T);
        if (status != 0) {
          absolute_pressure = pressure.sealevel(P, ALTITUDE);
          compensated_pressure = Serial.println(absolute_pressure);
          computed_altitude = a = pressure.altitude(P, absolute_pressure);
          Serial.println(a);
        }
      }
    }
  }

  //  Begin connection with your REST server
  http.begin(client, serverPressure);
  //  Set content type to JSON
  http.addHeader("Content-Type", "application/json");
  StaticJsonDocument<200> doc;
  doc["sensor_type"] = sensor_type;
  doc["altitude"] = ALTITUDE;
  doc["absolute_pressure"] = absolute_pressure;
  doc["compensated_pressure"] = compensated_pressure;
  doc["computed_altitude"] = computed_altitude;
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

void saveAnemometro(const char *sensor_type) {

  int totalReadings = 10;
  float velocity = 0.0;
  float velocityMax = 0.0;
  float value = 0.0;
  float sumReadings = 0.0;
  float lastValue = 0.0;
  float umbralReposo = 10.20;

  for (int i = 0; i < totalReadings; i++) {
    sumReadings += analogRead(sensorPin);
    delay(10);
  }

  value = sumReadings / totalReadings;

  // Aplicar filtro de media móvil
  value = (value * 0.8) + (lastValue * 0.2);
  lastValue = value;

  Serial.print("Valor filtrado: ");
  Serial.println(value);

  // Calibrar el valor en reposo antes del cálculo de velocidad
  if (value < umbralReposo) {
    value = 0.0;  // Ajustar el valor en reposo a 0
  }

  // Calcular la velocidad después de aplicar el umbral
  velocity = value * 0.190;

  Serial.print("Velocidad actual: ");
  Serial.print(velocity);
  Serial.println(" Km/h");

  if (velocity > velocityMax) {
    velocityMax = velocity;
    Serial.print("Velocidad máxima alcanzada: ");
    Serial.println(velocityMax, 2);
  }

  delay(delayMillis);

  {
    //  Begin connection with your REST server
    http.begin(client, serverAnemometro);
    //  Set content type to JSON
    http.addHeader("Content-Type", "application/json");
    StaticJsonDocument<200> doc;
    doc["sensor_type"] = sensor_type;
    doc["velocity"] = velocity;
    doc["velocityMax"] = velocityMax;
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
}

void saveWindDirection(const char *sensor_type, MechaQMC5883 qmc) {
  int x, y, z;                // variables de los 3 ejes
  String direction;           //Dirección
  float acimut, geografico;   // variables para acimut magnetico y geografico
  float declinacion = -0.58;  // declinacion desde pagina: http://www.magnetic-declination.com/

  qmc.read(&x, &y, &z, &acimut);  // lectura de los ejes y acimut magnetico

  geografico = acimut + declinacion;  // acimut geografico como suma del magnetico y declinacion
  if (geografico < 0)                 // si es un valor negativo
    geografico = geografico + 360;    // suma 360 y vuelve a asignar a variable

  if (geografico >= 337.5 || geografico < 22.5)
    direction = "Norte";
  else if (geografico >= 22.5 && geografico < 67.5)
    direction = "Noreste";
  else if (geografico >= 67.5 && geografico < 112.5)
    direction = "Este";
  else if (geografico >= 112.5 && geografico < 157.5)
    direction = "Sureste";
  else if (geografico >= 157.5 && geografico < 202.5)
    direction = "Sur";
  else if (geografico >= 202.5 && geografico < 247.5)
    direction = "Suroeste";
  else if (geografico >= 247.5 && geografico < 292.5)
    direction = "Oeste";
  else if (geografico >= 292.5 && geografico < 337.5)
    direction = "Noroeste";

  // muestra texto grados
  Serial.print("Acimut geografico: ");  // muestra texto Acimut geografico:
  Serial.print(geografico, 0);          // muestra valor de acimut geografico sin decimales
  Serial.print(" grados ");
  Serial.print("Dirección: ");
  Serial.println(direction);

  delay(delayMillis);

  {
    //  Begin connection with your REST server
    http.begin(client, serverWindVelocity);
    //  Set content type to JSON
    http.addHeader("Content-Type", "application/json");
    StaticJsonDocument<200> doc;
    doc["sensor_type"] = sensor_type;
    doc["geografico"] = geografico;
    doc["direction"] = direction;
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
}