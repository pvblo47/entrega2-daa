#pragma once

#include "ModeloBloques.hpp"

// Construye el modelo de prueba minimal (5x5x3).
ModeloBloques crearInstanciaMinimal();

// Instancia de prueba mediana 5x5x4 con multiples conos positivos.
ModeloBloques crearInstanciaMediana();

// Instancia de prueba con un cono de valor exactamente cero (3x3x2).
ModeloBloques crearInstanciaConoNulo();
