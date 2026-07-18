#pragma once

#include "types.hpp"
#include <vector>
#include <cstddef>

// Registro de un cono con valor igual a cero (caso limite).
struct RegistroConoNulo {
    // Bloque base del cono
    Coord3D bloque_raiz;
    // Cantidad de bloques en el cono
    std::size_t tamano_cono;
    // Siempre 0.0 en este registro
    double valor;
};

// Acumula todos los eventos de la ejecucion para reporte final.
struct RegistroEjecucion {
    // Conos con V(C) == 0
    std::vector<RegistroConoNulo> conos_nulos;
    // Numero de ciclos del While
    std::size_t iteraciones_totales = 0;
    // Conos con V(C) > 0 extraidos
    std::size_t conos_extraidos = 0;
    // Bloques descartados por borde
    std::size_t bloques_inextraibles = 0;
};
