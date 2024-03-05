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
#include <boton.h>

const char* ssid = "LINK";
const char* password = "MEMECACATROTRONINICACA2020";

ESP12f esp = ESP12f();

//Conf Valvulas y riego
const int numValvulas = 6;
int pinesValvulas[] = {16, 14, 12, 13, 15, 0};
valvula valvulas[] = { //Inicialización del objeto valvulas (Uso los dos relays restantes de la placa para no joder el server web)
  valvula(pinesValvulas[0]),
  valvula(pinesValvulas[1]),
  valvula(pinesValvulas[2]),
  valvula(pinesValvulas[3]),
  valvula(pinesValvulas[4]),
  valvula(pinesValvulas[5])
};
//Temporización
unsigned long previousMillis = 0;     
const long interval = 10000;
int tiemposRiego[] = {5, 3, 7, 2, 2, 4}; //minutos
int horaRiego = 17;
int minutoRiego = 5;
int fechaHora[] = {0,0,0};

//Control Manual
boton controlManual;


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
// Create a WebSocket object
AsyncWebSocket ws("/ws");


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

String getValveStates(){
  JSONVar myArray;
  for (int i =0; i<numValvulas; i++){
    myArray["valves"][i]["num"] = String(valvulas[i]._numValvula);
    myArray["valves"][i]["state"] = String(valvulas[i]._state);
  }
  String jsonString = JSON.stringify(myArray);
  return jsonString;
}

void notifyClients(String state) {
  ws.textAll(state);
}

//run whenever we receive new data from the clients via the WebSocket protocol. The client will send the "states" message to request the current valve states or a message containing the GPIO number to change the state.
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "states") == 0) { //If we receive the "states" message, send a message to all clients with the state of all valves using the notifyClients function. 
      notifyClients(getValveStates());
    }
    else{ //If the message is not "states", it means we’ve received a valveNumber and we want to toggle its state
      int valveNumber = atoi((char*)data);
      if (!valvulas[valveNumber]._state){
        valvulas[valveNumber].activar();
      }
      else{
        valvulas[valveNumber].desactivar();
      }
      notifyClients(getValveStates());
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
  
  server.begin(); // Start server
}

void loop() {
  ws.cleanupClients();
  AsyncElegantOTA.loop(); 

  if (!controlManual._state){ //Consulta boton controlManual para riego automático
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