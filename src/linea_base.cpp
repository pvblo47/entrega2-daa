/*
 * ============================================================
 *  Grupo 01 – Entrega 2
 *  linea_base.cpp — Implementación del algoritmo First-Fit
 * ============================================================
 *  Integrantes:
 *    Maximiliano Bezares Leyton
 *    Gabriel Briones Arce
 *    Pablo Bravo Bascuñan
 *
 *  Asignatura: Diseño y Análisis de Algoritmos
 *  Docente   : Aldo Quelopana Retamal
 * ============================================================
 */

#include "linea_base.h"

#include <iostream>
#include <iomanip>
#include <random>

// ─────────────────────────────────────────────
//  ModeloBloques
// ─────────────────────────────────────────────

ModeloBloques::ModeloBloques(int x, int y, int z)
    : X(x), Y(y), Z(z),
      valor   (x, std::vector<std::vector<double>>(y, std::vector<double>(z, 0.0))),
      extraido(x, std::vector<std::vector<bool>>  (y, std::vector<bool>  (z, false)))
{}

bool ModeloBloques::dentroLimites(int x, int y, int z) const
{
    return x >= 0 && x < X &&
           y >= 0 && y < Y &&
           z >= 0 && z < Z;
}

// ─────────────────────────────────────────────
//  Reconstruir_Cono_Recursivo
//  (Pseudocódigo Sección 4.3.2)
// ─────────────────────────────────────────────

bool Reconstruir_Cono_Recursivo(int x, int y, int z,
                                 const ModeloBloques& modelo,
                                 ConjBloques& cono,
                                 Metricas& metricas)
{
    // Caso 1: bloque ya extraído → conjunto vacío, éxito
    if (modelo.extraido[x][y][z]) {
        return true;
    }

    // Caso 2: bloque de superficie (caso base)
    if (!modelo.dentroLimites(x, y, z + 1)) {
        cono.insert({x, y, z});
        return true;
    }

    // Caso 3: calcular predecesores en nivel z+1
    //         Pred(x,y,z) = { (x+i, y+j, z+1) | i,j ∈ {-1,0,1} }
    std::vector<Coord> predecesores;
    for (int i : {-1, 0, 1}) {
        for (int j : {-1, 0, 1}) {
            int px = x + i, py = y + j, pz = z + 1;

            // Caso 4: predecesor fuera de límites → INEXTRAIBLE (Sección 2.3.3)
            if (!modelo.dentroLimites(px, py, pz)) {
                metricas.evaluacionesInextraibles++;
                return false;
            }

            if (!modelo.extraido[px][py][pz]) {
                predecesores.push_back({px, py, pz});
            }
        }
    }

    // Construir cono recursivamente
    cono.insert({x, y, z});
    for (auto& [px, py, pz] : predecesores) {
        if (!Reconstruir_Cono_Recursivo(px, py, pz, modelo, cono, metricas)) {
            return false;   // INEXTRAIBLE propagado
        }
    }
    return true;
}

// ─────────────────────────────────────────────
//  Calcular_Valor_Economico
// ─────────────────────────────────────────────

double Calcular_Valor_Economico(const ConjBloques& cono,
                                const ModeloBloques& modelo)
{
    double suma = 0.0;
    for (auto& [x, y, z] : cono) {
        suma += modelo.valor[x][y][z];
    }
    return suma;
}

// ─────────────────────────────────────────────
//  Marcar_Como_Extraidos
// ─────────────────────────────────────────────

void Marcar_Como_Extraidos(const ConjBloques& cono, ModeloBloques& modelo)
{
    for (auto& [x, y, z] : cono) {
        modelo.extraido[x][y][z] = true;
    }
}

// ─────────────────────────────────────────────
//  Unir_Conjuntos
// ─────────────────────────────────────────────

void Unir_Conjuntos(ConjBloques& destino, const ConjBloques& origen)
{
    for (auto& b : origen) {
        destino.insert(b);
    }
}

// ─────────────────────────────────────────────
//  Registrar_En_Log
// ─────────────────────────────────────────────

void Registrar_En_Log(int x, int y, int z,
                      int tamanioCono, double valorCono,
                      Metricas& metricas)
{
    metricas.conosCeroReportados++;
    // Descomentar para ver detalle en consola:
    // std::cout << "[LOG] Cono V=0 en (" << x << "," << y << "," << z
    //           << ") tamaño=" << tamanioCono << " V=" << valorCono << "\n";
    (void)x; (void)y; (void)z; (void)tamanioCono; (void)valorCono;
}

