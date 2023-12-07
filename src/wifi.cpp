#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <LittleFS.h>

#define LED_BUILTIN 2
// Replace with your network credentials

const char *ssid = "meteostation";
const char *password = "meteostation";


  AsyncWebServer server(80);

const char *hostname = "meteoesp32"; // Nom d'hôte pour mDNS
/*
const char *ssid = "Xcam";
const char *password = "laplateforme.io";
*/
// Create an instance of the BME280 sensor
Adafruit_BME280 sensor;
#define I2CAddr 0x76

void onRequestBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
  // Gérer le corps de la requête ici si nécessaire
}

// Initialise LittleFS
void initLittleFS() {
  Serial.println("Initialisation de LittleFS...");
  if (!LittleFS.begin()) {
    Serial.println("Erreur lors de l'initialisation de LittleFS");
    while (true);
  }

  Serial.println("LittleFS initialisé !");
}

void setup() {
  unsigned status;
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT); digitalWrite(LED_BUILTIN, LOW);// led bleu OFF

  //ESP32 en mode "access point"
  WiFi.softAP(ssid, password);
  Serial.println("Access Point Started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

/*
// Connectez-vous au réseau Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connexion au WiFi...");
  }
  Serial.println("Connecté au WiFi");
*/
  // Initialisez mDNS
  if (!MDNS.begin(hostname)) {
    Serial.println("Échec de l'initialisation mDNS");
    return;
  }
  Serial.println("mDNS initialisé");

 initLittleFS();
//////////////



  // Initialize the BME280 sensor
     status = sensor.begin(I2CAddr);
    if (!status) {
        Serial.println("Pas de capteur trouvé !");
        Serial.print("SensorID : 0x"); Serial.println(sensor.sensorID(),16);
        Serial.print("        ID of 0xFF mauvaise addresse, BMP 180 ou BMP 085\n");
        Serial.print("   ID of 0x56-0x58 BMP 280 ?,\n");
        Serial.print("        ID of 0x60 BME 280 ?\n");
        Serial.print("        ID of 0x61 BME 680 ?\n");
        #ifdef OLED 
        // display.setCursor(0,0);
        if ( SCREEN_HEIGHT == 64 ){
          display.setCursor(0,32);
        }
          display.print("Pas de capteur !\n");
          display.print("SensorID :");
          display.print(sensor.sensorID(),16);
          display.display();
        #endif
        while (1) delay(10);
    }

 Serial.println("Initialisation du serveur web...");
// Chargement des fichiers html / javascript / css / ico
  server.on("/",                HTTP_GET, [](AsyncWebServerRequest *request) { request->send(LittleFS, "/index.html", "text/html");  });
  server.on("/simple.html",     HTTP_GET, [](AsyncWebServerRequest *request) { request->send(LittleFS, "/simple.html", "text/html");  });
  server.on("/index2.html",     HTTP_GET, [](AsyncWebServerRequest *request) { request->send(LittleFS, "/index2.html", "text/html");  });
  server.on("/index3.html",     HTTP_GET, [](AsyncWebServerRequest *request) { request->send(LittleFS, "/index3.html", "text/html");  });
  server.on("/style.css",       HTTP_GET, [](AsyncWebServerRequest *request) { request->send(LittleFS, "/style.css", "text/css");  });
  server.on("/favicon.ico" ,    HTTP_GET, [](AsyncWebServerRequest *request) { request->send(LittleFS, "/favicon.ico", "image/x-icon");  });
  server.on("/404.png" ,        HTTP_GET, [](AsyncWebServerRequest *request) { request->send(LittleFS, "/404.png", "image/png");  });
  server.on("/raphael.min.js",  HTTP_GET, [](AsyncWebServerRequest *request) { request->send(LittleFS, "/raphael.min.js", "application/javascript");  });
  server.on("/justgage.js",     HTTP_GET, [](AsyncWebServerRequest *request) { request->send(LittleFS, "/justgage.js", "application/javascript");  });
  server.on("/chart.js",        HTTP_GET, [](AsyncWebServerRequest *request) { request->send(LittleFS, "/chart.js", "application/javascript");  });
  server.on("/highcharts.js",   HTTP_GET, [](AsyncWebServerRequest *request) { request->send(LittleFS, "/highcharts.js", "application/javascript");  });
  server.on("/accessibility.js",HTTP_GET, [](AsyncWebServerRequest *request) { request->send(LittleFS, "/accessibility.js", "application/javascript");  });
  server.on("/style3.css",      HTTP_GET, [](AsyncWebServerRequest *request) { request->send(LittleFS, "/style.css", "text/css");  });
  server.onNotFound(                     [](AsyncWebServerRequest *request) { request->send(LittleFS, "/404.html", "text/html");  });


/*
server.on("/index3.html", HTTP_GET, [](AsyncWebServerRequest *request){  AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/index3.html");
  response->addHeader("Content-Encoding", "gzip");
  request->send(response);
  });
  */
  
  // Serve the sensor data as JSON
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
    String temperature = String(sensor.readTemperature());
    String pressure = String(sensor.readPressure() / 100.0F);
    String humidity = String(sensor.readHumidity());
    String data = "{\"temperature\":" + temperature + ",\"pressure\":" + pressure + ",\"humidity\":" + humidity + "}";
    request->send(200, "application/json", data);
    Serial.println(data);
    });

  
  // Activer la compression Gzip
  server.onRequestBody(onRequestBody);  
  // Démarre le serveur web
  server.begin();
  Serial.println("Serveur web démarré !");
  digitalWrite(LED_BUILTIN, HIGH) ; // si ok on allume led integré
  
}

void loop() {
  // Nothing to do here
}

