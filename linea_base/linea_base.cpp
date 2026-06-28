#include <iostream>
#include <cmath>
#include "ModeloBloques.hpp"
#include "ExtractorConos.hpp"
#include "GeneradorInstancias.hpp"
#include "Reporte.hpp"

int main() {
    std::cout << '\n';
    imprimirSeparador('*');
    std::cout << "  LINEA BASE C++: Extraccion Iterativa de Conos Invertidos\n";
    std::cout << "  Politica: First-Fit | Estandar: C++17\n";
    imprimirSeparador('*');
    std::cout << '\n';
    // PRUEBA 1: Instancia Minimal 5×5×3
    {
        std::cout << ">>> Ejecutando Prueba 1: Instancia Minimal (5x5x3)...\n\n";
        ModeloBloques modelo = crearInstanciaMinimal();
        ExtractorConos extractor;
        ResultadoExtraccion resultado = extractor.extraccionLineaBase(modelo);
        imprimirReporte(resultado, "Minimal 5x5x3", 5, 5, 3);
        const double beneficio_esperado_p1 = 66.0;
        std::cout << "  [VERIFICACION P1] Beneficio esperado: " << beneficio_esperado_p1 << " | Obtenido: " << resultado.beneficio_total;
        if (std::abs(resultado.beneficio_total - beneficio_esperado_p1) < 1e-9) {
            std::cout << " ✓ CORRECTO\n\n";
        } else {
            std::cout << " ✗ INCORRECTO (revisar logica del cono)\n\n";
        }
    }
    // PRUEBA 2: Instancia con Cono de Valor Exactamente Cero
    {
        std::cout << ">>> Ejecutando Prueba 2: Instancia Cono Nulo (3x3x2)...\n\n";
        ModeloBloques modelo = crearInstanciaConoNulo();
        ExtractorConos extractor;
        ResultadoExtraccion resultado = extractor.extraccionLineaBase(modelo);
        imprimirReporte(resultado, "Cono Nulo 3x3x2", 3, 3, 2);
        std::cout << "  [VERIFICACION P2] Beneficio esperado: 0.0 | Obtenido: " << resultado.beneficio_total;
        if (std::abs(resultado.beneficio_total - 0.0) < 1e-9) {
            std::cout << " ✓ CORRECTO (ningun cono positivo)\n";
        } else {
            std::cout << " ✗ INCORRECTO\n";
        }
        std::cout << "  [VERIFICACION P2] Conos nulos esperados: 1 | Detectados: "
                  << resultado.registro.conos_nulos.size();
        if (resultado.registro.conos_nulos.size() >= 1) {
            std::cout << " ✓ CORRECTO\n\n";
        } else {
            std::cout << " ✗ INCORRECTO\n\n";
        }
    }

    // PRUEBA 3: Instancia Mediana 5×5×4
    {
        std::cout << ">>> Ejecutando Prueba 3: Instancia Mediana (5x5x4)...\n\n";
        ModeloBloques modelo = crearInstanciaMediana();
        ExtractorConos extractor;
        ResultadoExtraccion resultado = extractor.extraccionLineaBase(modelo);
        imprimirReporte(resultado, "Mediana 5x5x4", 5, 5, 4);
        std::cout << "  [INFO P3] Politica First-Fit extrae en orden secuencial.\n";
        std::cout << "  [INFO P3] Bloques de borde en z=0 son INEXTRAIBLES por Regla 2.3.3.\n\n";
    }

    // PRUEBA 4: Instancia de borde — modelo 1x1x3 (columna unica)
    {
        std::cout << ">>> Ejecutando Prueba 4: Columna Unica (1x1x3)...\n\n";
        ModeloBloques modelo(1, 1, 3);
        modelo.setValor(0, 0, 0, 10.0);
        modelo.setValor(0, 0, 1, -1.0);
        modelo.setValor(0, 0, 2,  5.0);
        ExtractorConos extractor;
        ResultadoExtraccion resultado = extractor.extraccionLineaBase(modelo);
        imprimirReporte(resultado, "Columna Unica 1x1x3", 1, 1, 3);
        std::cout << "  [VERIFICACION P4] Solo (0,0,2) es extraible (superficie).\n";
        std::cout << "  [INFO P4] Beneficio esperado: 5.0 | Obtenido: "
                  << resultado.beneficio_total;
        if (std::abs(resultado.beneficio_total - 5.0) < 1e-9) {
            std::cout << " ✓ CORRECTO\n\n";
        } else {
            std::cout << " ✗ INCORRECTO\n\n";
        }
    }
    imprimirSeparador('*');
    std::cout << "  Todas las pruebas completadas.\n";
    imprimirSeparador('*');
    std::cout << '\n';
    return 0;
}
