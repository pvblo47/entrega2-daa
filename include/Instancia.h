#pragma once

#include <vector>
#include <string>

// Estructura para contener los datos de cada bloque leido del CSV
struct BloqueDatos {
    double tonelada = 0.0;
    double metal1 = 0.0;
    double metal2 = 0.0;
    int roca = 0;
    double valorBloque = 0.0;

    void calcularValor() {
        valorBloque = (metal1 * 0.89 * 13228.0) - (tonelada * 13.0) - (tonelada * 25.0);
    }
};

// Lee la instancia desde un archivo CSV con formato X,Y,Z,Tonelada,Metal_1,Metal_2,Roca
std::vector<std::vector<std::vector<BloqueDatos>>> leerInstanciaCSV(
    const std::string& ruta,
    int& outX, int& outY, int& outZ
);

// Genera la matriz 3D del ejemplo de iteracion del documento (Seccion 2.5.1):
// X=5, Y=5, Z=3. Bloque valioso en (2,2,0) con v=+150, etc.
std::vector<std::vector<std::vector<BloqueDatos>>> generarInstanciaEjemplo();

// Genera una instancia sintetica aleatoria de dimensiones X*Y*Z.
// probPositivo: fraccion de bloques con valor positivo [0,1]
// semilla: para reproducibilidad
std::vector<std::vector<std::vector<BloqueDatos>>> generarInstanciaAleatoria(
    int X, int Y, int Z,
    double probPositivo = 0.3,
    unsigned int semilla = 42
);

// Instancia determinista identica a la linea base (crearInstanciaEscalable)
std::vector<std::vector<std::vector<BloqueDatos>>> generarInstanciaEscalable(int X, int Y, int Z);

// Imprime en stdout el resumen de un resultado de extraccion
void imprimirResultado(
    const std::string& etiqueta,
    int X, int Y, int Z,
    double beneficio,
    int bloquesExtraidos,
    int iteraciones,
    int casosZero,
    double tiempoMs,
    long ramKB
);

// Retorna el uso actual de RAM del proceso en KB (Linux /proc/self/status)
long obtenerRAM_KB();
