# Entrega 2 — Extracción Iterativa de Conos Invertidos Positivos en un Modelo de Bloques 3D

**Grupo 01** — Diseño y Análisis de Algoritmos, UCN

| Integrante | Rol en esta entrega |
|---|---|
| Maximiliano Bezares | Capítulo I, correcciones formales del Capítulo II, ejecución experimental, índices |
| Gabriel Briones | Código: línea base, DAG, lector de escenario, mediciones |
| Pablo Bravo | Capítulos III, V y VI del informe |

Comparación de una **línea base** de fuerza bruta (First-Fit, O(9^H) por cono) contra una **propuesta optimizada** basada en un DAG de precedencias con caché de métricas y dos políticas de selección (Max-Value y Razón Valor/Tamaño), evaluadas sobre instancias sintéticas y sobre el escenario oficial `escenario00.txt` provisto por el docente.

## Requisitos

- **Compilador:** g++ (MinGW-w64), estándar **C++17**.
- **Sistema operativo:** Windows — la medición de RAM (`obtenerRAM_KB()` en `Instancia.cpp`, `getPeakMemoryKB()` en `benchmark.cpp`) usa la API `<windows.h>` / `<psapi.h>` (`GetProcessMemoryInfo`), no hay implementación para Linux/macOS en esta rama de código.
- **make:** si `make` no está en el PATH, usa `mingw32-make.exe` (viene con la mayoría de las distribuciones MinGW-w64, ej. WinLibs) con la misma sintaxis, o compila directamente con los comandos `g++` de la sección siguiente.

## Estructura del proyecto

```
entrega2-daa/
├── data/
│   └── escenario00.txt
├── include/
│   ├── Bloque.h                
│   ├── GrafoDAG.h
│   ├── OperacionesCono.h       
│   ├── Extraccion.h            
│   └── Instancia.h             
├── src/
│   ├── extraccionDAG/
│   │   ├── main.cpp            
│   │   ├── GrafoDAG.cpp
│   │   ├── OperacionesCono.cpp
│   │   ├── Extraccion.cpp
│   │   └── Instancia.cpp
│   └── linea_base/             
│       ├── types.hpp           
│       ├── ModeloBloques.hpp / .cpp
│       ├── ExtractorConos.hpp / .cpp
│       ├── Registro.hpp
│       ├── GeneradorInstancias.hpp / .cpp
│       ├── Reporte.hpp / .cpp
│       ├── linea_base.cpp      
│       ├── benchmark.cpp       
│       └── Makefile
├── results/                    
├── build/                      
├── Makefile                    
└── README.md
```

## Compilación

Todos los comandos se ejecutan **desde la raíz del repositorio**, salvo que se indique lo contrario (las rutas relativas a `data/escenario00.txt` dependen de esto).

### Modo 1 — Suite comparativa completa (línea base + DAG)

```bash
make all
# si no tienes 'make' en el PATH:
mingw32-make.exe all
```

Equivalente directo con `g++` (sin Makefile):
```bash
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -Iinclude -Isrc/linea_base \
    src/extraccionDAG/main.cpp src/extraccionDAG/GrafoDAG.cpp src/extraccionDAG/OperacionesCono.cpp \
    src/extraccionDAG/Extraccion.cpp src/extraccionDAG/Instancia.cpp \
    src/linea_base/ModeloBloques.cpp src/linea_base/ExtractorConos.cpp \
    -o build/extraccion_dag.exe
```
> Nota: el `Makefile` raíz actualmente **no** agrega `-O2` (pendiente de unificar bandera de compilación en todos los binarios). Si vas a medir tiempos para el informe, decide una sola bandera y ejecuta ambos binarios (línea base y DAG) con la misma.

### Modo 2 y 3 — Benchmark y pruebas unitarias de la línea base

```bash
cd src/linea_base
make all
# o
mingw32-make.exe all
```

Equivalente directo con `g++`:
```bash
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic src/linea_base/linea_base.cpp src/linea_base/ExtractorConos.cpp src/linea_base/ModeloBloques.cpp src/linea_base/Reporte.cpp src/linea_base/GeneradorInstancias.cpp -o src/linea_base/linea_base.exe
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic src/linea_base/benchmark.cpp src/linea_base/ExtractorConos.cpp src/linea_base/ModeloBloques.cpp src/linea_base/Reporte.cpp src/linea_base/GeneradorInstancias.cpp -o src/linea_base/benchmark.exe
```

## Ejecución — modos disponibles y qué tabla del informe reproduce cada uno

### 1. Suite comparativa completa

```bash
./build/extraccion_dag.exe
# o en PowerShell:
.\build\extraccion_dag.exe
```

