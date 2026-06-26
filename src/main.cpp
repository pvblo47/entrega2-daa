#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>
#include <string>
#include <fstream>

#include "linea_base.h"
#include "dag_extraccion.h"
#include "Instancia.h"

// Convierte una matriz 3D cruda a la estructura de la línea base
static ModeloBloques crearModelo(int X, int Y, int Z, const std::vector<std::vector<std::vector<double>>>& mat) {
    ModeloBloques modelo(X, Y, Z);
    modelo.valor = mat;
    return modelo;
}

// Ejecuta la suite de pruebas para una escala específica
static void ejecutarEscala(const std::string& nombreEscala, int X, int Y, int Z, unsigned int semilla, std::ofstream& archivo) {
    // Generar la matriz 3D con ley mineral variable y sin padding
    auto matrizOrigen = generarInstanciaLeyVariable(X, Y, Z, semilla);

    // --- 1. Algoritmo Línea Base: First-Fit ---
    long ramAntesLB = obtenerRAM_KB();
    auto modeloLB = crearModelo(X, Y, Z, matrizOrigen);
    
    auto t0_lb = std::chrono::high_resolution_clock::now();
    Metricas metricasLB = Extraccion_Linea_Base(modeloLB);
    auto t1_lb = std::chrono::high_resolution_clock::now();
    
    double tiempoTotalLBMs = std::chrono::duration<double, std::milli>(t1_lb - t0_lb).count();
    long ramDespuesLB = obtenerRAM_KB();

    // --- 2. Algoritmo DAG: Max-Value ---
    long ramAntesDAGMax = obtenerRAM_KB();
    auto modeloDAGMax = crearModelo(X, Y, Z, matrizOrigen);
    
    auto t0_dagmax = std::chrono::high_resolution_clock::now();
    MetricasDAG metricasDAGMax = Extraccion_DAG(modeloDAGMax, "Max-Value");
    auto t1_dagmax = std::chrono::high_resolution_clock::now();
    
    double tiempoTotalDAGMaxMs = std::chrono::duration<double, std::milli>(t1_dagmax - t0_dagmax).count();
    long ramDespuesDAGMax = obtenerRAM_KB();

    // --- 3. Algoritmo DAG: Razón Valor/Tamaño ---
    long ramAntesDAGRazon = obtenerRAM_KB();
    auto modeloDAGRazon = crearModelo(X, Y, Z, matrizOrigen);
    
    auto t0_dagrazon = std::chrono::high_resolution_clock::now();
    MetricasDAG metricasDAGRazon = Extraccion_DAG(modeloDAGRazon, "Razon");
    auto t1_dagrazon = std::chrono::high_resolution_clock::now();
    
    double tiempoTotalDAGRazonMs = std::chrono::duration<double, std::milli>(t1_dagrazon - t0_dagrazon).count();
    long ramDespuesDAGRazon = obtenerRAM_KB();

    std::string ramLBStr = "N/D";
    if (ramAntesLB >= 0 && ramDespuesLB >= 0) {
        ramLBStr = std::to_string(ramAntesLB) + " -> " + std::to_string(ramDespuesLB) + " KB";
    }

    std::string ramDAGMaxStr = "N/D";
    if (ramAntesDAGMax >= 0 && ramDespuesDAGMax >= 0) {
        ramDAGMaxStr = std::to_string(ramAntesDAGMax) + " -> " + std::to_string(ramDespuesDAGMax) + " KB";
    }

    std::string ramDAGRazonStr = "N/D";
    if (ramAntesDAGRazon >= 0 && ramDespuesDAGRazon >= 0) {
        ramDAGRazonStr = std::to_string(ramAntesDAGRazon) + " -> " + std::to_string(ramDespuesDAGRazon) + " KB";
    }

    // Configurar los flujos de salida (Consola y Archivo)
    std::vector<std::ostream*> salidas = { &std::cout };
    if (archivo.is_open()) {
        salidas.push_back(&archivo);
    }

    for (auto outPtr : salidas) {
        std::ostream& out = *outPtr;
        out << "\n================================================================================\n";
        out << " INICIANDO PRUEBAS: " << nombreEscala << " (" << X << "x" << Y << "x" << Z << " = " << X*Y*Z << " bloques)\n";
        out << "================================================================================\n";

        // Imprimir Tabla Comparativa de Resultados de la Escala
        out << "\n----------------------------------------------------------------------------------------------------------------------\n";
        out << "  RESULTADOS COMPARATIVOS: " << nombreEscala << "\n";
        out << "----------------------------------------------------------------------------------------------------------------------\n";
        out << "  " << std::left << std::setw(28) << "Algoritmo"
            << std::right << std::setw(15) << "Beneficio"
            << std::setw(15) << "Bloques Ext."
            << std::setw(15) << "T. Total"
            << std::setw(24) << "Peak RAM (Antes->Desp)"
            << std::setw(15) << "Iteraciones" << "\n";
        out << "----------------------------------------------------------------------------------------------------------------------\n";
        
        // First-Fit
        out << "  " << std::left << std::setw(28) << "First-Fit (L. Base)"
            << std::right << std::fixed << std::setprecision(2)
            << std::setw(15) << metricasLB.beneficioTotal
            << std::setw(15) << metricasLB.bloquesExtraidos
            << std::setw(13) << tiempoTotalLBMs << " ms"
            << std::setw(24) << ramLBStr
            << std::setw(15) << metricasLB.conosExtraidos << "\n";

        // Max-Value
        out << "  " << std::left << std::setw(28) << "Max-Value (DAG)"
            << std::right << std::fixed << std::setprecision(2)
            << std::setw(15) << metricasDAGMax.beneficioTotal
            << std::setw(15) << metricasDAGMax.bloquesExtraidos
            << std::setw(13) << tiempoTotalDAGMaxMs << " ms"
            << std::setw(24) << ramDAGMaxStr
            << std::setw(15) << metricasDAGMax.conosExtraidos << "\n";

        // Razón
        out << "  " << std::left << std::setw(28) << "Razon (DAG)"
            << std::right << std::fixed << std::setprecision(2)
            << std::setw(15) << metricasDAGRazon.beneficioTotal
            << std::setw(15) << metricasDAGRazon.bloquesExtraidos
            << std::setw(13) << tiempoTotalDAGRazonMs << " ms"
            << std::setw(24) << ramDAGRazonStr
            << std::setw(15) << metricasDAGRazon.conosExtraidos << "\n";
                  
        out << "----------------------------------------------------------------------------------------------------------------------\n";

        // Imprimir marcas de tiempo internas de la propuesta optimizada
        out << "  DETALLE INTERNO SUBRUTINAS DAG:\n";
        out << "    * Max-Value:\n";
        out << "      - Construir_DAG_Precedencias   : " << std::fixed << std::setprecision(4) << metricasDAGMax.tiempoConstruirDAGMs << " ms\n";
        out << "      - Actualizar_Descendientes_Dinam: " << std::fixed << std::setprecision(4) << metricasDAGMax.tiempoActualizarDescMs << " ms\n";
        out << "      - Conos evaluados con valor 0  : " << metricasDAGMax.conosCeroReportados << "\n";
        out << "    * Razon:\n";
        out << "      - Construir_DAG_Precedencias   : " << std::fixed << std::setprecision(4) << metricasDAGRazon.tiempoConstruirDAGMs << " ms\n";
        out << "      - Actualizar_Descendientes_Dinam: " << std::fixed << std::setprecision(4) << metricasDAGRazon.tiempoActualizarDescMs << " ms\n";
        out << "      - Conos evaluados con valor 0  : " << metricasDAGRazon.conosCeroReportados << "\n";
        out << "================================================================================\n\n";
    }
}

