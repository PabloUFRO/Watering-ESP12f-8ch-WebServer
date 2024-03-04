#include <ESP12f.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

//vector para días de la semana método obtenerHora
String weekDays[7]={"Domingo", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado"};

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

ESP12f::ESP12f()
{
    Serial.begin(115200);
};

void ESP12f::fechaHora(){ //esta funcion linda me registra el día y la hora con los minutos
    timeClient.begin();
    timeClient.setTimeOffset(-10800);
    timeClient.update();

    dia = timeClient.getDay();
    hora = timeClient.getHours();
    minutos = timeClient.getMinutes();

    String weekDay = weekDays[dia];
    String formattedTime = timeClient.getFormattedTime();
    Serial.println("Obteniendo fecha y hora ... ");
    Serial.print("Día: ");
    Serial.print(weekDay + " "); 
    Serial.println(formattedTime);
};

int ESP12f::obtenerDia(){
    return dia;
};
int ESP12f::obtenerHora(){
    return hora;
};
int ESP12f::obtenerMinutos(){
    return minutos;
};
