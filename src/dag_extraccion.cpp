/*
 * ============================================================
 *  Grupo 01 – Entrega 2
 *  dag_extraccion.cpp — Implementación de la Propuesta
 *                       Algorítmica Principal basada en DAG
 *
 *  Pseudocódigo fuente: Sección 2.4.1 del informe
 *    Extraccion_DAG(Matriz_Bloques, Tipo_Politica)
 * ============================================================
 */

#include "dag_extraccion.h"

#include <iostream>
#include <iomanip>
#include <unordered_set>
#include <stack>
#include <chrono>


// ─────────────────────────────────────────────
//  Construir_DAG_Precedencias
//
//  Pseudocódigo → línea:
//    Grafo_DAG ← Construir_DAG_Precedencias(Matriz_Bloques)
//
//  Para cada bloque b:
//    1. Crea el NodoDAG con sus coordenadas y valor.
//    2. Verifica si algún predecesor Pred(x,y,z) cae fuera
//       de los límites → estatus_borde_valido = false.
//    3. Enlaza punteros predecesores ↑ y sucesores ↓
//       en ambas direcciones (lista bidireccional, Sección 2.2).
// ─────────────────────────────────────────────

GrafoDAG Construir_DAG_Precedencias(const ModeloBloques& modelo)
{
    GrafoDAG dag;
    dag.X = modelo.X;
    dag.Y = modelo.Y;
    dag.Z = modelo.Z;

    const int N = modelo.X * modelo.Y * modelo.Z;
    dag.nodos.resize(N);

    // — Paso 1: inicializar todos los nodos —
    for (int x = 0; x < modelo.X; x++) {
        for (int y = 0; y < modelo.Y; y++) {
            for (int z = 0; z < modelo.Z; z++) {
                NodoDAG& n       = dag.nodo(x, y, z);
                n.id             = dag.indice(x, y, z);
                n.x              = x;
                n.y              = y;
                n.z              = z;
                n.valor          = modelo.valor[x][y][z];
                n.activo         = true;
                n.estatus_borde_valido = true;   // se refuta abajo si aplica
            }
        }
    }

    // — Paso 2: enlazar arcos y detectar bordes —
    //   Pred(x,y,z) = { (x+i, y+j, z+1) | i,j ∈ {-1,0,1} }
    for (int x = 0; x < modelo.X; x++) {
        for (int y = 0; y < modelo.Y; y++) {
            for (int z = 0; z < modelo.Z; z++) {
                NodoDAG& nodo_b = dag.nodo(x, y, z);

                // Bloque de superficie: no tiene predecesores en z+1
                if (z + 1 >= modelo.Z) continue;

                for (int i : {-1, 0, 1}) {
                    for (int j : {-1, 0, 1}) {
                        int px = x + i, py = y + j, pz = z + 1;

                        // Predecesor fuera de límites → bloque INEXTRAIBLE (Sección 2.3.3)
                        if (!modelo.dentroLimites(px, py, pz)) {
                            nodo_b.estatus_borde_valido = false;
                            // No se puede romper aquí: hay que marcar y seguir
                            // para que el flag quede seteado aunque haya otros i,j válidos
                        } else {
                            // Arco bidireccional
                            NodoDAG& nodo_p = dag.nodo(px, py, pz);
                            nodo_b.predecesores.push_back(&nodo_p);   // b → p  (hacia arriba)
                            nodo_p.sucesores.push_back(&nodo_b);      // p → b  (hacia abajo)
                        }
                    }
                }
            }
        }
    }

    return dag;
}

// ─────────────────────────────────────────────
//  Calcular_Cierre_Cono
//
//  Pseudocódigo → línea:
//    Cono_b ← Calcular_Cierre_Cono(b, Grafo_DAG)
//
//  Recorre el DAG hacia arriba (predecesores activos)
//  con DFS iterativo para construir el conjunto mínimo
//  de bloques que deben removerse para extraer b.
//  (Cierre mínimo local en G restringido a R, Sección 2.5.3)
// ─────────────────────────────────────────────

std::vector<NodoDAG*> Calcular_Cierre_Cono(NodoDAG* b, GrafoDAG& /*dag*/)
{
    std::vector<NodoDAG*>         cono;
    std::unordered_set<NodoDAG*>  visitados;
    std::stack<NodoDAG*>          pila;

    pila.push(b);

    while (!pila.empty()) {
        NodoDAG* actual = pila.top();
        pila.pop();

        if (!actual->activo)         continue;   // ya extraído: no suma al cono
        if (visitados.count(actual)) continue;   // evita revisitar nodos compartidos

        visitados.insert(actual);
        cono.push_back(actual);

        // Ascender recursivamente por los predecesores activos
        for (NodoDAG* pred : actual->predecesores) {
            if (pred->activo && !visitados.count(pred)) {
                pila.push(pred);
            }
        }
    }

    return cono;
}

// ─────────────────────────────────────────────
//  Sumar_Valores_Economicos
//
//  Pseudocódigo → línea:
//    Valores_Conos[b.id] ← Sumar_Valores_Económicos(Cono_b)
// ─────────────────────────────────────────────

