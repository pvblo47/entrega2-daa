#include "GeneradorInstancias.hpp"

// Construye el modelo de prueba minimal (5x5x3).
ModeloBloques crearInstanciaMinimal() {
    ModeloBloques m(5, 5, 3);
    // Nivel z=0 (fondo): esteril general
    for (int32_t x = 0; x < 5; ++x) {
        for (int32_t y = 0; y < 5; ++y) {
            m.setValor(x, y, 0, -3.0);
        }
    }
    // Bloque valioso central del yacimiento
    m.setValor(2, 2, 0, 100.0);
    // Nivel z=1: capa esteril intermedia
    for (int32_t x = 0; x < 5; ++x) {
        for (int32_t y = 0; y < 5; ++y) {
            m.setValor(x, y, 1, -1.0);
        }
    }
    // Nivel z=2 (superficie): capa superior esteril
    for (int32_t x = 0; x < 5; ++x) {
        for (int32_t y = 0; y < 5; ++y) {
            m.setValor(x, y, 2, -1.0);
        }
    }
    return m;
}

// Instancia de prueba mediana 5x5x4 con multiples conos positivos.
ModeloBloques crearInstanciaMediana() {
    ModeloBloques m(5, 5, 4);
    // Rellenar con esteril uniforme
    for (int32_t x = 0; x < 5; ++x) {
        for (int32_t y = 0; y < 5; ++y) {
            for (int32_t z = 0; z < 4; ++z) {
                m.setValor(x, y, z, -1.0);
            }
        }
    }
    // Bloques valiosos en el fondo (z=0), posiciones internas (x,y ∈ {1,2,3})
    m.setValor(1, 1, 0, 30.0);
    m.setValor(2, 2, 0, 45.0);
    m.setValor(3, 3, 0, 25.0);
    m.setValor(1, 3, 0, 20.0);
    m.setValor(3, 1, 0, 15.0);
    // Un cono de valor exactamente cero para verificar el reporte
    m.setValor(2, 2, 1, 9.0);
    return m;
}

// Instancia de prueba con un cono de valor exactamente cero (3x3x2).
ModeloBloques crearInstanciaConoNulo() {
    ModeloBloques m(3, 3, 2);
    // Nivel z=1 (superficie): todos esteriles = -1.0
    for (int32_t x = 0; x < 3; ++x) {
        for (int32_t y = 0; y < 3; ++y) {
            m.setValor(x, y, 1, -1.0);
        }
    }
    // Nivel z=0: bloque central que equilibra exactamente el costo del cono
    for (int32_t x = 0; x < 3; ++x) {
        for (int32_t y = 0; y < 3; ++y) {
            m.setValor(x, y, 0, -2.0);
        }
    }
    // V(C) = 9.0 + 9×(-1.0) = 0.0
    m.setValor(1, 1, 0, 9.0);
    return m;
}
