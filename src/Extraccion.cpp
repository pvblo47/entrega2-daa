#include "Extraccion.h"
#include "OperacionesCono.h"
#include <limits>
#include <iostream>

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
ResultadoExtraccion extraccionOptimizadaDAG(GrafoDAG& grafo, TipoPolitica politica) {

    ResultadoExtraccion resultado;
    resultado.beneficioTotal  = 0.0;
    resultado.iteraciones     = 0;
    resultado.casosConosCero  = 0;

    const int total = grafo.totalNodos();
    const double NEG_INF = -1e18;

    // --- Preprocesamiento: arreglos indexados de metricas ---
    // valoresConos[id] = suma economica del cono con raiz en bloque id
    // tamanosConos[id] = cardinalidad del cono con raiz en bloque id
    std::vector<double> valoresConos(total, NEG_INF);
    std::vector<int>    tamanosConos(total, 1);

    // Calculo exhaustivo inicial
    for (const Bloque& b : grafo.nodos()) {
        if (b.estatusBorde == EstatusBorde::VALIDO) {
            std::vector<int> cono = calcularCierreCono(b.id, grafo);
            valoresConos[b.id]  = sumarValoresEconomicos(cono, grafo);
            tamanosConos[b.id]  = static_cast<int>(cono.size());
        } else {
            valoresConos[b.id]  = NEG_INF;
            tamanosConos[b.id]  = 1;
        }
    }

    // --- Ciclo principal de extraccion ---
    bool continuarExtraccion = true;

    while (continuarExtraccion) {
        int    bloqueObjetivo = -1;
        double metricaMaxima  = NEG_INF;

        // Ciclo de seleccion segun politica
        for (const Bloque& b : grafo.nodos()) {

            // Monitoreo de conos con valor cero (reporte obligatorio)
            if (b.activo && valoresConos[b.id] == 0.0 && !b.reportadoCero) {
                grafo.obtenerNodo(b.id).reportadoCero = true;
                resultado.casosConosCero++;
            }

            // Filtro: solo bloques activos con cono rentable
            if (!b.activo || valoresConos[b.id] <= 0.0) {
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
                conoAExtraer, grafo, valoresConos, tamanosConos
            );

            resultado.iteraciones++;
        }
    }

    return resultado;
}
