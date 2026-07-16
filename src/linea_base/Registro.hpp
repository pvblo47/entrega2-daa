#pragma once

#include "types.hpp"
#include <vector>
#include <cstddef>

// Registro de un cono con valor igual a cero (caso limite).
struct RegistroConoNulo {
    Coord3D bloque_raiz;     // Bloque base del cono
    std::size_t tamano_cono; // Cantidad de bloques en el cono
    double valor;            // Siempre 0.0 en este registro
};

// Acumula todos los eventos de la ejecucion para reporte final.
struct RegistroEjecucion {
    std::vector<RegistroConoNulo> conos_nulos;  // Conos con V(C) == 0
    std::size_t iteraciones_totales = 0;        // Numero de ciclos del While
    std::size_t conos_extraidos = 0;            // Conos con V(C) > 0 extraidos
    std::size_t bloques_inextraibles = 0;       // Bloques descartados por borde
};
