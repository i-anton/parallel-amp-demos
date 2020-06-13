﻿#include "stdafx.h"
#include "timer.h"
#include "vector_sum.h"	
#include "matrix_multiply_scalar.h"
#include "matrix_transpose.h"
#include "matrix_multiply.h"

using namespace std;
using namespace concurrency;

void get_accelerators_info() {
	auto accelerators = accelerator::get_all();
	for (const auto& accel : accelerators) {
		wcout << accel.get_description() << endl
			<< "\tis_debug: " << accel.get_is_debug() << endl
			<< "\tmemory: " << accel.get_dedicated_memory() << endl
			<< "\thas_display: " << accel.get_has_display() << endl
			<< "\tis_emulated: " << accel.get_is_emulated() << endl
			<< "\tversion: " << accel.get_version() << endl
			<< "\tsupports_cpu_shared: " << accel.get_supports_cpu_shared_memory() << endl
			<< "\tsupports_double: " << accel.get_supports_double_precision() << endl
			<< "\tsupports_limited_double: " << accel.get_supports_limited_double_precision() << endl
			<< endl;
	}
}

double vector_sum_perf(const string& algorithm_type, int size, int runs) {
	using namespace vector_sum;
	if (size <= 0) exit(1);
	auto algo_ptr = from_string(algorithm_type);
	if (algo_ptr == nullptr) exit(1);

	unique_ptr<MatrixValue[]> A(new MatrixValue[size]);
	unique_ptr<MatrixValue[]> B(new MatrixValue[size]);
	unique_ptr<MatrixValue[]> C(new MatrixValue[size]);
	
	double acc_time = 0.0;

	for (size_t i = 0; i < runs; i++) {
		auto a_ptr = A.get();
		auto b_ptr = B.get();
		auto c_ptr = C.get();
		Timer t;
		t.Start();
		algo_ptr(size, a_ptr, b_ptr, c_ptr);
		t.Stop();
		acc_time += t.Elapsed();
	}
	return acc_time / static_cast<double>(runs);
}

double matrix_multiply_scalar_perf(const string& algorithm_type, int size, int runs) {
	using namespace matrix_multiply_scalar;
	if (size <= 0) exit(1);
	auto algo_ptr = from_string(algorithm_type);
	if (algo_ptr == nullptr) exit(1);

	size_t size_scaled = static_cast<size_t>(size);
	unique_ptr<MatrixValue[]> A(new MatrixValue[size_scaled * size_scaled]);
	unique_ptr<MatrixValue[]> C(new MatrixValue[size_scaled * size_scaled]);
	Matrix first{ size, size, A.get() };
	Matrix result{ size, size, C.get() };
	
	double acc_time = 0.0;
	for (size_t i = 0; i < runs; i++) {
		Timer t;
		t.Start();
		algo_ptr(first, 5.0, result);
		t.Stop();
		acc_time += t.Elapsed();
	}
	return acc_time / static_cast<double>(runs);
}

double matrix_transpose_perf(const string& algorithm_type, int size, int runs) {
	using namespace matrix_transpose;
	if (size <= 0) exit(1);
	auto algo_ptr = from_string(algorithm_type);
	if (algo_ptr == nullptr) exit(1);

	size_t size_scaled = static_cast<size_t>(size);
	unique_ptr<MatrixValue[]> A(new MatrixValue[size_scaled * size_scaled]);
	unique_ptr<MatrixValue[]> C(new MatrixValue[size_scaled * size_scaled]);
	Matrix first{ size, size, A.get() };
	Matrix result{ size, size, C.get() };

	double acc_time = 0.0;
	for (size_t i = 0; i < runs; i++) {
		Timer t;
		t.Start();
		algo_ptr(first, result);
		t.Stop();
		acc_time += t.Elapsed();
	}
	return acc_time / static_cast<double>(runs);
}

double matrix_multiply_perf(const string& algorithm_type, int size, int runs) {
	using namespace matrix_multiply;
	if (size <= 0) exit(1);
	auto algo_ptr = from_string(algorithm_type);
	if (algo_ptr == nullptr) exit(1);

	size_t size_scaled = static_cast<size_t>(size);
	unique_ptr<MatrixValue[]> A(new MatrixValue[size_scaled * size_scaled]);
	unique_ptr<MatrixValue[]> B(new MatrixValue[size_scaled * size_scaled]);
	unique_ptr<MatrixValue[]> C(new MatrixValue[size_scaled * size_scaled]);
	Matrix first{ size, size, A.get() };
	Matrix second{ size, size, B.get() };
	Matrix result{ size, size, C.get() };

	double acc_time = 0.0;
	for (size_t i = 0; i < runs; i++) {
		Timer t;
		t.Start();
		algo_ptr(first, second, result);
		t.Stop();
		acc_time += t.Elapsed();
	}
	return acc_time / static_cast<double>(runs);
}

bool pick_accelerator(size_t index) {
	const auto accs = accelerator::get_all();
	accelerator chosen_one = accs[index];
	std::wcout << chosen_one.description << std::endl;
	bool success = accelerator::set_default(chosen_one.device_path);
	return success;
}

const int runs = 3;
const std::vector<int> sizes{ 10, 1000, 1500, 2000 };

int main(int argc, char* argv[]) {
	cout << "omp" << endl;
	for (const auto size : sizes) {
		wcout << matrix_multiply_perf(openmpTitle, size, runs) << endl;
	}
	pick_accelerator(0);
	for (const auto size : sizes) {
		wcout << matrix_multiply_perf(ampTitle, size, runs) << endl;
	}
	pick_accelerator(1);
	for (const auto size : sizes) {
		wcout << matrix_multiply_perf(ampTitle, size, runs) << endl;
	}
	cout << "seq" << endl;
	for (const auto size : sizes) {
		wcout << matrix_multiply_perf(seqentalTitle, size, runs) << endl;
	}

	return 0;
}