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

    // Nuevas variables (Requisito 3)
    double tonelada;
    double metal1;
    double metal2;
    int roca;
    double valorBloque;

    bool existe;         // false si la celda es vacia (topografia irregular)

    // Constructor por defecto
    Bloque() : id(-1), x(-1), y(-1), z(-1), valor(0.0), activo(true), reportadoCero(false),
               estatusBorde(EstatusBorde::VALIDO), tonelada(0.0), metal1(0.0), metal2(0.0),
               roca(0), valorBloque(0.0), existe(true) {}

    // Metodo para calcular valorBloque automaticamente (Requisito 4 & 5)
    void calcularValor() {
        valorBloque = (metal1 * 0.89 * 13228.0) - (tonelada * 13.0) - (tonelada * 25.0);
        valor = valorBloque;
    }
};
