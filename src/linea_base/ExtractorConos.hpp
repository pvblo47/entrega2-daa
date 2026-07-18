#pragma once

#include "types.hpp"
#include "ModeloBloques.hpp"
#include "Registro.hpp"

// Resultado completo de la ejecucion del algoritmo.
struct ResultadoExtraccion {
    // V(E) = Σ v(b) para b ∈ E
    double beneficio_total;
    // Conjunto E (sin duplicados)
    ConjuntoCoordenadas bloques_extraidos;
    // Log de eventos
    RegistroEjecucion registro;
    // Latencia total en milisegundos
    double tiempo_ms;
};

#include <chrono>

// Clase que encapsula el algoritmo de extraccion y construccion de conos.
class ExtractorConos {
    public:
        // Algoritmo principal de Extraccion Iterativa con politica First-Fit.
        ResultadoExtraccion extraccionLineaBase(ModeloBloques& modelo);

    private:
        std::chrono::high_resolution_clock::time_point tiempo_inicio_;
        uint64_t total_llamadas_recursivas_ = 0;
        // Reconstruye el cono invertido de un bloque objetivo de forma recursiva.
        ResultadoCono reconstruirConoRecursivo(int32_t x, int32_t y, int32_t z, const ModeloBloques& modelo);
        // Calcula el valor economico total V(C) = Σ v(b) para b ∈ C.
        [[nodiscard]] double calcularValorEconomico(const ConjuntoCoordenadas& cono, const ModeloBloques& modelo) noexcept;
        // Marca todos los bloques de un cono como extraidos en el modelo.
        void marcarComoExtraidos(const ConjuntoCoordenadas& cono, ModeloBloques& modelo) noexcept;
};
