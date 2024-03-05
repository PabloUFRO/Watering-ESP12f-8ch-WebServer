#include <Arduino.h>
#include <boton.h>

boton::boton(){
    desactivar();
};

void boton::activar(){
    _estado = true;
};

void boton::desactivar(){
    _estado = false;
};