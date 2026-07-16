#include "OperacionesCono.h"
#include <queue>
#include <vector>
#include <stdexcept>

// ---------------------------------------------------------------------------
// calcularCierreCono: Subrutina 2.4.3 - BFS con control de visitados
// ---------------------------------------------------------------------------
// Recorre hacia arriba (predecesores) desde bloque_origen acumulando
// todos los bloques activos que conforman el cono invertido.
// Complejidad: O(|Cono|) con |Cono| <= total de nodos del grafo.
// ---------------------------------------------------------------------------
std::vector<int> calcularCierreCono(int idOrigen, const GrafoDAG& grafo) {
    std::vector<int> conoResultado;
    const int total = grafo.totalNodos();

    const Bloque& origen = grafo.obtenerNodo(idOrigen);
    if (!origen.activo) {
        return conoResultado; // nodo inactivo: cono vacio
    }

    std::vector<bool> visitados(total, false);
    std::queue<int> cola;

    // Inicializacion con el bloque raiz
    cola.push(idOrigen);
    conoResultado.push_back(idOrigen);
    visitados[idOrigen] = true;

    while (!cola.empty()) {
        int actualId = cola.front();
        cola.pop();

        // Recorrer predecesores inmediatos (nivel z+1)
        for (int predId : grafo.predecesores(actualId)) {
            const Bloque& p = grafo.obtenerNodo(predId);
            if (p.activo && !visitados[predId]) {
                visitados[predId] = true;
                conoResultado.push_back(predId);
                cola.push(predId);
            }
        }
    }

    return conoResultado;
}

// ---------------------------------------------------------------------------
// sumarValoresEconomicos
// ---------------------------------------------------------------------------
// Suma los valores economicos de un conjunto de bloques (ids)
double sumarValoresEconomicos(const std::vector<int>& ids, const GrafoDAG& grafo) {
    double suma = 0.0;
    for (int id : ids) {
        suma += grafo.obtenerNodo(id).valor;
    }
    return suma;
}

// ---------------------------------------------------------------------------
// actualizarDescendientesDinamico: Subrutina 2.4.4
// ---------------------------------------------------------------------------
// Fase 1: BFS hacia abajo (sucesores) desde los nodos del cono removido
//         para identificar el conjunto D de descendientes afectados.
// Fase 2: Recalculo exclusivo de valoresConos y tamanosConos para cada d en D.
// ---------------------------------------------------------------------------
void actualizarDescendientesDinamico(
    const std::vector<int>& conoRemovido,
    GrafoDAG& grafo,
    std::vector<double>& valoresConos,
    std::vector<int>& tamanosConos,
    std::chrono::high_resolution_clock::time_point tiempo_inicio)
{
    const int total = grafo.totalNodos();
    std::vector<bool> visitados(total, false);
    std::queue<int> colaPropagacion;
    std::vector<int> descendientesAfectados;

    // --- Fase 1: Recorrido inverso por arcos sucesores ---
    for (int u : conoRemovido) {
        for (int sucId : grafo.sucesores(u)) {
            Bloque& s = grafo.obtenerNodo(sucId);
            if (s.activo && !visitados[sucId]) {
                visitados[sucId] = true;
                descendientesAfectados.push_back(sucId);
                colaPropagacion.push(sucId);
            }
        }
    }

    while (!colaPropagacion.empty()) {
        int actualId = colaPropagacion.front();
        colaPropagacion.pop();

        for (int sucId : grafo.sucesores(actualId)) {
            Bloque& s = grafo.obtenerNodo(sucId);
            if (s.activo && !visitados[sucId]) {
                visitados[sucId] = true;
                descendientesAfectados.push_back(sucId);
                colaPropagacion.push(sucId);
            }
        }
    }

    // --- Fase 2: Recalculo exclusivo confinado a la zona de impacto D ---
    int conteo_afectados = 0;
    for (int dId : descendientesAfectados) {
        // Control de timeout
        if (++conteo_afectados % 100 == 0) {
            auto ahora = std::chrono::high_resolution_clock::now();
            double transcurrido = std::chrono::duration<double, std::milli>(ahora - tiempo_inicio).count();
            if (transcurrido > 1800000.0) {
                throw std::runtime_error("Timeout");
            }
        }

        Bloque& d = grafo.obtenerNodo(dId);
        if (d.estatusBorde == EstatusBorde::VALIDO) {
            std::vector<int> nuevoCono = calcularCierreCono(dId, grafo);
            valoresConos[dId]  = sumarValoresEconomicos(nuevoCono, grafo);
            tamanosConos[dId]  = static_cast<int>(nuevoCono.size());
        } else {
            // Restriccion geometrica permanente: se ignora con -INFINITO
            valoresConos[dId]  = -1e18;
            tamanosConos[dId]  = 1;
        }
    }
}
