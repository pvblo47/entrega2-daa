#ifndef DAG_EXTRACCION_H
#define DAG_EXTRACCION_H

/*
 * ============================================================
 *  Grupo 01 – Entrega 2
 *  dag_extraccion.h — Interfaz de la Propuesta Algorítmica
 *                     Principal basada en DAG
 *
 *  Implementa el pseudocódigo de la Sección 2.4.1:
 *    Extraccion_DAG(Matriz_Bloques, Tipo_Politica)
 *
 *  Políticas soportadas (Secciones 2.3.1 y 2.3.2):
 *    "Max-Value"  → maximiza V(Cono(b))
 *    "Razon"      → maximiza V(Cono(b)) / |Cono(b)|
 * ============================================================
 */

#include "linea_base.h"   // reutiliza ModeloBloques, Metricas, ConjBloques

#include <vector>
#include <string>
#include <limits>

// ─────────────────────────────────────────────
//  Constantes de política (Sección 2.3)
// ─────────────────────────────────────────────

inline constexpr double NEG_INF = -std::numeric_limits<double>::infinity();

// ─────────────────────────────────────────────
//  Nodo del Grafo DAG
//
//  Cada bloque b ∈ B se representa como un Nodo.
//  Almacena conexiones bidireccionales (Sección 2.2):
//    predecesores → los 9 bloques de nivel z+1 que lo restringen
//    sucesores    → los bloques de nivel z-1 que dependen de él
// ─────────────────────────────────────────────

struct NodoDAG {
    int  id;                          // índice lineal único
    int  x, y, z;                     // coordenadas en el yacimiento
    double valor;                     // v(b) — valor económico

    bool activo;                      // true = aún en el yacimiento remanente R
    bool estatus_borde_valido;        // false = algún predecesor queda fuera de límites

    std::vector<NodoDAG*> predecesores;   // conexión hacia arriba  (z+1)
    std::vector<NodoDAG*> sucesores;      // conexión hacia abajo   (z-1)
};

// ─────────────────────────────────────────────
//  Grafo DAG del yacimiento
// ─────────────────────────────────────────────

struct GrafoDAG {
    std::vector<NodoDAG>   nodos;     // almacenamiento contiguo de todos los nodos
    int X, Y, Z;

    // Convierte (x,y,z) a índice lineal
    int indice(int x, int y, int z) const { return x * Y * Z + y * Z + z; }

    // Acceso por coordenada
    NodoDAG& nodo(int x, int y, int z) { return nodos[indice(x, y, z)]; }
    const NodoDAG& nodo(int x, int y, int z) const { return nodos[indice(x, y, z)]; }
};

// ─────────────────────────────────────────────
//  Métricas extendidas para la propuesta DAG
// ─────────────────────────────────────────────

struct MetricasDAG {
    int       conosExtraidos        = 0;
    long long bloquesExtraidos      = 0;
    double    beneficioTotal        = 0.0;
    long long iteracionesWhile      = 0;
    long long actualizacionesDinam  = 0;   // bloques actualizados por propagación
    long long conosCeroReportados   = 0;
    std::string politica;
    double    tiempoConstruirDAGMs  = 0.0;
    double    tiempoActualizarDescMs= 0.0;
};


// ─────────────────────────────────────────────
//  Declaraciones de funciones auxiliares DAG
// ─────────────────────────────────────────────

// Construye el DAG completo desde la Matriz_Bloques (Sección 2.2).
// Asigna predecesores, sucesores y estatus_borde_valido a cada nodo.
GrafoDAG Construir_DAG_Precedencias(const ModeloBloques& modelo);

// Calcula el cono invertido de un nodo recorriendo predecesores activos
// hacia arriba en el DAG (equivale a Calcular_Cierre_Cono del pseudocódigo).
// Devuelve el conjunto de punteros a nodos que forman el cono.
std::vector<NodoDAG*> Calcular_Cierre_Cono(NodoDAG* b, GrafoDAG& dag);

// Suma los valores económicos de los nodos en un cono.
double Sumar_Valores_Economicos(const std::vector<NodoDAG*>& cono);

// Preprocesamiento inicial: calcula Valores_Conos[] y Tamaños_Conos[]
// para todos los nodos activos con estatus_borde_valido == true.
void Preprocesar_Conos(GrafoDAG& dag,
                       std::vector<double>& Valores_Conos,
                       std::vector<int>&    Tamanios_Conos);

// Actualización dinámica acotada tras extraer un cono (Sección 2.2):
// recorre únicamente los sucesores de los nodos extraídos y actualiza
// Valores_Conos[] y Tamaños_Conos[] de los bloques afectados.
void Actualizar_Descendientes_Dinamico(const std::vector<NodoDAG*>& cono_extraido,
                                       GrafoDAG& dag,
                                       std::vector<double>& Valores_Conos,
                                       std::vector<int>&    Tamanios_Conos,
                                       MetricasDAG& metricas);

// ─────────────────────────────────────────────
//  Algoritmo principal (Sección 2.4.1)
//
//  Tipo_Politica: "Max-Value" | "Razon"
// ─────────────────────────────────────────────

MetricasDAG Extraccion_DAG(ModeloBloques& modelo,
                            const std::string& Tipo_Politica);

// Imprime resultados en formato de tabla (comparable con Línea Base).
void Imprimir_Resultados_DAG(const std::string& escala,
                              int X, int Y, int Z,
                              const MetricasDAG& m,
                              double ms);

#endif // DAG_EXTRACCION_H
