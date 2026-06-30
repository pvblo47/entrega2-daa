#pragma once

#include "GrafoDAG.h"
#include <vector>
#include <string>

// Politicas de seleccion disponibles (Seccion 2.3)
enum class TipoPolitica {
    MAX_VALUE,      // Maximo valor economico total del cono
    RAZON_VALOR_TAMANO  // Razon valor / tamaño del cono
};

// Resultado devuelto por el algoritmo principal
struct ResultadoExtraccionDAG {
    double beneficioTotal;
    std::vector<int> bloquesExtraidos;   // ids de todos los bloques removidos
    int iteraciones;                     // cantidad de conos extraidos
    int casosConosCero;                  // conos con valor exactamente 0 detectados
};

// Algoritmo principal: Extraccion_Optimizada_DAG (Seccion 2.4.1)
ResultadoExtraccionDAG extraccionOptimizadaDAG(GrafoDAG& grafo, TipoPolitica politica);

// Convierte el enum a string legible
std::string politicaToString(TipoPolitica p);
