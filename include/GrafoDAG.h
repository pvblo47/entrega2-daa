#pragma once

#include "Bloque.h"
#include "Instancia.h"
#include <vector>

// Grafo Dirigido Aciclico (DAG) de precedencias para el yacimiento 3D.
// Cada nodo es un Bloque. Los arcos modelan la regla Pred(x,y,z).
//
// Arcos bidireccionales:
//   predecesores[b.id] -> bloques del nivel z+1 que restringen a b  (hacia arriba)
//   sucesores[b.id]    -> bloques del nivel z-1 que dependen de b   (hacia abajo)
class GrafoDAG {
public:
    // Construye el grafo a partir de la matriz tridimensional de valores
    GrafoDAG(int X, int Y, int Z, const std::vector<std::vector<std::vector<BloqueDatos>>>& matrizValores);

    // Devuelve referencia al nodo por id
    Bloque& obtenerNodo(int id);
    const Bloque& obtenerNodo(int id) const;

    // Devuelve el nodo por coordenadas (asume coordenadas validas)
    Bloque& obtenerNodoPorCoord(int x, int y, int z);

    // Listas de adyacencia
    const std::vector<int>& predecesores(int id) const;
    const std::vector<int>& sucesores(int id) const;

    // Iteracion sobre todos los nodos
    const std::vector<Bloque>& nodos() const;

    int totalNodos() const;
    int X() const;
    int Y() const;
    int Z() const;

private:
    int _X, _Y, _Z;

    std::vector<Bloque> _nodos;

    // _pred[id] = lista de ids de predecesores inmediatos (nivel z+1)
    std::vector<std::vector<int>> _pred;
    // _suc[id]  = lista de ids de sucesores inmediatos (nivel z-1)
    std::vector<std::vector<int>> _suc;

    // Mapa de coordenadas a id: id = x*Y*Z + y*Z + z
    inline int coordAId(int x, int y, int z) const {
        return x * _Y * _Z + y * _Z + z;
    }
};
