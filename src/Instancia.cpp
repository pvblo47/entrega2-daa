#include "Instancia.h"
#include <random>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

// ---------------------------------------------------------------------------
// generarInstanciaEjemplo: modelo 5x5x3 descrito en Seccion 2.5.1
// ---------------------------------------------------------------------------
std::vector<std::vector<std::vector<double>>> generarInstanciaEjemplo() {
    const int X = 5, Y = 5, Z = 3;

    std::vector<std::vector<std::vector<double>>> mat(
        X, std::vector<std::vector<double>>(
            Y, std::vector<double>(Z, 0.0)));

    // Nivel 0 (Z=0, profundo): bloque central valioso, resto esteril
    for (int x = 0; x < X; ++x)
        for (int y = 0; y < Y; ++y)
            mat[x][y][0] = -20.0;
    mat[2][2][0] = +150.0;

    // Nivel 1 (Z=1, intermedio): nucleo 3x3 con v=-5 (central +30), periferia v=-10
    for (int x = 0; x < X; ++x) {
        for (int y = 0; y < Y; ++y) {
            bool enNucleo = (x >= 1 && x <= 3 && y >= 1 && y <= 3);
            mat[x][y][1] = enNucleo ? -5.0 : -10.0;
        }
    }
    mat[2][2][1] = +30.0;

    // Nivel 2 (Z=2, superficie): todo esteril
    for (int x = 0; x < X; ++x)
        for (int y = 0; y < Y; ++y)
            mat[x][y][2] = -2.0;

    return mat;
}

// ---------------------------------------------------------------------------
// generarInstanciaAleatoria
// ---------------------------------------------------------------------------
std::vector<std::vector<std::vector<double>>> generarInstanciaAleatoria(
    int X, int Y, int Z, double probPositivo, unsigned int semilla)
{
    std::mt19937 rng(semilla);
    std::uniform_real_distribution<double> distPos(1.0, 100.0);
    std::uniform_real_distribution<double> distNeg(-50.0, -1.0);
    std::uniform_real_distribution<double> distProb(0.0, 1.0);

    std::vector<std::vector<std::vector<double>>> mat(
        X, std::vector<std::vector<double>>(
            Y, std::vector<double>(Z, 0.0)));

    for (int x = 0; x < X; ++x)
        for (int y = 0; y < Y; ++y)
            for (int z = 0; z < Z; ++z)
                mat[x][y][z] = (distProb(rng) < probPositivo)
                               ? distPos(rng) : distNeg(rng);

    return mat;
}

// ---------------------------------------------------------------------------
// imprimirResultado
// ---------------------------------------------------------------------------
void imprimirResultado(
    const std::string& etiqueta,
    int X, int Y, int Z,
    double beneficio,
    int bloquesExtraidos,
    int iteraciones,
    int casosZero,
    double tiempoMs,
    long ramKB)
{
    std::cout << "\n=== " << etiqueta << " ===\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  Modelo               : " << X << " x " << Y << " x " << Z
              << "  (N=" << X*Y*Z << " bloques)\n";
    std::cout << "  Conos extraidos      : " << iteraciones << "\n";
    std::cout << "  Bloques extraidos    : " << bloquesExtraidos << "\n";
    std::cout << "  Beneficio total V(E) : " << beneficio << "\n";
    std::cout << "  Conos valor=0        : " << casosZero << "\n";
    std::cout << "  Tiempo ejecucion     : " << tiempoMs << " ms\n";
    if (ramKB >= 0)
        std::cout << "  Peak RAM             : " << ramKB << " KB\n";
    else
        std::cout << "  Peak RAM             : N/D\n";
}

// ---------------------------------------------------------------------------
// obtenerRAM_KB
//   Linux  -> /proc/self/status  (VmRSS en KB)
//   Otros  -> retorna -1 (no disponible sin dependencias externas)
// ---------------------------------------------------------------------------
long obtenerRAM_KB() {
#if defined(__linux__)
    std::ifstream status("/proc/self/status");
    std::string linea;
    while (std::getline(status, linea)) {
        if (linea.rfind("VmRSS:", 0) == 0) {
            std::istringstream ss(linea);
            std::string clave;
            long valor;
            ss >> clave >> valor;
            return valor;
        }
    }
#endif
    return -1;
}
