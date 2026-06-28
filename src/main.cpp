#include <iostream>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>

// DAG propio (tu implementacion)
#include "GrafoDAG.h"
#include "Extraccion.h"
#include "Instancia.h"

// Linea base del compañero (solo necesitamos ModeloBloques + Extraccion_Linea_Base)
#include "linea_base.h"

// Convierte matriz 3D al ModeloBloques de la linea base
static ModeloBloques crearModelo(int X, int Y, int Z,
    const std::vector<std::vector<std::vector<double>>>& mat)
{
    ModeloBloques m(X, Y, Z);
    m.valor = mat;
    return m;
}

static void separador(std::ostream& out) {
    out << "--------------------------------------------------------------------------------\n";
}

static void ejecutarEscala(
    const std::string& nombre, int X, int Y, int Z,
    const std::vector<std::vector<std::vector<double>>>& mat,
    std::ofstream& archivo)
{
    std::vector<std::ostream*> salidas = {&std::cout};
    if (archivo.is_open()) salidas.push_back(&archivo);

    for (auto* out : salidas) {
        *out << "\n================================================================================\n";
        *out << "  " << nombre << " (" << X << "x" << Y << "x" << Z
             << " = " << X*Y*Z << " bloques)\n";
        *out << "================================================================================\n";
        *out << std::left  << std::setw(32) << "  Algoritmo"
             << std::right << std::setw(14) << "Beneficio"
             << std::setw(14) << "Bloques Ext."
             << std::setw(12) << "Tiempo(ms)"
             << std::setw(14) << "Iteraciones" << "\n";
        separador(*out);
    }

    // ── 1. First-Fit (Linea Base) ──────────────────────────────────────────
    auto modeloLB = crearModelo(X, Y, Z, mat);
    auto t0 = std::chrono::high_resolution_clock::now();
    Metricas mLB = Extraccion_Linea_Base(modeloLB);
    auto t1 = std::chrono::high_resolution_clock::now();
    double msLB = std::chrono::duration<double, std::milli>(t1 - t0).count();

    // ── 2. DAG Max-Value ───────────────────────────────────────────────────
    GrafoDAG grafoMV(X, Y, Z, mat);
    t0 = std::chrono::high_resolution_clock::now();
    ResultadoExtraccion resMV = extraccionOptimizadaDAG(grafoMV, TipoPolitica::MAX_VALUE);
    t1 = std::chrono::high_resolution_clock::now();
    double msMV = std::chrono::duration<double, std::milli>(t1 - t0).count();

    // ── 3. DAG Razon ───────────────────────────────────────────────────────
    GrafoDAG grafoRZ(X, Y, Z, mat);
    t0 = std::chrono::high_resolution_clock::now();
    ResultadoExtraccion resRZ = extraccionOptimizadaDAG(grafoRZ, TipoPolitica::RAZON_VALOR_TAMANO);
    t1 = std::chrono::high_resolution_clock::now();
    double msRZ = std::chrono::duration<double, std::milli>(t1 - t0).count();

    // ── Imprimir resultados ────────────────────────────────────────────────
    for (auto* out : salidas) {
        *out << std::fixed << std::setprecision(2);

        *out << std::left  << std::setw(32) << "  First-Fit (Linea Base)"
             << std::right << std::setw(14) << mLB.beneficioTotal
             << std::setw(14) << mLB.bloquesExtraidos
             << std::setw(12) << msLB
             << std::setw(14) << mLB.conosExtraidos << "\n";

        *out << std::left  << std::setw(32) << "  Max-Value (DAG)"
             << std::right << std::setw(14) << resMV.beneficioTotal
             << std::setw(14) << (int)resMV.bloquesExtraidos.size()
             << std::setw(12) << msMV
             << std::setw(14) << resMV.iteraciones << "\n";

        *out << std::left  << std::setw(32) << "  Razon Valor/Tamano (DAG)"
             << std::right << std::setw(14) << resRZ.beneficioTotal
             << std::setw(14) << (int)resRZ.bloquesExtraidos.size()
             << std::setw(12) << msRZ
             << std::setw(14) << resRZ.iteraciones << "\n";

        separador(*out);
    }
}

int main() {
    std::ofstream archivo("resultados_pruebas.txt");

    std::vector<std::ostream*> salidas = {&std::cout};
    if (archivo.is_open()) salidas.push_back(&archivo);

    for (auto* out : salidas) {
        *out << "================================================================================\n";
        *out << "  SUITE DE PRUEBAS - Grupo 01: Bezares / Briones / Bravo\n";
        *out << "  Instancia: generarInstanciaEscalable (determinista)\n";
        *out << "================================================================================\n";
    }

    struct Escala { int X, Y, Z; const char* nombre; };
    Escala escalas[] = {
        {10, 10, 3, "Escala Pequena"},
        {30, 30, 3, "Escala Mediana"},
        {50, 50, 3, "Escala Grande" },
    };

    for (auto& e : escalas) {
        auto mat = generarInstanciaEscalable(e.X, e.Y, e.Z);
        ejecutarEscala(e.nombre, e.X, e.Y, e.Z, mat, archivo);
    }

    for (auto* out : salidas) {
        *out << "\n  Resultados exportados a: resultados_pruebas.txt\n";
        *out << "================================================================================\n";
    }

    if (archivo.is_open()) archivo.close();
    return 0;
}
