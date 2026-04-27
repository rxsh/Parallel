#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>
#include <omp.h>

using namespace std;

static vector<int> makeData(int n, unsigned int seed) {
	vector<int> a(n);
	mt19937 rng(seed);
	uniform_int_distribution<int> dist(0, n * 10);
	for (int i = 0; i < n; ++i) {
		a[i] = dist(rng);
	}
	return a;
}

static bool isSorted(const vector<int>& a) {
	for (size_t i = 1; i < a.size(); ++i) {
		if (a[i - 1] > a[i]) {
			return false;
		}
	}
	return true;
}

static void oddEvenSortParallelFor(vector<int>& a, int threadCount) {
	int n = static_cast<int>(a.size());
	int i, tmp;
	int phase;

	for (phase = 0; phase < n; phase++) {
		if (phase % 2 == 0) {
#pragma omp parallel for num_threads(threadCount) default(none) shared(a, n) private(i, tmp)
			for (i = 1; i < n; i += 2) {
				if (a[i - 1] > a[i]) {
					tmp = a[i - 1];
					a[i - 1] = a[i];
					a[i] = tmp;
				}
			}
		} else {
#pragma omp parallel for num_threads(threadCount) default(none) shared(a, n) private(i, tmp)
			for (i = 1; i < n - 1; i += 2) {
				if (a[i] > a[i + 1]) {
					tmp = a[i + 1];
					a[i + 1] = a[i];
					a[i] = tmp;
				}
			}
		}
	}
}

static void oddEvenSortSingleParallelRegion(vector<int>& a, int threadCount) {
	int n = static_cast<int>(a.size());
	int i, tmp, phase;

#pragma omp parallel num_threads(threadCount) default(none) shared(a, n) private(i, tmp, phase)
	{
		for (phase = 0; phase < n; phase++) {
			if (phase % 2 == 0) {
#pragma omp for
				for (i = 1; i < n; i += 2) {
					if (a[i - 1] > a[i]) {
						tmp = a[i - 1];
						a[i - 1] = a[i];
						a[i] = tmp;
					}
				}
			} else {
#pragma omp for
				for (i = 1; i < n - 1; i += 2) {
					if (a[i] > a[i + 1]) {
						tmp = a[i + 1];
						a[i + 1] = a[i];
						a[i] = tmp;
					}
				}
			}
		}
	}
}

struct Result {
	int n;
	int threads;
	double seconds;
};

static Result benchmarkParallelFor(int n, int threads, int trials) {
	double total = 0.0;
	vector<int> last;

	for (int t = 0; t < trials; ++t) {
		vector<int> a = makeData(n, 1234u + static_cast<unsigned int>(t));
		auto start = chrono::high_resolution_clock::now();
		oddEvenSortParallelFor(a, threads);
		auto end = chrono::high_resolution_clock::now();
		total += chrono::duration<double>(end - start).count();
		last = move(a);
	}

	if (!isSorted(last)) {
		cerr << "Error: la version parallel for no ordeno correctamente." << endl;
	}

	return {n, threads, total / trials};
}

static Result benchmarkSingleParallelRegion(int n, int threads, int trials) {
	double total = 0.0;
	vector<int> last;

	for (int t = 0; t < trials; ++t) {
		vector<int> a = makeData(n, 4321u + static_cast<unsigned int>(t));
		auto start = chrono::high_resolution_clock::now();
		oddEvenSortSingleParallelRegion(a, threads);
		auto end = chrono::high_resolution_clock::now();
		total += chrono::duration<double>(end - start).count();
		last = move(a);
	}

	if (!isSorted(last)) {
		cerr << "Error: la version single parallel region no ordeno correctamente." << endl;
	}

	return {n, threads, total / trials};
}

int main(int argc, char** argv) {
	int n = 20000;
	int trials = 3;

	if (argc > 1) n = atoi(argv[1]);
	if (argc > 2) trials = max(1, atoi(argv[2]));

	vector<int> threadCounts = {1, 2, 3, 4};

	cout << "Odd-Even Sort con OpenMP" << endl;
	cout << "n=" << n << ", trials=" << trials << endl << endl;

	cout << left << setw(30) << "Version"
		 << setw(12) << "threads"
		 << setw(14) << "time(s)" << endl;
	cout << string(56, '-') << endl;

	for (int threads : threadCounts) {
		Result r1 = benchmarkParallelFor(n, threads, trials);
		cout << left << setw(30) << "Two parallel for directives"
			 << setw(12) << r1.threads
			 << setw(14) << fixed << setprecision(6) << r1.seconds << endl;
	}

	cout << endl;

	for (int threads : threadCounts) {
		Result r2 = benchmarkSingleParallelRegion(n, threads, trials);
		cout << left << setw(30) << "Two for directives"
			 << setw(12) << r2.threads
			 << setw(14) << fixed << setprecision(6) << r2.seconds << endl;
	}

	return 0;
}
