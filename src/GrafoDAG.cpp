#include "GrafoDAG.h"
#include <stdexcept>

// ---------------------------------------------------------------------------
// Constructor: Construir_DAG_Precedencias (Pseudocodigo Seccion 2.4.2)
// ---------------------------------------------------------------------------
GrafoDAG::GrafoDAG(int X, int Y, int Z,
                   const std::vector<std::vector<std::vector<double>>>& matrizValores)
    : _X(X), _Y(Y), _Z(Z)
{
    const int total = X * Y * Z;
    _nodos.resize(total);
    _pred.resize(total);
    _suc.resize(total);

    // --- Fase 1: Instanciacion lineal de nodos ---
    int contadorId = 0;
    for (int x = 0; x < X; ++x) {
        for (int y = 0; y < Y; ++y) {
            for (int z = 0; z < Z; ++z) {
                Bloque b;
                b.id            = contadorId;
                b.x             = x;
                b.y             = y;
                b.z             = z;
                b.valor         = matrizValores[x][y][z];
                b.activo        = true;
                b.reportadoCero = false;
                b.estatusBorde  = EstatusBorde::VALIDO;

                _nodos[contadorId] = b;
                ++contadorId;
            }
        }
    }

    // --- Fase 2: Enlazado geometrico y marcado de bordes ---
    for (int id = 0; id < total; ++id) {
        Bloque& b = _nodos[id];

        // Bloques de la capa superior (z = Z-1) son superficie: no tienen predecesores
        if (b.z + 1 >= Z) {
            continue; // caso base: bloque de superficie, valido sin predecesores
        }

        // Verificar los 9 predecesores inmediatos en nivel z+1
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                int nx = b.x + i;
                int ny = b.y + j;
                int nz = b.z + 1;

                if (nx >= 0 && nx < X && ny >= 0 && ny < Y) {
                    // Predecesor valido dentro de los limites
                    int idPred = coordAId(nx, ny, nz);
                    _pred[id].push_back(idPred);
                    _suc[idPred].push_back(id);
                } else {
                    // Predecesor fuera del modelo: marcado permanente como INEXTRAIBLE
                    b.estatusBorde = EstatusBorde::INEXTRAIBLE;
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Accesores
// ---------------------------------------------------------------------------

Bloque& GrafoDAG::obtenerNodo(int id) {
    return _nodos[id];
}

const Bloque& GrafoDAG::obtenerNodo(int id) const {
    return _nodos[id];
}

Bloque& GrafoDAG::obtenerNodoPorCoord(int x, int y, int z) {
    return _nodos[coordAId(x, y, z)];
}

const std::vector<int>& GrafoDAG::predecesores(int id) const {
    return _pred[id];
}

const std::vector<int>& GrafoDAG::sucesores(int id) const {
    return _suc[id];
}

const std::vector<Bloque>& GrafoDAG::nodos() const {
    return _nodos;
}

int GrafoDAG::totalNodos() const {
    return static_cast<int>(_nodos.size());
}

int GrafoDAG::X() const { return _X; }
int GrafoDAG::Y() const { return _Y; }
int GrafoDAG::Z() const { return _Z; }