int main() {
    std::ofstream archivo("resultados_pruebas.txt");
    
    std::vector<std::ostream*> salidas = { &std::cout };
    if (archivo.is_open()) {
        salidas.push_back(&archivo);
    }

    for (auto outPtr : salidas) {
        std::ostream& out = *outPtr;
        out << "================================================================================\n";
        out << "       SUITE DE PRUEBAS Y PERFILAMIENTO DE ALGORITMOS DE EXTRACCION\n";
        out << "       Yacimiento 3D Open Pit - C++17 Standard Library\n";
        out << "================================================================================\n";
    }

    // Semilla para reproducibilidad de las pruebas
    const unsigned int semilla = 42;

    // 1. Escala Pequeña (10x10x3)
    ejecutarEscala("Escala Pequena", 10, 10, 3, semilla, archivo);

    // 2. Escala Mediana (30x30x3)
    ejecutarEscala("Escala Mediana", 30, 30, 3, semilla, archivo);

    // 3. Escala Grande (50x50x3)
    ejecutarEscala("Escala Grande", 50, 50, 3, semilla, archivo);

    for (auto outPtr : salidas) {
        std::ostream& out = *outPtr;
        out << "================================================================================\n";
        out << "  Pruebas completadas exitosamente.\n";
        if (outPtr == &std::cout && archivo.is_open()) {
            out << "  Resultados exportados a: resultados_pruebas.txt\n";
        }
        out << "================================================================================\n";
    }

    if (archivo.is_open()) {
        archivo.close();
    }

    return 0;
}
