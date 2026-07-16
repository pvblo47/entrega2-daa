#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstdint>
#include "ModeloBloques.hpp"
#include "ExtractorConos.hpp"
#include "Reporte.hpp"

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#endif

// Pruebas para Resultados de Linea Base en Tabla II-1 del Capitulo II del informe,
// Donde los datos documentados en esa tabla se realizaron en la fecha: 14/06/2026.
// a las 14:34:52. Por lo que los datos que dieron en su momento no se volveran a dar
// igual, debido a la diferencia de condiciones de ejecucion.

// Retorna Peak Working Set en KB (solo Windows).
size_t getPeakMemoryKB() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.PeakWorkingSetSize / 1024;
    }
#endif
    return 0;
}

// Genera una instancia escalable con conos efectivamente extraibles.
// Los bloques ricos se colocan en z=0 con margen suficiente para
// que sus predecesores quepan dentro de los limites del modelo.
ModeloBloques crearInstanciaEscalable(int32_t X, int32_t Y, int32_t Z) {
    ModeloBloques m(X, Y, Z);

    // Rellenar todo con esteril
    for (int32_t x = 0; x < X; ++x)
        for (int32_t y = 0; y < Y; ++y)
            for (int32_t z = 0; z < Z; ++z)
                m.setValor(x, y, z, -1.0);

    // Para que un bloque en z=0 sea extraible, necesita que sus
    // predecesores recursivos hasta la superficie (z = Z-1) quepan.
    // El margen requerido en X/Y es (Z-1) bloques a cada lado.
    int32_t margen = Z - 1;

    int bloques_ricos = 0;
    for (int32_t x = margen; x < X - margen; ++x) {
        for (int32_t y = margen; y < Y - margen; ++y) {
            // Valor suficientemente alto para compensar el costo del cono
            double valor = 200.0 + (x * 7 + y * 3) % 50;
            m.setValor(x, y, 0, valor);
            bloques_ricos++;
        }
    }

    std::cout << "  Bloques ricos inyectados en z=0: " << bloques_ricos << "\n";
    std::cout << "  Margen requerido por lado: " << margen << "\n";
    return m;
}

void ejecutarBenchmark(const std::string& nombre, int32_t X, int32_t Y, int32_t Z) {
    std::cout << "\n";
    imprimirSeparador('=');
    std::cout << "  BENCHMARK: " << nombre << " (" << X << "x" << Y << "x" << Z << ")\n";
    imprimirSeparador('=');

    int64_t total_bloques = static_cast<int64_t>(X) * Y * Z;
    std::cout << "  Total bloques: " << total_bloques << "\n";

    size_t mem_antes = getPeakMemoryKB();

    ModeloBloques modelo = crearInstanciaEscalable(X, Y, Z);
    ExtractorConos extractor;
    ResultadoExtraccion resultado = extractor.extraccionLineaBase(modelo);

    size_t mem_despues = getPeakMemoryKB();

    imprimirReporte(resultado, nombre, X, Y, Z);

    std::cout << "\n  [BENCHMARK RESUMEN]\n";
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "  Tiempo total:          " << resultado.tiempo_ms << " ms\n";
    std::cout << "  Beneficio total:       " << resultado.beneficio_total << "\n";
    std::cout << "  Bloques extraidos:     " << resultado.bloques_extraidos.size() << "\n";
    std::cout << "  Conos extraidos:       " << resultado.registro.conos_extraidos << "\n";
    std::cout << "  Iteraciones while:     " << resultado.registro.iteraciones_totales << "\n";
    std::cout << "  Bloques inextraibles:  " << resultado.registro.bloques_inextraibles << "\n";
    std::cout << "  Peak RAM (KB):         " << mem_despues << "\n";
    std::cout << "  Delta Peak RAM (KB):   " << (mem_despues - mem_antes) << "\n";
    imprimirSeparador('-');
}

int main() {
    std::cout << "\n";
    imprimirSeparador('*');
    std::cout << "  BENCHMARK DE ESCALABILIDAD - LINEA BASE FIRST-FIT\n";
    imprimirSeparador('*');

    // Prueba 1: Escala pequena - 10x10x3 = 300 bloques
    // Margen = 2, bloques ricos interiores: 6x6 = 36
    ejecutarBenchmark("Pequena", 10, 10, 3);

    // Prueba 2: Escala mediana - 30x30x3 = 2700 bloques
    // Margen = 2, bloques ricos interiores: 26x26 = 676
    ejecutarBenchmark("Mediana", 30, 30, 3);

    // Prueba 3: Escala grande - 50x50x3 = 7500 bloques
    // Margen = 2, bloques ricos interiores: 46x46 = 2116
    ejecutarBenchmark("Grande", 50, 50, 3);

    imprimirSeparador('*');
    std::cout << "  Benchmark completado.\n";
    imprimirSeparador('*');
    std::cout << "\n";

    return 0;
}
