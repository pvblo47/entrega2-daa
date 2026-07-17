#include "Extraccion.h"
#include "OperacionesCono.h"
#include <limits>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <stdexcept>

// ---------------------------------------------------------------------------
// politicaToString
// ---------------------------------------------------------------------------
std::string politicaToString(TipoPolitica p) {
    switch (p) {
        case TipoPolitica::MAX_VALUE:          return "Max-Value";
        case TipoPolitica::RAZON_VALOR_TAMANO: return "Razon Valor/Tamanio";
        default:                               return "Desconocida";
    }
}

// ---------------------------------------------------------------------------
// extraccionOptimizadaDAG: Algoritmo Principal (Seccion 2.4.1)
// ---------------------------------------------------------------------------
ResultadoExtraccionDAG extraccionOptimizadaDAG(GrafoDAG& grafo, TipoPolitica politica) {

    ResultadoExtraccionDAG resultado;
    resultado.beneficioTotal  = 0.0;
    resultado.iteraciones     = 0;
    resultado.casosConosCero  = 0;

    const int total = grafo.totalNodos();
    const double NEG_INF = -1e18;

    auto tiempo_inicio = std::chrono::high_resolution_clock::now();

    // --- Preprocesamiento: arreglos indexados de metricas ---
    // valoresConos[id] = suma economica del cono con raiz en bloque id
    // tamanosConos[id] = cardinalidad del cono con raiz en bloque id
    std::vector<double> valoresConos(total, NEG_INF);
    std::vector<int>    tamanosConos(total, 1);

    // Calculo exhaustivo inicial
    static auto ultimo_print_prep = std::chrono::high_resolution_clock::now();
    for (const Bloque& b : grafo.nodos()) {
        auto ahora = std::chrono::high_resolution_clock::now();
        double transcurrido = std::chrono::duration<double, std::milli>(ahora - tiempo_inicio).count();
        if (transcurrido > 1800000.0) {
            std::cout << "\n";
            throw std::runtime_error("Timeout");
        }
        if (std::chrono::duration<double, std::milli>(ahora - ultimo_print_prep).count() >= 250.0) {
            ultimo_print_prep = ahora;
            double porcentaje = 100.0 * b.id / total;
            int ancho_barra = 30;
            int completado = static_cast<int>(porcentaje * ancho_barra / 100.0);
            std::cout << "\r[" << politicaToString(politica) << " (DAG)] Preprocesamiento: [";
            for (int i = 0; i < ancho_barra; ++i) {
                if (i < completado) std::cout << "=";
                else if (i == completado) std::cout << ">";
                else std::cout << " ";
            }
            std::cout << "] " << std::fixed << std::setprecision(1) << porcentaje 
                      << "% (Tiempo: " << transcurrido / 1000.0 << "s)" << std::flush;
        }

        if (b.estatusBorde == EstatusBorde::VALIDO) {
            std::vector<int> cono = calcularCierreCono(b.id, grafo);
            valoresConos[b.id]  = sumarValoresEconomicos(cono, grafo);
            tamanosConos[b.id]  = static_cast<int>(cono.size());
        } else {
            valoresConos[b.id]  = NEG_INF;
            tamanosConos[b.id]  = 1;
        }
    }
    std::cout << "\r[" << politicaToString(politica) << " (DAG)] Preprocesamiento completado!                                                 \n" << std::flush;

    // --- Ciclo principal de extraccion ---
    bool continuarExtraccion = true;
    static auto ultimo_print_ext = std::chrono::high_resolution_clock::now();

    while (continuarExtraccion) {
        auto ahora = std::chrono::high_resolution_clock::now();
        double transcurrido = std::chrono::duration<double, std::milli>(ahora - tiempo_inicio).count();
        if (transcurrido > 1800000.0) {
            throw std::runtime_error("Timeout");
        }
        if (std::chrono::duration<double, std::milli>(ahora - ultimo_print_ext).count() >= 250.0) {
            ultimo_print_ext = ahora;
            double porcentaje = 100.0 * resultado.bloquesExtraidos.size() / total;
            int ancho_barra = 30;
            int completado = static_cast<int>(porcentaje * ancho_barra / 100.0);
            std::cout << "\r[" << politicaToString(politica) << " (DAG)] Extracción: [";
            for (int i = 0; i < ancho_barra; ++i) {
                if (i < completado) std::cout << "=";
                else if (i == completado) std::cout << ">";
                else std::cout << " ";
            }
            std::cout << "] " << std::fixed << std::setprecision(1) << porcentaje 
                      << "% (Iter: " << resultado.iteraciones 
                      << ", Extraídos: " << resultado.bloquesExtraidos.size()
                      << ", Tiempo: " << transcurrido / 1000.0 << "s)" << std::flush;
        }

        int    bloqueObjetivo = -1;
        double metricaMaxima  = NEG_INF;

        // Ciclo de seleccion segun politica
        for (const Bloque& b : grafo.nodos()) {

            // Monitoreo de conos con valor cero (reporte obligatorio)
            if (b.activo && std::abs(valoresConos[b.id]) <= EPSILON_VALOR && !b.reportadoCero) {
                grafo.obtenerNodo(b.id).reportadoCero = true;
                resultado.casosConosCero++;
            }

            // Filtro: solo bloques activos con cono rentable
            if (!b.activo || valoresConos[b.id] <= EPSILON_VALOR) {
                continue;
            }

            double metricaActual = 0.0;

            if (politica == TipoPolitica::MAX_VALUE) {
                metricaActual = valoresConos[b.id];
            } else { // RAZON_VALOR_TAMANO
                metricaActual = valoresConos[b.id] /
                                static_cast<double>(tamanosConos[b.id]);
            }

            if (metricaActual > metricaMaxima) {
                metricaMaxima  = metricaActual;
                bloqueObjetivo = b.id;
            }
        }

        // Condicion y criterio de termino
        if (bloqueObjetivo == -1) {
            continuarExtraccion = false;
        } else {
            // Reconstruccion acotada del cono optimo identificado
            std::vector<int> conoAExtraer = calcularCierreCono(bloqueObjetivo, grafo);

            // Acumular ganancia
            resultado.beneficioTotal += valoresConos[bloqueObjetivo];

            // Agregar bloques al registro historico
            for (int id : conoAExtraer) {
                resultado.bloquesExtraidos.push_back(id);
            }

            // Desactivacion fisica de nodos extraidos
            for (int id : conoAExtraer) {
                grafo.obtenerNodo(id).activo = false;
                valoresConos[id]             = NEG_INF;
            }

            // Actualizacion dinamica exclusiva de descendientes afectados
            actualizarDescendientesDinamico(
                conoAExtraer, grafo, valoresConos, tamanosConos, tiempo_inicio
            );

            resultado.iteraciones++;
        }
    }

    std::cout << "\r[" << politicaToString(politica) << " (DAG)] Completado! (Tiempo: " 
              << std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - tiempo_inicio).count() / 1000.0 
              << "s)                                                                       \n" << std::flush;

    return resultado;
}
