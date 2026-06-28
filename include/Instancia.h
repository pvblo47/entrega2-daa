#pragma once

#include <vector>
#include <string>

// Genera la matriz 3D del ejemplo de iteracion del documento (Seccion 2.5.1):
// X=5, Y=5, Z=3. Bloque valioso en (2,2,0) con v=+150, etc.
std::vector<std::vector<std::vector<double>>> generarInstanciaEjemplo();

// Genera una instancia sintetica aleatoria de dimensiones X*Y*Z.
// probPositivo: fraccion de bloques con valor positivo [0,1]
// semilla: para reproducibilidad
std::vector<std::vector<std::vector<double>>> generarInstanciaAleatoria(
    int X, int Y, int Z,
    double probPositivo = 0.3,
    unsigned int semilla = 42
);

// Instancia determinista identica a la linea base (crearInstanciaEscalable)
std::vector<std::vector<std::vector<double>>> generarInstanciaEscalable(int X, int Y, int Z);

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
