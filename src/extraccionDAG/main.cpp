#include <iostream>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>

// DAG propio
#include "GrafoDAG.h"
#include "Extraccion.h"
#include "Instancia.h"

// Linea base original
#include "../linea_base/ExtractorConos.hpp"
#include "../linea_base/ModeloBloques.hpp"

// Convierte matriz 3D a ModeloBloques usando setValor()
static ModeloBloques crearModelo(int X, int Y, int Z,
    const std::vector<std::vector<std::vector<BloqueDatos>>>& mat)
{
    ModeloBloques m(X, Y, Z);
    for (int x = 0; x < X; ++x)
        for (int y = 0; y < Y; ++y)
            for (int z = 0; z < Z; ++z)
                m.setValor(x, y, z, mat[x][y][z].valorBloque);
    return m;
}

static void separador(std::ostream& out) {
    out << "----------------------------------------------------------------------------------------------------\n";
}

static void ejecutarEscala(
    const std::string& nombre, int X, int Y, int Z,
    const std::vector<std::vector<std::vector<BloqueDatos>>>& mat,
    std::ofstream& archivo)
{
    std::vector<std::ostream*> salidas = {&std::cout};
    if (archivo.is_open()) salidas.push_back(&archivo);

    for (auto* out : salidas) {
        *out << "\n====================================================================================================\n";
        *out << "  " << nombre << " (" << X << "x" << Y << "x" << Z
             << " = " << X*Y*Z << " bloques)\n";
        *out << "====================================================================================================\n";
        *out << std::left  << std::setw(32) << "  Algoritmo"
             << std::right << std::setw(14) << "Beneficio"
             << std::setw(14) << "Bloques Ext."
             << std::setw(12) << "Tiempo(ms)"
             << std::setw(14) << "Iteraciones"
             << std::setw(14) << "RAM (KB)" << "\n";
        separador(*out);
    }

    // ── 1. First-Fit (Linea Base) ──────────────────────────────────────────
    double msLB = 0;
    ResultadoExtraccion resLB;
    long ramLB = -1;
    bool timeoutLB = false;
    try {
        auto modeloLB = crearModelo(X, Y, Z, mat);
        ExtractorConos extractor;
        auto t0 = std::chrono::high_resolution_clock::now();
        resLB = extractor.extraccionLineaBase(modeloLB);
        auto t1 = std::chrono::high_resolution_clock::now();
        msLB = std::chrono::duration<double, std::milli>(t1 - t0).count();
        ramLB = obtenerRAM_KB();
    } catch (const std::runtime_error& e) {
        if (std::string(e.what()) == "Timeout") timeoutLB = true;
        else throw;
    }

    // ── 2. DAG Max-Value ───────────────────────────────────────────────────
    double msMV = 0;
    ResultadoExtraccionDAG resMV;
    long ramMV = -1;
    bool timeoutMV = false;
    try {
        GrafoDAG grafoMV(X, Y, Z, mat);
        auto t0 = std::chrono::high_resolution_clock::now();
        resMV = extraccionOptimizadaDAG(grafoMV, TipoPolitica::MAX_VALUE);
        auto t1 = std::chrono::high_resolution_clock::now();
        msMV = std::chrono::duration<double, std::milli>(t1 - t0).count();
        ramMV = obtenerRAM_KB();
    } catch (const std::runtime_error& e) {
        if (std::string(e.what()) == "Timeout") timeoutMV = true;
        else throw;
    }

    // ── 3. DAG Razon ───────────────────────────────────────────────────────
    double msRZ = 0;
    ResultadoExtraccionDAG resRZ;
    long ramRZ = -1;
    bool timeoutRZ = false;
    try {
        GrafoDAG grafoRZ(X, Y, Z, mat);
        auto t0 = std::chrono::high_resolution_clock::now();
        resRZ = extraccionOptimizadaDAG(grafoRZ, TipoPolitica::RAZON_VALOR_TAMANO);
        auto t1 = std::chrono::high_resolution_clock::now();
        msRZ = std::chrono::duration<double, std::milli>(t1 - t0).count();
        ramRZ = obtenerRAM_KB();
    } catch (const std::runtime_error& e) {
        if (std::string(e.what()) == "Timeout") timeoutRZ = true;
        else throw;
    }

    // ── Imprimir resultados ────────────────────────────────────────────────
    for (auto* out : salidas) {
        *out << std::fixed << std::setprecision(2);

        if (timeoutLB) {
            *out << std::left << std::setw(32) << "  First-Fit (Linea Base)"
                 << std::right << std::setw(14) << "TIMEOUT"
                 << std::setw(14) << "TIMEOUT"
                 << std::setw(12) << ">1800000"
                 << std::setw(14) << "—"
                 << std::setw(14) << "N/D" << "\n";
        } else {
            *out << std::left << std::setw(32) << "  First-Fit (Linea Base)"
                 << std::right << std::setw(14) << resLB.beneficio_total
                 << std::setw(14) << resLB.bloques_extraidos.size()
                 << std::setw(12) << msLB
                 << std::setw(14) << "—"
                 << std::setw(14) << (ramLB >= 0 ? std::to_string(ramLB) : "N/D") << "\n";
        }

        if (timeoutMV) {
            *out << std::left << std::setw(32) << "  Max-Value (DAG)"
                 << std::right << std::setw(14) << "TIMEOUT"
                 << std::setw(14) << "TIMEOUT"
                 << std::setw(12) << ">1800000"
                 << std::setw(14) << "TIMEOUT"
                 << std::setw(14) << "N/D" << "\n";
        } else {
            *out << std::left << std::setw(32) << "  Max-Value (DAG)"
                 << std::right << std::setw(14) << resMV.beneficioTotal
                 << std::setw(14) << (int)resMV.bloquesExtraidos.size()
                 << std::setw(12) << msMV
                 << std::setw(14) << resMV.iteraciones
                 << std::setw(14) << (ramMV >= 0 ? std::to_string(ramMV) : "N/D") << "\n";
        }

        if (timeoutRZ) {
            *out << std::left << std::setw(32) << "  Razon Valor/Tamano (DAG)"
                 << std::right << std::setw(14) << "TIMEOUT"
                 << std::setw(14) << "TIMEOUT"
                 << std::setw(12) << ">1800000"
                 << std::setw(14) << "TIMEOUT"
                 << std::setw(14) << "N/D" << "\n";
        } else {
            *out << std::left << std::setw(32) << "  Razon Valor/Tamano (DAG)"
                 << std::right << std::setw(14) << resRZ.beneficioTotal
                 << std::setw(14) << (int)resRZ.bloquesExtraidos.size()
                 << std::setw(12) << msRZ
                 << std::setw(14) << resRZ.iteraciones
                 << std::setw(14) << (ramRZ >= 0 ? std::to_string(ramRZ) : "N/D") << "\n";
        }

        separador(*out);
    }
}

