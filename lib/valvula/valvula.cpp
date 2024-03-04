#include <Arduino.h>
#include <valvula.h>

valvula::valvula(int pin){
    _pin = pin;
    pinMode(_pin, OUTPUT);
    desactivar();

};

void valvula::compruebaRiego(int* fechaHora){
    
    Serial.print("Comprobando riego bancal N° "); Serial.println(_numValvula);

    _diaActual = fechaHora[0];
    _horaActual = fechaHora[1]; 
    _minutoActual = fechaHora[2];

    if (_diaActual != _diaRiego){   //Si los dias no coinciden, significa que no se ha regado
        
        if ((_horaActual = _horaRiego) and (_minutoActual >= _minutoRiego) and (!_activa)){ //Si es hora de riego y está apagada:
            Serial.print("Iniciando riego bancal N° ");
            Serial.println(_numValvula);
            activar();
        }

        if ((_horaActual = _horaRiego) and (_minutoActual >= _minutoRiego) and (_activa)){ //Aqui se está regando, se debe corroborar el tiempo
            if (_minutoActual - _minutoRiego >= _tiempoRiego){//ya se cumplió el tiempo, se desactiva y actualiza el dia
                Serial.println("Periodo de riego finalizado. Desactvando ...");
                desactivar();
                _diaRiego = _diaActual;
            }
            else{
               Serial.println("En periodo de riego ..."); 
            }    
        }
        
        else{ //aun no es hora del riego
            Serial.println("Aun no es hora del riego"); 
        }
    }
    else{ //Si los días coinciden, significa que ya se regó
        Serial.println("Ya se regó hoy");
    }

};

void valvula::asignaParametros(int i, int tiempoRiego, int horaRiego, int minutoRiego){
    _numValvula = i+1;
    _tiempoRiego = tiempoRiego;
    _horaRiego = horaRiego;
    _minutoRiego = minutoRiego;
};

void valvula::activar(){
    digitalWrite(_pin,HIGH);
    _activa = true;
};

void valvula::desactivar(){
    digitalWrite(_pin,LOW);
    _activa = false;
};