double Sumar_Valores_Economicos(const std::vector<NodoDAG*>& cono)
{
    double suma = 0.0;
    for (const NodoDAG* n : cono) suma += n->valor;
    return suma;
}

// ─────────────────────────────────────────────
//  Preprocesar_Conos
//
//  Pseudocódigo → bloque:
//    PARA CADA bloque b EN Yacimiento_Remanente HACER
//      SI b.estatus_borde == VALIDO ENTONCES
//        Cono_b ← Calcular_Cierre_Cono(b, Grafo_DAG)
//        Valores_Conos[b.id] ← Sumar_Valores_Económicos(Cono_b)
//        Tamaños_Conos[b.id] ← Cardinalidad(Cono_b)
//      SINO
//        Valores_Conos[b.id] ← -INFINITO
//        Tamaños_Conos[b.id] ← 1
//      FIN SI
//    FIN PARA
// ─────────────────────────────────────────────

void Preprocesar_Conos(GrafoDAG& dag,
                       std::vector<double>& Valores_Conos,
                       std::vector<int>&    Tamanios_Conos)
{
    const int N = static_cast<int>(dag.nodos.size());
    Valores_Conos.assign(N, NEG_INF);
    Tamanios_Conos.assign(N, 1);

    for (NodoDAG& b : dag.nodos) {
        if (!b.activo) continue;

        if (b.estatus_borde_valido) {
            std::vector<NodoDAG*> cono = Calcular_Cierre_Cono(&b, dag);
            double val = Sumar_Valores_Economicos(cono);
            Valores_Conos [b.id] = val;
            Tamanios_Conos[b.id] = static_cast<int>(cono.size());
            if (val == 0.0) {
                std::cout << "[ALERTA DAG Preproceso] Cono con valor exactamente 0 en bloque id " << b.id 
                          << " (" << b.x << "," << b.y << "," << b.z << ")\n";
            }
        } else {
            // Bloque de borde: marcado como -INFINITO para nunca ser seleccionado
            Valores_Conos [b.id] = NEG_INF;
            Tamanios_Conos[b.id] = 1;
        }
    }

}

// ─────────────────────────────────────────────
//  Actualizar_Descendientes_Dinamico
//
//  Pseudocódigo → línea:
//    Actualizar_Descendientes_Dinamico(Cono_A_Extraer, Grafo_DAG,
//                                      Valores_Conos, Tamaños_Conos)
//
//  Mecanismo de actualización acotada (Sección 2.2):
//  Solo los sucesores (bloques debajo del cono extraído)
//  pueden haber cambiado su cono. Se recalculan únicamente ellos.
// ─────────────────────────────────────────────

void Actualizar_Descendientes_Dinamico(const std::vector<NodoDAG*>& cono_extraido,
                                       GrafoDAG& dag,
                                       std::vector<double>& Valores_Conos,
                                       std::vector<int>&    Tamanios_Conos,
                                       MetricasDAG& metricas)
{
    // Recolectar el conjunto de sucesores únicos afectados por la extracción
    std::unordered_set<NodoDAG*> afectados;

    for (NodoDAG* extraido : cono_extraido) {
        for (NodoDAG* sucesor : extraido->sucesores) {
            if (sucesor->activo && sucesor->estatus_borde_valido) {
                afectados.insert(sucesor);
            }
        }
    }

    // Recalcular solo los bloques afectados
    for (NodoDAG* b : afectados) {
        std::vector<NodoDAG*> nuevo_cono = Calcular_Cierre_Cono(b, dag);
        double val = Sumar_Valores_Economicos(nuevo_cono);
        Valores_Conos [b->id] = val;
        Tamanios_Conos[b->id] = static_cast<int>(nuevo_cono.size());
        metricas.actualizacionesDinam++;
        if (val == 0.0) {
            std::cout << "[ALERTA DAG Dinamico] Cono con valor exactamente 0 en bloque id " << b->id 
                      << " (" << b->x << "," << b->y << "," << b->z << ")\n";
        }
    }

}

// ─────────────────────────────────────────────
//  Extraccion_DAG  — Algoritmo Principal
//
//  Fiel al pseudocódigo de la Sección 2.4.1.
// ─────────────────────────────────────────────

