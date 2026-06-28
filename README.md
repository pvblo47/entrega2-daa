# Entrega 2 - Extracción Optimizada DAG
**Grupo 01** | Bezares / Briones / Bravo  
Diseño y Análisis de Algoritmos - UCN

## Estructura del proyecto
```
entrega2-daa/
├── include/
│   ├── Bloque.h           # Nodo del grafo: struct Bloque + enum EstatusBorde
│   ├── GrafoDAG.h         # Grafo DAG bidireccional de precedencias
│   ├── OperacionesCono.h  # BFS cierre de cono + actualización dinámica
│   ├── Extraccion.h       # Algoritmo principal + enum TipoPolitica
│   └── Instancia.h        # Generador de instancias + utilidades de reporte
├── src/
│   ├── GrafoDAG.cpp
│   ├── OperacionesCono.cpp
│   ├── Extraccion.cpp
│   ├── Instancia.cpp
│   ├── linea_base.h       # Interfaz algoritmo First-Fit
│   ├── linea_base.cpp     # Implementación algoritmo First-Fit
│   └── main.cpp           # Suite de pruebas comparativa
├── linea_base/            # Código original de la línea base (Tabla II-1)
├── docs/
│   └── origen_tabla_II1.md 
├── Makefile
└── README.md
```

## Compilación y ejecución
```bash (MINGW64)
g++ -std=c++17 -o suite.exe src/main.cpp src/GrafoDAG.cpp src/OperacionesCono.cpp src/Extraccion.cpp src/Instancia.cpp src/linea_base.cpp -Iinclude
./suite.exe

```
