#pragma once

#include "ExtractorConos.hpp"
#include <string>

// Imprime una linea divisoria decorativa.
void imprimirSeparador(char c = '-', int ancho = 70);

// Encabezado de seccion numerada.
void imprimirEncabezado(const std::string& titulo);

// Imprime el reporte completo de una ejecucion del algoritmo.
void imprimirReporte(
    const ResultadoExtraccion& resultado,
    const std::string& nombre_instancia,
    int32_t mx, int32_t my, int32_t mz);
