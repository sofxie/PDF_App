#include "listasPDF.h"
#include <cstring> // para strcpy

char rutasPDF[MAX_RUTAS][MAX_LONGITUD_RUTA];
int cantidadRutas = 0;

void agregarRutaPDF(const char* ruta) {
    if (cantidadRutas < MAX_RUTAS) {
        strncpy(rutasPDF[cantidadRutas], ruta, MAX_LONGITUD_RUTA - 1);
        rutasPDF[cantidadRutas][MAX_LONGITUD_RUTA - 1] = '\0'; // Null-terminate
        cantidadRutas++;
    }
}

const char* obtenerRutaPDF(int index) {
    if (index >= 0 && index < cantidadRutas) {
        return rutasPDF[index];
    }
    return nullptr;
}

void limpiarRutasPDF() {
    cantidadRutas = 0;
}

int obtenerCantidadRutasPDF() {
    return cantidadRutas;
}

bool rutaExisteEnLista(const char* ruta) {
    for (int i = 0; i < cantidadRutas; i++) {
        if (strcmp(rutasPDF[i], ruta) == 0) {
            return true;
        }
    }
    return false;
}


