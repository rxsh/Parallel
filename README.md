# Trabajo: Bucles Anidados y Multiplicacion de Matrices

Archivos principales:

- `2loops.cpp`: comparacion en C++ de los dos ordenes de recorrido (`i-j` y `j-i`).
- `2loops.go`: comparacion en Go de los dos ordenes de recorrido (`i-j` y `j-i`).
- `matmul.cpp`: comparacion en C++ entre multiplicacion clasica y multiplicacion por bloques.
- `matmul.go`: comparacion en Go entre multiplicacion clasica y multiplicacion por bloques.
- `informe.tex`: base del informe en formato articulo LaTeX.

## Compilacion en C++

```bash
g++ -O2 -std=c++17 2loops.cpp -o 2loops_cpp
g++ -O2 -std=c++17 matmul.cpp -o matmul_cpp
```

## Ejecucion en C++

```bash
./2loops_cpp
./2loops_cpp 256 512 1024

./matmul_cpp
./matmul_cpp 64 128 256 384
```

## Compilacion en Go

```bash
go build -o 2loops_go 2loops.go
go build -o matmul_go matmul.go
```

## Ejecucion en Go

```bash
./2loops_go
./2loops_go 256 512 1024

./matmul_go
./matmul_go 64 128 256 384
```

## Medicion con Valgrind y KCachegrind

Estas herramientas normalmente se usan en Linux o en WSL.

```bash
valgrind --tool=cachegrind --cache-sim=yes ./2loops_cpp 512
valgrind --tool=cachegrind --cache-sim=yes ./matmul_cpp 128
cg_annotate cachegrind.out.PID
```

Para visualizar mejor los resultados:

```bash
kcachegrind cachegrind.out.PID
```

## Nota

La version por bloques usa un tamano de bloque de `32`. Si quieres experimentar, puedes cambiar ese valor directamente en `matmul.cpp` y `matmul.go`.
