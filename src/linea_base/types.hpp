#pragma once

#include <cstdint>
#include <unordered_set>
#include <optional>
#include <cstddef>

// Coordenada 3D de un bloque en el yacimiento.
struct Coord3D {
    int32_t x, y, z;

    bool operator==(const Coord3D& other) const noexcept {
        return x == other.x && y == other.y && z == other.z;
    }

    bool operator<(const Coord3D& other) const noexcept {
        if (x != other.x) return x < other.x;
        if (y != other.y) return y < other.y;
        return z < other.z;
    }
};

// Funcion hash para Coord3D, necesaria para unordered_set.
struct Coord3DHash {
    std::size_t operator()(const Coord3D& c) const noexcept {
        std::size_t seed = 0;
        auto hash_combine = [&](int32_t v) {
            seed ^= static_cast<std::size_t>(v + 0x9e3779b9)
                    + (seed << 6) + (seed >> 2);
        };
        hash_combine(c.x);
        hash_combine(c.y);
        hash_combine(c.z);
        return seed;
    }
};

// Conjunto de coordenadas con lookup O(1) amortizado.
using ConjuntoCoordenadas = std::unordered_set<Coord3D, Coord3DHash>;

// Sentinel para indicar que un bloque es INEXTRAIBLE.
using ResultadoCono = std::optional<ConjuntoCoordenadas>;

// Tolerancia para comparaciones de valores economicos en punto flotante.
// Un cono con |V(C)| <= EPSILON_VALOR se clasifica como cono de valor cero
// (se reporta, no se extrae). Un cono es positivo si V(C) > EPSILON_VALOR.
// Supuesto declarado en las Secciones 2.5.1 y 3.3.2 del informe.
inline constexpr double EPSILON_VALOR = 1e-6;
