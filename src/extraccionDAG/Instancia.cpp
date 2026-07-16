#include "Instancia.h"
#include <random>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#endif

// ---------------------------------------------------------------------------
// leerInstanciaCSV: Lee escenario00.txt y calcula automaticamente valorBloque
// ---------------------------------------------------------------------------
std::vector<std::vector<std::vector<BloqueDatos>>> leerInstanciaCSV(
    const std::string& ruta,
    int& outX, int& outY, int& outZ)
{
    std::ifstream archivo(ruta);
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo " << ruta << "\n";
        outX = outY = outZ = 0;
        return {};
    }

    std::string linea;
    struct LineaDatos {
        int x, y, z;
        double tonelada;
        double metal1;
        double metal2;
        int roca;
    };
    std::vector<LineaDatos> lineas;
    int max_x = 0, max_y = 0, max_z = 0;

    while (std::getline(archivo, linea)) {
        if (linea.empty()) continue;
        std::stringstream ss(linea);
        std::string celda;

        int x = 0, y = 0, z = 0, roca = 0;
        double tonelada = 0.0, metal1 = 0.0, metal2 = 0.0;

        if (std::getline(ss, celda, ',')) x = std::stoi(celda);
        if (std::getline(ss, celda, ',')) y = std::stoi(celda);
        if (std::getline(ss, celda, ',')) z = std::stoi(celda);
        if (std::getline(ss, celda, ',')) tonelada = std::stod(celda);
        if (std::getline(ss, celda, ',')) metal1 = std::stod(celda);
        if (std::getline(ss, celda, ',')) metal2 = std::stod(celda);
        if (std::getline(ss, celda, ',')) roca = std::stoi(celda);

        if (x > max_x) max_x = x;
        if (y > max_y) max_y = y;
        if (z > max_z) max_z = z;

        lineas.push_back({x, y, z, tonelada, metal1, metal2, roca});
    }

    outX = max_x;
    outY = max_y;
    outZ = max_z;

    std::vector<std::vector<std::vector<BloqueDatos>>> mat(
        max_x, std::vector<std::vector<BloqueDatos>>(
            max_y, std::vector<BloqueDatos>(max_z)));

    for (const auto& l : lineas) {
        int ix = l.x - 1;
        int iy = l.y - 1;
        int iz = l.z - 1;

        if (ix >= 0 && ix < max_x && iy >= 0 && iy < max_y && iz >= 0 && iz < max_z) {
            BloqueDatos bd;
            bd.tonelada = l.tonelada;
            bd.metal1 = l.metal1;
            bd.metal2 = l.metal2;
            bd.roca = l.roca;
            bd.calcularValor(); // Calcular valorBloque automaticamente (Requisito 4 & 5)
            mat[ix][iy][iz] = bd;
        }
    }

    return mat;
}

// ---------------------------------------------------------------------------
// generarInstanciaEjemplo: modelo 5x5x3 descrito en Seccion 2.5.1
// ---------------------------------------------------------------------------
std::vector<std::vector<std::vector<BloqueDatos>>> generarInstanciaEjemplo() {
    const int X = 5, Y = 5, Z = 3;

    std::vector<std::vector<std::vector<BloqueDatos>>> mat(
        X, std::vector<std::vector<BloqueDatos>>(
            Y, std::vector<BloqueDatos>(Z)));

    // Nivel 0 (Z=0, profundo): bloque central valioso, resto esteril
    for (int x = 0; x < X; ++x) {
        for (int y = 0; y < Y; ++y) {
            mat[x][y][0].valorBloque = -20.0;
        }
    }
    mat[2][2][0].valorBloque = +150.0;

    // Nivel 1 (Z=1, intermedio): nucleo 3x3 con v=-5 (central +30), periferia v=-10
    for (int x = 0; x < X; ++x) {
        for (int y = 0; y < Y; ++y) {
            bool enNucleo = (x >= 1 && x <= 3 && y >= 1 && y <= 3);
            mat[x][y][1].valorBloque = enNucleo ? -5.0 : -10.0;
        }
    }
    mat[2][2][1].valorBloque = +30.0;

    // Nivel 2 (Z=2, superficie): todo esteril
    for (int x = 0; x < X; ++x) {
        for (int y = 0; y < Y; ++y) {
            mat[x][y][2].valorBloque = -2.0;
        }
    }

    return mat;
}

// ---------------------------------------------------------------------------
// generarInstanciaAleatoria
// ---------------------------------------------------------------------------
std::vector<std::vector<std::vector<BloqueDatos>>> generarInstanciaAleatoria(
    int X, int Y, int Z, double probPositivo, unsigned int semilla)
{
    std::mt19937 rng(semilla);
    std::uniform_real_distribution<double> distPos(1.0, 100.0);
    std::uniform_real_distribution<double> distNeg(-50.0, -1.0);
    std::uniform_real_distribution<double> distProb(0.0, 1.0);

    std::vector<std::vector<std::vector<BloqueDatos>>> mat(
        X, std::vector<std::vector<BloqueDatos>>(
            Y, std::vector<BloqueDatos>(Z)));

    for (int x = 0; x < X; ++x) {
        for (int y = 0; y < Y; ++y) {
            for (int z = 0; z < Z; ++z) {
                mat[x][y][z].valorBloque = (distProb(rng) < probPositivo)
                                           ? distPos(rng) : distNeg(rng);
            }
        }
    }

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
#elif defined(_WIN32)
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize / 1024;
    }
#endif
    return -1;
}

// ---------------------------------------------------------------------------
// generarInstanciaEscalable: replica exacta de crearInstanciaEscalable()
// de la linea base. Deterministico, sin semilla RNG.
// Todo esteril en -1.0, bloques ricos en z=0 con valor 200+(x*7+y*3)%50
// dentro del margen = Z-1 bloques por lado.
// ---------------------------------------------------------------------------
std::vector<std::vector<std::vector<BloqueDatos>>> generarInstanciaEscalable(int X, int Y, int Z) {
    std::vector<std::vector<std::vector<BloqueDatos>>> mat(
        X, std::vector<std::vector<BloqueDatos>>(Y, std::vector<BloqueDatos>(Z)));

    for (int x = 0; x < X; ++x) {
        for (int y = 0; y < Y; ++y) {
            for (int z = 0; z < Z; ++z) {
                mat[x][y][z].valorBloque = -1.0;
            }
        }
    }

    int margen = Z - 1;
    for (int x = margen; x < X - margen; ++x) {
        for (int y = margen; y < Y - margen; ++y) {
            mat[x][y][0].valorBloque = 200.0 + (x * 7 + y * 3) % 50;
        }
    }

    return mat;
}
