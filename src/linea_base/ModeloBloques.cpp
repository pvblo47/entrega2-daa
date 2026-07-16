#include "ModeloBloques.hpp"
#include <cassert>

// Traduce coordenadas 3D a indice lineal.
std::size_t ModeloBloques::idx(int32_t x, int32_t y, int32_t z) const noexcept {
    return static_cast<std::size_t>(z) * max_y * max_x
         + static_cast<std::size_t>(y) * max_x
         + static_cast<std::size_t>(x);
}

// Construye el modelo con dimensiones dadas, todos los bloques en 0.0.
ModeloBloques::ModeloBloques(int32_t mx, int32_t my, int32_t mz)
    : max_x(mx), max_y(my), max_z(mz),
      valores_(static_cast<std::size_t>(mx) * my * mz, 0.0),
      extraido_(static_cast<std::size_t>(mx) * my * mz, false)
{
    assert(mx > 0 && my > 0 && mz > 0);
}

// Establece el valor economico de un bloque.
void ModeloBloques::setValor(int32_t x, int32_t y, int32_t z, double valor) {
    valores_[idx(x, y, z)] = valor;
}

// Retorna el valor economico de un bloque.
double ModeloBloques::getValor(int32_t x, int32_t y, int32_t z) const noexcept {
    return valores_[idx(x, y, z)];
}

// Retorna true si el bloque ya fue extraido.
bool ModeloBloques::estaExtraido(int32_t x, int32_t y, int32_t z) const noexcept {
    return extraido_[idx(x, y, z)];
}

// Marca un bloque como extraido.
void ModeloBloques::marcarExtraido(int32_t x, int32_t y, int32_t z) noexcept {
    extraido_[idx(x, y, z)] = true;
}

// Verifica si una coordenada esta dentro de los limites del modelo.
bool ModeloBloques::dentroLimites(int32_t x, int32_t y, int32_t z) const noexcept {
    return (x >= 0 && x < max_x)
        && (y >= 0 && y < max_y)
        && (z >= 0 && z < max_z);
}

// Verifica si un bloque es de superficie.
bool ModeloBloques::esSuperficie(int32_t x, int32_t y, int32_t z) const noexcept {
    return !dentroLimites(x, y, z + 1);
}

// Numero total de bloques en el modelo.
std::size_t ModeloBloques::totalBloques() const noexcept {
    return valores_.size();
}
