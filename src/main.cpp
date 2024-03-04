#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#include <Arduino_JSON.h>
#include <AsyncElegantOTA.h>

#include <NTPClient.h>
#include <valvula.h>
#include <ESP12f.h>

const char* ssid = "Pablo";
const char* password = "436143028";

ESP12f esp = ESP12f();

//Conf Valvulas y riego
const int numValvulas = 2;
int pinesValvulas[] = {4, 5};
valvula valvulas[] = { //Inicialización del objeto valvulas (Uso los dos relays restantes de la placa para no joder el server web)
  valvula(pinesValvulas[0]),
  valvula(pinesValvulas[1])
};
//Temporización
unsigned long previousMillis = 0;     
const long interval = 1000;
int tiemposRiego[] = {2, 3}; //minutos
int horaRiego = 16;
int minutoRiego = 11;
int fechaHora[] = {0,0,0};
//Control Manual
boolean controlManual = false;  //automático por defecto


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
// Create a WebSocket object
AsyncWebSocket ws("/ws");

// Set number of outputs
#define NUM_OUTPUTS 6
// Assign each GPIO to an output
int outputGPIOs[NUM_OUTPUTS] = {16, 14, 12, 13, 15, 0}; //GPIO 16&15 detalle inicio.

// Initialize LittleFS
void initFS() {
if (!LittleFS.begin()) {
Serial.println("An error has occurred while mounting LittleFS");
}
Serial.println("LittleFS mounted successfully");
}

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
Serial.println(WiFi.localIP());
}

String getOutputStates(){
  JSONVar myArray;
  for (int i =0; i<NUM_OUTPUTS; i++){
    myArray["gpios"][i]["output"] = String(outputGPIOs[i]);
    myArray["gpios"][i]["state"] = String(digitalRead(outputGPIOs[i]));
  }
  String jsonString = JSON.stringify(myArray);
  return jsonString;
}

void notifyClients(String state) {
ws.textAll(state);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "states") == 0) {
      notifyClients(getOutputStates());
    }
    else{
      int gpio = atoi((char*)data);
      digitalWrite(gpio, !digitalRead(gpio));
      notifyClients(getOutputStates());
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void setup() {
  
  // Set GPIOs as outputs
  for (int i =0; i<NUM_OUTPUTS; i++){
    pinMode(outputGPIOs[i], OUTPUT);
    digitalWrite(outputGPIOs[i],LOW);
  }
  //setea válvulas usando objetos (Solo 2)
  for (int i=0; i<numValvulas;i++){ //asigna parametros de riego a cada valvula
    valvulas[i].asignaParametros(i, tiemposRiego[i], horaRiego, minutoRiego);
  }

  initFS();
  initWiFi();
  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", "text/html",false);
  });
  server.serveStatic("/", LittleFS, "/");
  AsyncElegantOTA.begin(&server); // Start ElegantOTA
  // Start server
  server.begin();
}

void loop() {
  ws.cleanupClients();
  AsyncElegantOTA.loop();

  if (!controlManual){ //pruebaBlinkeo
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      esp.fechaHora();
      fechaHora[0] = esp.obtenerDia();
      fechaHora[1] = esp.obtenerHora();
      fechaHora[2] = esp.obtenerMinutos();
  
      for (int i = 0; i < numValvulas; i++){
        valvulas[i].compruebaRiego(fechaHora);
      }
    }
  }  
}