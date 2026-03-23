package main

import (
	"fmt"
	"os"
	"strconv"
	"strings"
	"time"
)

func llenarMatrices(A []float64, B []float64, n int) {
	for i := 0; i < n; i++ {
		for j := 0; j < n; j++ {
			A[i*n+j] = float64((i+2*j)%100) / 20.0
			B[i*n+j] = float64((3*i+j)%100) / 30.0
		}
	}
}

func limpiar(C []float64) {
	for i := range C {
		C[i] = 0.0
	}
}

func multiplicacionClasica(A []float64, B []float64, C []float64, n int) float64 {
	limpiar(C)

	inicio := time.Now()
	for i := 0; i < n; i++ {
		for j := 0; j < n; j++ {
			suma := 0.0
			for k := 0; k < n; k++ {
				suma += A[i*n+k] * B[k*n+j]
			}
			C[i*n+j] = suma
		}
	}

	return time.Since(inicio).Seconds() * 1000.0
}

func minimo(a int, b int) int {
	if a < b {
		return a
	}
	return b
}

func multiplicacionBloques(A []float64, B []float64, C []float64, n int, bloque int) float64 {
	limpiar(C)

	inicio := time.Now()
	for ii := 0; ii < n; ii += bloque {
		for jj := 0; jj < n; jj += bloque {
			for kk := 0; kk < n; kk += bloque {
				finI := minimo(ii+bloque, n)
				finJ := minimo(jj+bloque, n)
				finK := minimo(kk+bloque, n)

				for i := ii; i < finI; i++ {
					for j := jj; j < finJ; j++ {
						suma := C[i*n+j]
						for k := kk; k < finK; k++ {
							suma += A[i*n+k] * B[k*n+j]
						}
						C[i*n+j] = suma
					}
				}
			}
		}
	}

	return time.Since(inicio).Seconds() * 1000.0
}

func checksum(C []float64) float64 {
	suma := 0.0
	for _, valor := range C {
		suma += valor
	}
	return suma
}

func main() {
	tamanos := []int{64, 128, 256, 384}
	bloque := 32

	if len(os.Args) > 1 {
		tamanos = nil
		for i := 1; i < len(os.Args); i++ {
			arg := os.Args[i]
			if (arg == "--block" || arg == "-b") && i+1 < len(os.Args) {
				if b, err := strconv.Atoi(os.Args[i+1]); err == nil && b > 0 {
					bloque = b
				}
				i++
				continue
			}
			if strings.HasPrefix(arg, "--block=") {
				val := strings.TrimPrefix(arg, "--block=")
				if b, err := strconv.Atoi(val); err == nil && b > 0 {
					bloque = b
				}
				continue
			}
			n, err := strconv.Atoi(arg)
			if err == nil && n > 0 {
				tamanos = append(tamanos, n)
			}
		}
	}

	fmt.Println("Comparacion de multiplicacion de matrices")
	fmt.Printf("Bloque usado en la version por bloques: %d\n", bloque)
	fmt.Printf("%-10s%-20s%-20s%-14s%s\n", "N", "clasica (ms)", "bloques (ms)", "razon", "checksum")

	for _, n := range tamanos {
		A := make([]float64, n*n)
		B := make([]float64, n*n)
		C := make([]float64, n*n)

		llenarMatrices(A, B, n)

		tiempoClasico := multiplicacionClasica(A, B, C, n)
		control := checksum(C)

		tiempoBloques := multiplicacionBloques(A, B, C, n, bloque)
		control += checksum(C)

		fmt.Printf("%-10d%-20.3f%-20.3f%-14.2f%.2f\n", n, tiempoClasico, tiempoBloques, tiempoClasico/tiempoBloques, control)
	}
}
