#pragma once

// Estado de extraibilidad de un bloque segun la regla de borde (Seccion 2.3.3)
enum class EstatusBorde {
    VALIDO,
    INEXTRAIBLE
};

// Nodo del grafo DAG: representa un bloque del yacimiento tridimensional
struct Bloque {
    int id;              // Identificador unico correlativo
    int x, y, z;         // Coordenadas en la matriz 3D
    double valor;        // Valor economico v(b)

    bool activo;         // true mientras no haya sido extraido
    bool reportadoCero;  // true si ya se registro como cono de valor 0

    EstatusBorde estatusBorde;
};
