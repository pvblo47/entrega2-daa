# Entrega 2 - Extracción Optimizada DAG
**Grupo 01** | Bezares / Briones / Bravo  
Diseño y Análisis de Algoritmos - UCN

## Estructura del proyecto
```
entrega2/
├── include/
│   ├── Bloque.h          # Nodo del grafo: struct Bloque + enum EstatusBorde
│   ├── GrafoDAG.h        # Grafo DAG bidireccional de precedencias
│   ├── OperacionesCono.h # BFS cierre de cono + actualización dinámica
│   ├── Extraccion.h      # Algoritmo principal + enum TipoPolitica
│   └── Instancia.h       # Generador de instancias + utilidades de reporte
├── src/
│   ├── GrafoDAG.cpp
│   ├── OperacionesCono.cpp
│   ├── Extraccion.cpp
│   ├── Instancia.cpp
│   └── main.cpp
├── Makefile
└── README.md
```

## Compilación y ejecución
```bash
make        # compila con g++ C++17 -O2
make run    # compila y ejecuta
make clean  # limpia artefactos
```

## Correspondencia pseudocódigo ↔ implementación
| Pseudocódigo (Sección)          | Archivo implementación             |
|---------------------------------|------------------------------------|
| Construir_DAG_Precedencias 2.4.2 | `src/GrafoDAG.cpp`                |
| Calcular_Cierre_Cono 2.4.3      | `src/OperacionesCono.cpp`         |
| Actualizar_Descendientes 2.4.4  | `src/OperacionesCono.cpp`         |
| Extraccion_Optimizada_DAG 2.4.1 | `src/Extraccion.cpp`              |