MetricasDAG Extraccion_DAG(ModeloBloques& modelo,
                            const std::string& Tipo_Politica)
{
    MetricasDAG metricas;
    metricas.politica = Tipo_Politica;

    double      beneficio_total           = 0.0;
    ConjBloques bloques_extraidos_totales;

    // ── Transformación de la matriz en estructura de nodos (grafo DAG) ──
    auto t_start_dag = std::chrono::high_resolution_clock::now();
    GrafoDAG Grafo_DAG = Construir_DAG_Precedencias(modelo);
    auto t_end_dag = std::chrono::high_resolution_clock::now();
    metricas.tiempoConstruirDAGMs = std::chrono::duration<double, std::milli>(t_end_dag - t_start_dag).count();


    // ── Extracción de lista de punteros a bloques activos ──
    //    Yacimiento_Remanente ← Obtener_Nodos_Activos(Grafo_DAG)
    //    (Se trabaja directamente sobre dag.nodos para O(N) en memoria)

    // ── Instancias de arreglos indexados contiguos en RAM ──
    std::vector<double> Valores_Conos;
    std::vector<int>    Tamanios_Conos;

    // ── Preprocesamiento exhaustivo inicial ──
    Preprocesar_Conos(Grafo_DAG, Valores_Conos, Tamanios_Conos);

    bool continuar_extraccion = true;

    // ── Ciclo principal MIENTRAS ──────────────────────────────────────
    while (continuar_extraccion) {
        metricas.iteracionesWhile++;

        NodoDAG* bloque_objetivo = nullptr;
        double   metrica_maxima  = NEG_INF;

        // ── Evaluación exhaustiva de la política sobre métricas persistentes ──
        for (NodoDAG& b : Grafo_DAG.nodos) {

            if (!b.activo)                          continue;
            if (Valores_Conos[b.id] <= 0.0)        continue;
            if (Valores_Conos[b.id] == NEG_INF)    continue;

            double metrica_actual = 0.0;

            if (Tipo_Politica == "Max-Value") {
                // Sección 2.3.1: b* tal que V(Cono(b*)) = max V(Cono(b))
                metrica_actual = Valores_Conos[b.id];

            } else if (Tipo_Politica == "Razon") {
                // Sección 2.3.2: b* tal que V(Cono(b*))/|Cono(b*)| = max ratio
                metrica_actual = Valores_Conos[b.id]
                               / static_cast<double>(Tamanios_Conos[b.id]);
            }

            if (metrica_actual > metrica_maxima) {
                metrica_maxima  = metrica_actual;
                bloque_objetivo = &b;
            }
        }

        // ── Criterio y condición de término (Sección 2.4.2) ──
        if (bloque_objetivo == nullptr) {
            continuar_extraccion = false;

        } else {
            // ── Reconstrucción acotada del cono óptimo identificado ──
            std::vector<NodoDAG*> Cono_A_Extraer =
                Calcular_Cierre_Cono(bloque_objetivo, Grafo_DAG);

            // ── Consolidación de la extracción ──
            beneficio_total += Valores_Conos[bloque_objetivo->id];

            // Reportar cono de valor exactamente cero (Sección 2.4.2)
            if (Valores_Conos[bloque_objetivo->id] == 0.0) {
                metricas.conosCeroReportados++;
            }

            // ── Desactivación física de nodos extraídos ──
            for (NodoDAG* u : Cono_A_Extraer) {
                u->activo = false;
                bloques_extraidos_totales.insert({u->x, u->y, u->z});
            }

            metricas.conosExtraidos++;

            // ── Actualización dinámica acotada ──
            auto t_start_act = std::chrono::high_resolution_clock::now();
            Actualizar_Descendientes_Dinamico(
                Cono_A_Extraer, Grafo_DAG,
                Valores_Conos, Tamanios_Conos, metricas);
            auto t_end_act = std::chrono::high_resolution_clock::now();
            metricas.tiempoActualizarDescMs += std::chrono::duration<double, std::milli>(t_end_act - t_start_act).count();
        }
    }   // fin MIENTRAS


    metricas.beneficioTotal   = beneficio_total;
    metricas.bloquesExtraidos = static_cast<long long>(bloques_extraidos_totales.size());
    return metricas;
}

// ─────────────────────────────────────────────
//  Imprimir_Resultados_DAG
// ─────────────────────────────────────────────

void Imprimir_Resultados_DAG(const std::string& escala,
                              int X, int Y, int Z,
                              const MetricasDAG& m,
                              double ms)
{
    long long totalBloques = (long long)X * Y * Z;

    std::cout << "\n------------------------------------------------------\n";
    std::cout << "  [DAG – " << m.politica << "]  Escala: " << escala
              << "  (" << X << "x" << Y << "x" << Z << ")\n";
    std::cout << "------------------------------------------------------\n";
    std::cout << "  Total de Bloques (n)               : "
              << std::setw(8) << totalBloques                        << " \n";
    std::cout << "  Conos Positivos Extraídos           : "
              << std::setw(8) << m.conosExtraidos                    << " \n";
    std::cout << "  Bloques Extraídos (|E|)             : "
              << std::setw(8) << m.bloquesExtraidos                  << " \n";
    std::cout << "  Beneficio Total V(E)                : "
              << std::setw(8) << std::fixed << std::setprecision(2)
              << m.beneficioTotal                                     << " \n";
    std::cout << "  Iteraciones ciclo Mientras          : "
              << std::setw(8) << m.iteracionesWhile                  << " \n";
    std::cout << "  Actualizaciones Dinámicas           : "
              << std::setw(8) << m.actualizacionesDinam              << " \n";
    std::cout << "  Conos de Valor Cero Reportados      : "
              << std::setw(8) << m.conosCeroReportados               << " \n";
    std::cout << "  Tiempo de Ejecución (ms)            : "
              << std::setw(8) << std::setprecision(2) << ms          << " \n";
    std::cout << "------------------------------------------------------\n";
}
