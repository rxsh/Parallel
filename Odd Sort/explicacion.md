# Odd-Even Sort con OpenMP (Cap. 5 - Peter Pacheco)

## 1) ¿Qué es Odd-Even Sort?

Es un algoritmo de ordenamiento por fases.

- Se ejecutan `n` fases (siendo `n` el tamaño del arreglo).
- En cada fase se comparan e intercambian pares vecinos.

### Fase par
Se comparan los pares:

- `(0,1)`, `(2,3)`, `(4,5)`, ...

### Fase impar
Se comparan los pares:

- `(1,2)`, `(3,4)`, `(5,6)`, ...

Después de suficientes fases, el arreglo queda ordenado.

---

## 2) Implementaciones del libro

El libro muestra dos versiones OpenMP:

## Program 5.4: `parallel for` en cada fase

- En cada fase se usa `#pragma omp parallel for`.
- Es simple y directa.
- Pero paga overhead repetido (entrar/salir de región paralela en cada fase).

## Program 5.5: una sola región `parallel`

- Se crea una sola región con `#pragma omp parallel`.
- Dentro de cada fase se reparte con `#pragma omp for`.
- Reduce overhead y normalmente mejora tiempos.

---

## 3) ¿Por qué la versión 5.5 suele ser más rápida?

Porque evita crear y cerrar regiones paralelas muchas veces.

En resumen:

1. Menor overhead de administración de hilos.
2. Reutilización del mismo equipo de hilos.
3. Menor costo de sincronización global acumulada.

Por eso en pruebas suele ganar la 5.5 frente a la 5.4.

---

## 4) Significado de los pragmas OpenMP usados

- `#pragma omp parallel`: crea región paralela.
- `#pragma omp for`: reparte iteraciones del `for` dentro de la región.
- `#pragma omp parallel for`: combinación de ambas en una línea.
- `num_threads(threadCount)`: cantidad de hilos a usar.
- `default(none)`: obliga a declarar explícitamente alcance de variables.
- `shared(a, n)`: compartidas por todos los hilos.
- `private(i, tmp, phase)`: cada hilo tiene su propia copia.

---

## 5) ¿Qué es `trials`?

`trials` es el número de repeticiones del experimento.

Si `trials = 5`, se ejecuta 5 veces y se promedia:

$$
t_{promedio} = \frac{t_1 + t_2 + t_3 + t_4 + t_5}{5}
$$

Esto reduce ruido en mediciones.

---

## 6) ¿Por qué usar `last = move(a)` en benchmark?

`a` es un vector grande. Con `move` se transfiere su contenido sin copia pesada.

- `last = a` -> copia completa (más costosa).
- `last = move(a)` -> movimiento (más eficiente).

Se usa para luego validar con `isSorted(last)` al final de cada repetición.

---

## 7) Argumentos por línea de comandos (`argc`, `argv`)

Sí, el programa acepta argumentos:

- `argv[1]` -> tamaño `n`
- `argv[2]` -> repeticiones `trials`

Ejemplo:

- `odd-evenSort.exe 30000 5`

Significa:

- `n = 30000`
- `trials = 5`

---

## 8) Frase corta para explicar en clase

"Ambas implementaciones ordenan correctamente; la versión 5.5 suele ser más rápida porque mantiene una sola región paralela y reduce el overhead de creación/sincronización de hilos en cada fase."

