#include <FS.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>
//#include <Adafruit_BME280.h>
#include <Adafruit_BMP280.h>
#include <LittleFS.h>

// Remplacez ces valeurs par les vôtres
const char *ssid = "Esp32";
const char *password = "Laplateforme.io";
const char* host = "espmeteo";

Adafruit_BMP280 bmp; // use I2C interface

Adafruit_Sensor *bmp_temp = bmp.getTemperatureSensor();
Adafruit_Sensor *bmp_pressure = bmp.getPressureSensor();
//Adafruit_BME280 bme;
AsyncWebServer server(80);

void setup() {
  Serial.begin(9600);
 unsigned status;
  //status = bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID);
  status = bmp.begin(0x76);
  if (!status) {
    Serial.println(F("Could not find a valid BME280 sensor, check wiring or "
                      "try a different address!"));
    Serial.print("SensorID was: 0x"); Serial.println(bmp.sensorID(),16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1) delay(10);
  }
  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

  bmp_temp->printSensorDetails();
  // Montage du système de fichiers LittleFS
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    return;
  }

  // Configurer le point d'accès WiFi 
  
  WiFi.softAP(ssid, password);

  if (!MDNS.begin(host))  // http://ESP32OTA.local
    Serial.println("Error setting up MDNS responder!");   
  else
    Serial.printf("mDNS responder started. Hotstname = http://%s.local\n", host);

  // Configurer les routes du serveur Web
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    // Lire le contenu de la page Web depuis LittleFS
    File file = LittleFS.open("/index.html", "r");
    if (file) {
      request->send(LittleFS, "/index.html", "text/html");
      file.close();
    } else {
      request->send(404, "text/plain", "File not found");
    }
  }
  );

  // Configurer une route pour obtenir les données du capteur
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
    // Mesurer la température, l'humidité et la pression atmosphérique
 sensors_event_t temp_event, pressure_event;
bmp_temp->getEvent(&temp_event);
  bmp_pressure->getEvent(&pressure_event);
    float temperature =temp_event.temperature; //21.5; // bme.readTemperature();
    float humidity = 0;     //bme.readHumidity();
    float pressure = pressure_event.pressure; //800.1;   //bme.readPressure() / 100.0F; // Conversion en hPa

    // Construire la réponse JSON
    String json = "{\"temperature\": " + String(temperature) +
                  ", \"humidity\": " + String(humidity) +
                  ", \"pressure\": " + String(pressure) + "}";

    // Envoyer la réponse JSON
    request->send(200, "application/json", json);
  });

  // Démarrer le serveur Web
  server.begin();
}

void loop() {
  // Rien à faire dans la boucle loop si vous utilisez le serveur Web Async
}

