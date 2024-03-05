#include <Arduino.h>
#include <boton.h>

boton::boton(){
    desactivar();
};

void boton::activar(){
    _state = true;
};

void boton::desactivar(){
    _state = false;
};