#include "Reporte.hpp"
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>

// Imprime una linea divisoria decorativa.
void imprimirSeparador(char c, int ancho) {
    std::cout << std::string(ancho, c) << '\n';
}

// Encabezado de seccion numerada.
void imprimirEncabezado(const std::string& titulo) {
    imprimirSeparador('=');
    std::cout << "  " << titulo << '\n';
    imprimirSeparador('=');
}

// Imprime el reporte completo de una ejecucion del algoritmo.
void imprimirReporte(
    const ResultadoExtraccion& resultado,
    const std::string& nombre_instancia,
    int32_t mx, int32_t my, int32_t mz)
{
    imprimirEncabezado("REPORTE DE EJECUCION: " + nombre_instancia);

    std::cout << std::fixed << std::setprecision(4);

    // Dimensiones del modelo
    std::cout << "\n[MODELO]\n";
    std::cout << "  Dimensiones:         " << mx << " x " << my << " x " << mz
              << "  (" << (static_cast<long long>(mx)*my*mz) << " bloques totales)\n";

    // Metricas de ejecucion
    std::cout << "\n[METRICAS DE RENDIMIENTO]\n";
    std::cout << "  Tiempo de ejecucion: " << resultado.tiempo_ms << " ms\n";
    std::cout << "  Iteraciones (ciclos while): "
              << resultado.registro.iteraciones_totales << '\n';
    std::cout << "  Conos positivos extraidos:  "
              << resultado.registro.conos_extraidos << '\n';
    std::cout << "  Bloques inextraibles (borde): "
              << resultado.registro.bloques_inextraibles << '\n';

    // Valor total extraido
    std::cout << "\n[RESULTADO ECONOMICO]\n";
    std::cout << "  Beneficio total V(E): " << resultado.beneficio_total << '\n';
    std::cout << "  Bloques extraidos |E|: "
              << resultado.bloques_extraidos.size() << '\n';

    // Conjunto de bloques extraidos
    std::cout << "\n[BLOQUES EXTRAIDOS — conjunto E]\n";
    if (resultado.bloques_extraidos.empty()) {
        std::cout << "  (ningun bloque fue extraido)\n";
    } else {
        // Ordenar por z (nivel), luego x, luego y para presentacion legible
        std::vector<Coord3D> ordenados(
            resultado.bloques_extraidos.begin(),
            resultado.bloques_extraidos.end());
        std::sort(ordenados.begin(), ordenados.end(),
            [](const Coord3D& a, const Coord3D& b) {
                if (a.z != b.z) return a.z < b.z;
                if (a.x != b.x) return a.x < b.x;
                return a.y < b.y;
            });

        // Agrupar por nivel z (ahora el vector esta ordenado por z)
        int32_t z_actual = -1;
        for (const Coord3D& b : ordenados) {
            if (b.z != z_actual) {
                if (z_actual != -1) std::cout << '\n';
                z_actual = b.z;
                std::cout << "\n  Nivel z=" << z_actual << ":\n    ";
            }
            std::cout << "(" << b.x << "," << b.y << "," << b.z << ") ";
        }
        std::cout << '\n';
    }

    // Reporte de conos con valor cero
    std::cout << "\n[REPORTE — CONOS CON VALOR CERO V(C) = 0]\n";
    if (resultado.registro.conos_nulos.empty()) {
        std::cout << "  No se detectaron conos con valor exactamente cero.\n";
    } else {
        std::cout << "  Accion: detectados pero NO extraidos (sin beneficio).\n";
        std::cout << "  Cantidad de conos nulos: "
                  << resultado.registro.conos_nulos.size() << '\n';
        std::cout << "  Detalle:\n";
        for (const auto& reg : resultado.registro.conos_nulos) {
            std::cout << "    Bloque raiz: ("
                      << reg.bloque_raiz.x << ","
                      << reg.bloque_raiz.y << ","
                      << reg.bloque_raiz.z << ")"
                      << " | Tamaño cono: " << reg.tamano_cono
                      << " | V(C): " << reg.valor << '\n';
        }
    }

    imprimirSeparador();
    std::cout << '\n';
}
