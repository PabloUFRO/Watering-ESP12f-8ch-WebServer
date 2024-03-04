#ifndef ESP12f_h
#define ESP12f_h
#include <Arduino.h>

class ESP12f
{
    public: 
        ESP12f();
        void fechaHora();
        int obtenerDia();
        int obtenerHora();
        int obtenerMinutos();
    private:
        int dia;
        int hora;
        int minutos;        
};
#endif