Corre, en este orden, con **First-Fit (línea base)**, **Max-Value (DAG)** y **Razón Valor/Tamaño (DAG)**:
1. Tres escalas sintéticas generadas por `generarInstanciaEscalable()`: Pequeña (10×10×3), Mediana (30×30×3), Grande (50×50×3) → **reproduce Tablas IV-2 a IV-5**.
2. El escenario oficial `data/escenario00.txt` (27×40×13 = 14.040 bloques, verificado) → **reproduce la Sección 4.4**.

La salida se imprime en consola **y** se guarda en `resultados_pruebas.txt` (se crea en el directorio desde el que ejecutes el binario — corre desde la raíz para que quede junto al resto del proyecto; el archivo no se versiona, está en `.gitignore`).

**Salida esperada** (una tabla por escala, formato fijo de columnas):
```
====================================================================================================
  Escenario Real 00 (27x40x13 = 14040 bloques)
====================================================================================================
  Algoritmo                          Beneficio  Bloques Ext.  Tiempo(ms)   Iteraciones      RAM (KB)
----------------------------------------------------------------------------------------------------
  First-Fit (Linea Base)             <valor>       <valor>     <valor>            —         <valor>
  Max-Value (DAG)                    <valor>       <valor>     <valor>       <valor>         <valor>
  Razon Valor/Tamano (DAG)           <valor>       <valor>     <valor>       <valor>         <valor>
----------------------------------------------------------------------------------------------------
```
Mientras corre, cada algoritmo imprime una barra de progreso en la misma línea (`\r`) que se sobrescribe hasta completarse.

> **Pendiente (no implementado en este binario):** repeticiones k=10 con media ± desviación estándar, e instancia de divergencia dedicada para la Tabla IV-6. Ambos están en el plan de trabajo del grupo pero no están integrados todavía.

### 2. Benchmark de escalabilidad de la línea base → Tabla II-1

```bash
cd src/linea_base
./benchmark.exe
```

Corre **solo First-Fit** sobre las mismas tres escalas sintéticas (10×10×3 / 30×30×3 / 50×50×3, generadas localmente por `crearInstanciaEscalable()` dentro de `benchmark.cpp`) y reporta Peak Working Set vía `GetProcessMemoryInfo`.

**Reproduce la Tabla II-1** — campaña de diagnóstico preliminar, ejecutada antes del diseño del DAG. **No es directamente comparable en tiempos/RAM con las Tablas IV-2 a IV-5**, porque usa una función de medición de RAM distinta (Peak Working Set) a la unificada que usa el binario de la Sección 1 (`obtenerRAM_KB()`, RSS actual).

### 3. Pruebas unitarias de la línea base → verificación de correctitud

```bash
cd src/linea_base
./linea_base.exe
```

Corre 4 instancias con valor esperado conocido y compara automáticamente:
- **Minimal (5×5×3)** — beneficio esperado: `66.0`
- **Cono Nulo (3×3×2)** — beneficio esperado: `0.0`, 1 cono nulo detectado
- **Mediana (5×5×4)** — informativa (sin assert numérico)
- **Columna única (1×1×3)** — beneficio esperado: `5.0`

**Salida esperada** por cada caso: el reporte completo (`imprimirReporte`, con secciones `[MODELO]`, `[METRICAS DE RENDIMIENTO]`, `[RESULTADO ECONOMICO]`, `[BLOQUES EXTRAIDOS]`, `[REPORTE — CONOS CON VALOR CERO]`) seguido de una línea `[VERIFICACION] ... ✓ CORRECTO` o `✗ INCORRECTO`.

## Umbral de timeout

Todos los algoritmos (línea base y DAG) comparten el mismo umbral **hardcodeado: `1800000.0` ms = 30 minutos** (declarado de forma independiente en `ExtractorConos.cpp`, `Extraccion.cpp` y `OperacionesCono.cpp` — no es un parámetro configurable por línea de comandos).

- **En el Modo 1** (`extraccion_dag.exe`): cada uno de los tres algoritmos corre dentro de su propio `try/catch`. Si un algoritmo supera el umbral, lanza `std::runtime_error("Timeout")`, se captura, y esa fila de la tabla se imprime como `TIMEOUT` en Beneficio/Bloques Ext./Tiempo — **sin detener** la ejecución de los otros dos algoritmos de esa escala.
- **En los Modos 2 y 3** (`benchmark.exe`, `linea_base.exe`): **no hay try/catch** alrededor de la llamada al algoritmo. Si se alcanzara el timeout, el programa termina con una excepción no capturada. En la práctica es muy improbable en estos dos modos porque solo corren sobre instancias sintéticas pequeñas (≤ 7.500 bloques) o de prueba unitaria (≤ 100 bloques).

Una fila `TIMEOUT` en la tabla del Modo 1 significa: el algoritmo no llegó a una condición de término natural (ya no quedan conos con `V(C) > EPSILON`) dentro de los 30 minutos.
