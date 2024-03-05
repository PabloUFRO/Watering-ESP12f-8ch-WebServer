#ifndef valvula_h
#define valvula_h
#include <Arduino.h>

class valvula
{
    public: 
        valvula(int pin);
        void compruebaRiego(int* fechaHora);
        void activar();
        void desactivar(); 
        void asignaParametros(int i, int tiempoRiego, int horaRiego, int minutoRiego);

        int _numValvula = 0;    
        int _tiempoRiego;   //almacena tiempo de riego
        int _horaRiego;     //almacena horario de riego
        int _minutoRiego; //almacena horario de riego
        int _diaRiego; //almacena dia cuando se regó
        int _diaActual; //entrada metodo compruebaRiego
        int _horaActual;
        int _minutoActual;

        bool _state; 

    private:
        int _pin;
              
};
#endif