int main() {
    std::ofstream archivo("resultados_pruebas.txt");

    std::vector<std::ostream*> salidas = {&std::cout};
    if (archivo.is_open()) salidas.push_back(&archivo);

    for (auto* out : salidas) {
        *out << "====================================================================================================\n";
        *out << "  SUITE DE PRUEBAS - Grupo 01: Bezares / Briones / Bravo\n";
        *out << "====================================================================================================\n";
    }

    // ── Instancias sintéticas escalables ──────────────────────────────────
    for (auto* out : salidas)
        *out << "\n  [1/4] Instancias sinteticas (generarInstanciaEscalable)\n";

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

    // ── Escenario real ─────────────────────────────────────────────────────
    for (auto* out : salidas)
        *out << "\n  [2/4] Escenario real (escenario00.txt)\n";

    int X = 0, Y = 0, Z = 0;
    auto mat = leerInstanciaCSV("escenario00.txt", X, Y, Z);
    if (!mat.empty()) {
        ejecutarEscala("Escenario Real 00", X, Y, Z, mat, archivo);
    } else {
        std::cerr << "Error: no se pudo cargar escenario00.txt\n";
    }

    for (auto* out : salidas) {
        *out << "\n  Resultados exportados a: resultados_pruebas.txt\n";
        *out << "====================================================================================================\n";
    }

    if (archivo.is_open()) archivo.close();
    return 0;
}
