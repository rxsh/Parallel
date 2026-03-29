package main

import (
	"fmt"
	"os"
	"strconv"
	"time"
)

func llenarDatos(A []float64, x []float64, n int) {
	for i := 0; i < n; i++ {
		x[i] = 1.0 + float64(i%7)
		for j := 0; j < n; j++ {
			A[i*n+j] = float64((i+j)%100) / 25.0
		}
	}
}

func ejecutarOrdenIJ(A []float64, x []float64, y []float64, n int) float64 {
	for i := range y {
		y[i] = 0.0
	}

	inicio := time.Now()
	for i := 0; i < n; i++ {
		for j := 0; j < n; j++ {
			y[i] += A[i*n+j] * x[j]
		}
	}

	return time.Since(inicio).Seconds() * 1000.0
}

func ejecutarOrdenJI(A []float64, x []float64, y []float64, n int) float64 {
	for i := range y {
		y[i] = 0.0
	}

	inicio := time.Now()
	for j := 0; j < n; j++ {
		for i := 0; i < n; i++ {
			y[i] += A[i*n+j] * x[j]
		}
	}

	return time.Since(inicio).Seconds() * 1000.0
}

func checksum(y []float64) float64 {
	suma := 0.0
	for _, valor := range y {
		suma += valor
	}
	return suma
}

func main() {
	tamanos := []int{256, 512, 1024}

	if len(os.Args) > 1 {
		tamanos = nil
		for _, arg := range os.Args[1:] {
			n, err := strconv.Atoi(arg)
			if err == nil && n > 0 {
				tamanos = append(tamanos, n)
			}
		}
	}

	fmt.Println("Comparacion de dos bucles anidados para y = A*x")
	fmt.Printf("%-10s%-18s%-18s%-14s%s\n", "N", "orden i-j (ms)", "orden j-i (ms)", "razon", "checksum")

	for _, n := range tamanos {
		A := make([]float64, n*n)
		x := make([]float64, n)
		y := make([]float64, n)

		llenarDatos(A, x, n)

		tiempoIJ := ejecutarOrdenIJ(A, x, y, n)
		control := checksum(y)

		tiempoJI := ejecutarOrdenJI(A, x, y, n)
		control += checksum(y)

		fmt.Printf("%-10d%-18.3f%-18.3f%-14.2f%.2f\n", n, tiempoIJ, tiempoJI, tiempoJI/tiempoIJ, control)
	}
}
