#pragma once

#define MAX_RUTAS 100
#define MAX_LONGITUD_RUTA 512

// Lista global de rutas (extern para usarla en otros .cpp)
extern char rutasPDF[MAX_RUTAS][MAX_LONGITUD_RUTA];
extern int cantidadRutas;

// Funciones para manipular la lista
void agregarRutaPDF(const char* ruta);
const char* obtenerRutaPDF(int index);
void limpiarRutasPDF();
int obtenerCantidadRutasPDF();
bool rutaExisteEnLista(const char* ruta);



