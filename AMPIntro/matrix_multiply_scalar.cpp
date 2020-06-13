#include "matrix_multiply_scalar.h"

using namespace matrix_multiply_scalar;

void matrix_multiply_scalar::sequental(const Matrix& A, MatrixValue B, Matrix& C) {
	auto pA = A.values;
	auto pC = C.values;
	const auto size = A.size_x * A.size_y;
	for (size_t idx = 0; idx < size; idx++)
		pC[idx] = pA[idx] * B;
}

void matrix_multiply_scalar::amp_impl(const Matrix& A, MatrixValue B, Matrix& C) {
	using namespace Concurrency;
	array_view<const MatrixValue, 2> a(A.size_x, A.size_y, A.values);
	array_view<MatrixValue, 2> result(A.size_x, A.size_y, C.values);
	result.discard_data();
	parallel_for_each(result.extent,
		[=](index<2> idx) restrict(amp) { result[idx] = a[idx] * B; });
	result.synchronize();
}

void matrix_multiply_scalar::openmp(const Matrix& A, MatrixValue B, Matrix& C) {
	auto pA = A.values;
	auto pC = C.values;
	const auto size = A.size_x * A.size_y;
#pragma omp parallel for
	for (int idx = 0; idx < size; idx++)
		pC[idx] = pA[idx] * B;
}

FunctionSignature matrix_multiply_scalar::from_string(const std::string& name) {
	if (name == seqentalTitle)
		return &sequental;
	if (name == ampTitle)
		return &amp_impl;
	else if (name == openmpTitle)
		return &openmp;
	else return nullptr;
}

std::string matrix_multiply_scalar::to_string(FunctionSignature name) {
	if (name == &sequental) return seqentalTitle;
	else if (name == &amp_impl) return ampTitle;
	else if (name == &openmp) return openmpTitle;
	else return unknownTitle;
}
