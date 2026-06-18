#pragma once

#include "GrafoDAG.h"
#include <vector>
#include <unordered_set>

// Calcula el cierre minimo del cono invertido con raiz en bloque_origen
// usando BFS con control de visitados (Subrutina 2.4.3).
// Retorna el conjunto de ids de bloques activos que conforman el cono.
std::vector<int> calcularCierreCono(int idOrigen, const GrafoDAG& grafo);

// Suma los valores economicos de un conjunto de bloques (ids)
double sumarValoresEconomicos(const std::vector<int>& ids, const GrafoDAG& grafo);

// Actualiza dinamicamente los vectores de metricas solo para los
// descendientes afectados por la remocion de conoRemovido (Subrutina 2.4.4).
void actualizarDescendientesDinamico(
    const std::vector<int>& conoRemovido,
    GrafoDAG& grafo,
    std::vector<double>& valoresConos,
    std::vector<int>& tamanosConos
);
