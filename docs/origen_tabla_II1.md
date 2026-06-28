# Origen de resultados Tabla II-1

Los resultados de la Tabla II-1 (Seccion 2.1.1 del informe) fueron obtenidos
ejecutando la linea base original ubicada en la carpeta linea_base/.

## Como ejecutar
cd linea_base
make
./benchmark

## Instancia utilizada
crearInstanciaEscalable: deterministico, sin semilla RNG.
Esteril = -1.0, bloques ricos en z=0 con valor 200+(x*7+y*3)%50
dentro del margen = Z-1 bloques por lado.

## Escalas probadas
- Pequena: 10x10x3 (300 bloques)
- Mediana: 30x30x3 (2700 bloques)  
- Grande:  50x50x3 (7500 bloques)
