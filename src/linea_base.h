#ifndef LINEA_BASE_H
#define LINEA_BASE_H

/*
 * ============================================================
 *  Grupo 01 – Entrega 2
 *  linea_base.h — Interfaz del algoritmo First-Fit
 * ============================================================
 */

#include <vector>
#include <set>
#include <tuple>
#include <string>

// ─────────────────────────────────────────────
//  Tipos de datos
// ─────────────────────────────────────────────

using Coord       = std::tuple<int,int,int>;
using ConjBloques = std::set<Coord>;

// ─────────────────────────────────────────────
//  Estructura: Modelo de Bloques 3D
// ─────────────────────────────────────────────

struct ModeloBloques {
    int X, Y, Z;
    std::vector<std::vector<std::vector<double>>> valor;
    std::vector<std::vector<std::vector<bool>>>   extraido;

    ModeloBloques(int x, int y, int z);

    bool dentroLimites(int x, int y, int z) const;
};

// ─────────────────────────────────────────────
//  Estructura: Métricas de ejecución
// ─────────────────────────────────────────────

struct Metricas {
    int       conosExtraidos            = 0;
    long long bloquesExtraidos          = 0;
    double    beneficioTotal            = 0.0;
    long long iteracionesWhile          = 0;
    long long evaluacionesInextraibles  = 0;
    long long conosCeroReportados       = 0;
};

// ─────────────────────────────────────────────
//  Declaraciones de funciones
// ─────────────────────────────────────────────

// Subrutina auxiliar: reconstruye el cono invertido de (x,y,z).
// Devuelve true si el cono es extraíble, false si es INEXTRAIBLE.
bool Reconstruir_Cono_Recursivo(int x, int y, int z,
                                 const ModeloBloques& modelo,
                                 ConjBloques& cono,
                                 Metricas& metricas);

// Calcula el valor económico total de un cono.
double Calcular_Valor_Economico(const ConjBloques& cono,
                                const ModeloBloques& modelo);

// Marca todos los bloques del cono como extraídos en la matriz.
void Marcar_Como_Extraidos(const ConjBloques& cono, ModeloBloques& modelo);

// Une dos conjuntos de bloques (destino ← destino ∪ origen).
void Unir_Conjuntos(ConjBloques& destino, const ConjBloques& origen);

// Registra un cono de valor cero en el log de métricas.
void Registrar_En_Log(int x, int y, int z,
                      int tamanioCono, double valorCono,
                      Metricas& metricas);

// Algoritmo principal: Extracción Iterativa con política First-Fit.
Metricas Extraccion_Linea_Base(ModeloBloques& modelo);

// Genera una instancia sintética con valores aleatorios en [-10, 10].
ModeloBloques Generar_Instancia(int X, int Y, int Z, int semilla = 42);

// Imprime los resultados en formato de tabla.
void Imprimir_Resultados(const std::string& escala,
                         int X, int Y, int Z,
                         const Metricas& m,
                         double ms);

#endif // LINEA_BASE_H
