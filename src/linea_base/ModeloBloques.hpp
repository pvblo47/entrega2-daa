#pragma once

#include <vector>
#include <cstdint>
#include <cstddef>

// Modelo tridimensional del yacimiento minero.
class ModeloBloques {
public:
    const int32_t max_x; // Numero de bloques en eje X (columnas)
    const int32_t max_y; // Numero de bloques en eje Y (filas)
    const int32_t max_z; // Numero de bloques en eje Z (niveles)

private:
    std::vector<double> valores_;  // Valores economicos v(b)
    std::vector<bool> extraido_;   // Estado de extraccion por bloque

    // Traduce coordenadas 3D a indice lineal.
    [[nodiscard]] std::size_t idx(int32_t x, int32_t y, int32_t z) const noexcept;

public:
    // Construye el modelo con dimensiones dadas, todos los bloques en 0.0.
    ModeloBloques(int32_t mx, int32_t my, int32_t mz);

    // Establece el valor economico de un bloque.
    void setValor(int32_t x, int32_t y, int32_t z, double valor);

    // Retorna el valor economico de un bloque.
    [[nodiscard]] double getValor(int32_t x, int32_t y, int32_t z) const noexcept;

    // Retorna true si el bloque ya fue extraido.
    [[nodiscard]] bool estaExtraido(int32_t x, int32_t y, int32_t z) const noexcept;

    // Marca un bloque como extraido.
    void marcarExtraido(int32_t x, int32_t y, int32_t z) noexcept;

    // Verifica si una coordenada esta dentro de los limites del modelo.
    [[nodiscard]] bool dentroLimites(int32_t x, int32_t y, int32_t z) const noexcept;

    // Verifica si un bloque es de superficie.
    [[nodiscard]] bool esSuperficie(int32_t x, int32_t y, int32_t z) const noexcept;

    // Numero total de bloques en el modelo.
    [[nodiscard]] std::size_t totalBloques() const noexcept;
};
