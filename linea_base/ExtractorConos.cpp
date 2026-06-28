#include "ExtractorConos.hpp"
#include <chrono>
#include <algorithm>

ResultadoCono ExtractorConos::reconstruirConoRecursivo(int32_t x, int32_t y, int32_t z, const ModeloBloques& modelo)
{
    // Caso 1: bloque ya extraido
    if (modelo.estaExtraido(x, y, z)) {
        return ConjuntoCoordenadas{};
    }
    // Caso 2: bloque de superficie (caso base)
    if (modelo.esSuperficie(x, y, z)) {
        return ConjuntoCoordenadas{{ Coord3D{x, y, z} }};
    }
    // Caso 3: calcular predecesores en el nivel z+1
    ConjuntoCoordenadas predecesores_activos;
    for (int32_t i = -1; i <= 1; ++i) {
        for (int32_t j = -1; j <= 1; ++j) {
            const int32_t px = x + i;
            const int32_t py = y + j;
            const int32_t pz = z + 1;
            // Caso 4: predecesor fuera de limites -> INEXTRAIBLE
            if (!modelo.dentroLimites(px, py, pz)) {
                return std::nullopt;
            }
            // Solo incluir predecesores que aun no fueron extraidos
            if (!modelo.estaExtraido(px, py, pz)) {
                predecesores_activos.insert(Coord3D{px, py, pz});
            }
        }
    }
    // Construir el cono recursivamente desde el bloque actual
    ConjuntoCoordenadas cono;
    cono.insert(Coord3D{x, y, z});
    for (const Coord3D& pred : predecesores_activos) {
        ResultadoCono resultado_recursivo = reconstruirConoRecursivo(
            pred.x, pred.y, pred.z, modelo);
        // Si algun predecesor es inextraible, el cono completo es inextraible
        if (!resultado_recursivo.has_value()) {
            return std::nullopt;
        }
        // Union del conjunto (cono <- cono ∪ resultado_recursivo)
        for (const Coord3D& b : resultado_recursivo.value()) {
            cono.insert(b);
        }
    }
    return cono;
}

// Calcula el valor economico total V(C) = Σ v(b) para b ∈ C.
double ExtractorConos::calcularValorEconomico(const ConjuntoCoordenadas& cono, const ModeloBloques& modelo) noexcept
{
    double valor_total = 0.0;
    for (const Coord3D& b : cono) {
        valor_total += modelo.getValor(b.x, b.y, b.z);
    }
    return valor_total;
}

// Marca todos los bloques de un cono como extraidos en el modelo.
void ExtractorConos::marcarComoExtraidos(const ConjuntoCoordenadas& cono, ModeloBloques& modelo) noexcept
{
    for (const Coord3D& b : cono) {
        modelo.marcarExtraido(b.x, b.y, b.z);
    }
}

// Algoritmo principal de Extraccion Iterativa con politica First-Fit.
ResultadoExtraccion ExtractorConos::extraccionLineaBase(ModeloBloques& modelo) {
    double beneficio_total = 0.0;
    ConjuntoCoordenadas bloques_extraidos_totales;
    bool hubo_extraccion = true;
    RegistroEjecucion registro;
    const auto tiempo_inicio = std::chrono::high_resolution_clock::now();
    // Ciclo principal: continua mientras haya al menos una extraccion por iteracion.
    while (hubo_extraccion) {
        hubo_extraccion = false;
        ++registro.iteraciones_totales;
        // Escaneo secuencial x -> y -> z
        for (int32_t x = 0; x < modelo.max_x; ++x) {
            for (int32_t y = 0; y < modelo.max_y; ++y) {
                for (int32_t z = 0; z < modelo.max_z; ++z) {
                    // Saltar bloques ya extraidos
                    if (modelo.estaExtraido(x, y, z)) {
                        continue;
                    }
                    // Reconstruir el cono (puede retornar INEXTRAIBLE)
                    ResultadoCono cono_candidato = reconstruirConoRecursivo(x, y, z, modelo);
                    // Control de inextraibilidad
                    if (!cono_candidato.has_value()) {
                        ++registro.bloques_inextraibles;
                        continue;
                    }
                    // Calcular valor economico del cono
                    const double valor_cono = calcularValorEconomico(
                        cono_candidato.value(), modelo);
                    // Politica First-Fit: primer cono con V(C) > 0
                    if (valor_cono > 0.0) {
                        beneficio_total += valor_cono;
                        // Union sin duplicados: E <- E ∪ C
                        for (const Coord3D& b : cono_candidato.value()) {
                            bloques_extraidos_totales.insert(b);
                        }
                        // Marcar bloques del cono como extraidos: R <- R \ C
                        marcarComoExtraidos(cono_candidato.value(), modelo);
                        hubo_extraccion = true;
                        ++registro.conos_extraidos;
                        // Interrumpir escaneo y reiniciar
                        break;
                    } else if (valor_cono == 0.0) {
                        // Caso limite: cono de valor cero
                        registro.conos_nulos.push_back(RegistroConoNulo{
                            Coord3D{x, y, z},
                            cono_candidato.value().size(),
                            valor_cono
                        });
                    }
                }
                if (hubo_extraccion) {
                    break;
                }
            }
            if (hubo_extraccion) {
                break;
            }
        }
    }

    const auto tiempo_fin = std::chrono::high_resolution_clock::now();
    const double tiempo_ms = std::chrono::duration<double, std::milli>(
        tiempo_fin - tiempo_inicio).count();
    return ResultadoExtraccion{
        beneficio_total,
        std::move(bloques_extraidos_totales),
        registro,
        tiempo_ms
    };
}