// ─────────────────────────────────────────────
//  Extraccion_Linea_Base
//  (Pseudocódigo Sección 4.3.1)
// ─────────────────────────────────────────────

Metricas Extraccion_Linea_Base(ModeloBloques& modelo)
{
    Metricas    metricas;
    double      beneficio_total           = 0.0;
    ConjBloques bloques_extraidos_totales;
    bool        hubo_extraccion           = true;

    while (hubo_extraccion) {
        hubo_extraccion = false;
        metricas.iteracionesWhile++;

        bool salir_xyz = false;

        for (int x = 0; x < modelo.X && !salir_xyz; x++) {
            for (int y = 0; y < modelo.Y && !salir_xyz; y++) {
                for (int z = 0; z < modelo.Z && !salir_xyz; z++) {

                    if (modelo.extraido[x][y][z]) continue;

                    // Reconstrucción del cono
                    ConjBloques cono_candidato;
                    bool extraible = Reconstruir_Cono_Recursivo(
                                        x, y, z, modelo, cono_candidato, metricas);

                    // Control de inextraibilidad (Sección 2.3.3)
                    if (!extraible) continue;

                    double valor_cono = Calcular_Valor_Economico(cono_candidato, modelo);

                    // Política First-Fit
                    if (valor_cono > 0.0) {
                        beneficio_total += valor_cono;
                        Unir_Conjuntos(bloques_extraidos_totales, cono_candidato);
                        Marcar_Como_Extraidos(cono_candidato, modelo);

                        metricas.conosExtraidos++;
                        hubo_extraccion = true;
                        salir_xyz       = true;   // reiniciar escaneo

                    } else if (valor_cono == 0.0) {
                        Registrar_En_Log(x, y, z,
                                         static_cast<int>(cono_candidato.size()),
                                         valor_cono, metricas);
                    }
                }
            }
        }
    }

    metricas.beneficioTotal   = beneficio_total;
    metricas.bloquesExtraidos = static_cast<long long>(bloques_extraidos_totales.size());
    return metricas;
}

// ─────────────────────────────────────────────
//  Generar_Instancia
// ─────────────────────────────────────────────

ModeloBloques Generar_Instancia(int X, int Y, int Z, int semilla)
{
    ModeloBloques m(X, Y, Z);
    std::mt19937 rng(semilla);
    std::uniform_real_distribution<double> dist(-10.0, 10.0);

    for (int x = 0; x < X; x++)
        for (int y = 0; y < Y; y++)
            for (int z = 0; z < Z; z++)
                m.valor[x][y][z] = dist(rng);

    return m;
}

// ─────────────────────────────────────────────
//  Imprimir_Resultados
// ─────────────────────────────────────────────

void Imprimir_Resultados(const std::string& escala,
                         int X, int Y, int Z,
                         const Metricas& m,
                         double ms)
{
    long long totalBloques = (long long)X * Y * Z;

    std::cout << "\n------------------------------------------------------\n";
    std::cout << "  Escala: " << escala
              << "  (" << X << "x" << Y << "x" << Z << ")\n";
    std::cout << "------------------------------------------------------\n";
    std::cout << "  Total de Bloques (n)               : " << std::setw(8) << totalBloques           << " \n";
    std::cout << "  Conos Positivos Extraídos           : " << std::setw(8) << m.conosExtraidos      << " \n";
    std::cout << "  Bloques Extraídos (|E|)             : " << std::setw(8) << m.bloquesExtraidos    << " \n";
    std::cout << " Beneficio Total V(E)                : " << std::setw(8) << std::fixed
              << std::setprecision(2) << m.beneficioTotal                                             << " \n";
    std::cout << "  Iteraciones ciclo While             : " << std::setw(8) << m.iteracionesWhile    << " \n";
    std::cout << "  Evaluaciones Inextraíbles           : " << std::setw(8) << m.evaluacionesInextraibles << " \n";
    std::cout << "  Conos de Valor Cero Reportados      : " << std::setw(8) << m.conosCeroReportados << " \n";
    std::cout << " Tiempo de Ejecución (ms)            : " << std::setw(8)
              << std::setprecision(2) << ms                                                           << " \n";
    std::cout << "------------------------------------------------------\n";
}
