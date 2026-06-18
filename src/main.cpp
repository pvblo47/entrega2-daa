#include <iostream>
#include <chrono>
#include <vector>

#include "GrafoDAG.h"
#include "Extraccion.h"
#include "Instancia.h"

// ---------------------------------------------------------------------------
// Ejecuta ambas politicas sobre una instancia dada y reporta resultados
// ---------------------------------------------------------------------------
static void ejecutarInstancia(
    const std::string& nombreInstancia,
    int X, int Y, int Z,
    const std::vector<std::vector<std::vector<double>>>& mat)
{
    // --- Max-Value ---
    {
        GrafoDAG grafo(X, Y, Z, mat);

        auto inicio = std::chrono::high_resolution_clock::now();

        ResultadoExtraccion res = extraccionOptimizadaDAG(grafo, TipoPolitica::MAX_VALUE);

        auto fin = std::chrono::high_resolution_clock::now();
        double ms = std::chrono::duration<double, std::milli>(fin - inicio).count();
        long ram = obtenerRAM_KB();

        imprimirResultado(
            nombreInstancia + " | " + politicaToString(TipoPolitica::MAX_VALUE),
            X, Y, Z,
            res.beneficioTotal,
            static_cast<int>(res.bloquesExtraidos.size()),
            res.iteraciones,
            res.casosConosCero,
            ms,
            ram
        );
    }

    // --- Razon Valor/Tamaño ---
    {
        GrafoDAG grafo(X, Y, Z, mat);

        auto inicio = std::chrono::high_resolution_clock::now();

        ResultadoExtraccion res = extraccionOptimizadaDAG(grafo, TipoPolitica::RAZON_VALOR_TAMANO);

        auto fin = std::chrono::high_resolution_clock::now();
        double ms = std::chrono::duration<double, std::milli>(fin - inicio).count();
        long ramDespues = obtenerRAM_KB();

        imprimirResultado(
            nombreInstancia + " | " + politicaToString(TipoPolitica::RAZON_VALOR_TAMANO),
            X, Y, Z,
            res.beneficioTotal,
            static_cast<int>(res.bloquesExtraidos.size()),
            res.iteraciones,
            res.casosConosCero,
            ms,
            ramDespues
        );
    }
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main() {
    std::cout << "============================================================\n";
    std::cout << "  Extraccion Optimizada DAG - Entrega 2                     \n";
    std::cout << "  Grupo 01: Bezares / Briones / Bravo                       \n";
    std::cout << "============================================================\n";

    // 1. Instancia ejemplo del documento (5x5x3)
    {
        auto mat = generarInstanciaEjemplo();
        ejecutarInstancia("Ejemplo Doc (5x5x3)", 5, 5, 3, mat);
    }

    // 2. Escala pequeña (10x10x3)
    {
        auto mat = generarInstanciaAleatoria(10, 10, 3, 0.35, 42);
        ejecutarInstancia("Escala Pequena (10x10x3)", 10, 10, 3, mat);
    }

    // 3. Escala mediana (30x30x3)
    {
        auto mat = generarInstanciaAleatoria(30, 30, 3, 0.35, 42);
        ejecutarInstancia("Escala Mediana (30x30x3)", 30, 30, 3, mat);
    }

    // 4. Escala grande (50x50x3)
    {
        auto mat = generarInstanciaAleatoria(50, 50, 3, 0.35, 42);
        ejecutarInstancia("Escala Grande (50x50x3)", 50, 50, 3, mat);
    }

    // 5. Escala profunda (20x20x10)
    {
        auto mat = generarInstanciaAleatoria(20, 20, 10, 0.25, 99);
        ejecutarInstancia("Escala Profunda (20x20x10)", 20, 20, 10, mat);
    }

    std::cout << "\n============================================================\n";
    std::cout << "  Ejecucion completada.\n";
    std::cout << "============================================================\n";

    return 0;
}
