#include "matrix_transpose.h"

using namespace matrix_transpose;

void matrix_transpose::sequental(const Matrix& A, Matrix& C) {
	const auto pA = A.values;
	const auto pC = C.values;
	for (int y = 0; y < A.size_y; y++) {
		for (int x = 0; x < A.size_x; x++) {
			const auto idx = y * A.size_x + x;
			const auto transposed_idx = x * A.size_y + y;
			pC[idx] = pA[transposed_idx];
		}
	}
}

void matrix_transpose::amp_impl(const Matrix& A, Matrix& C) {
	using namespace Concurrency;
	array_view<const MatrixValue, 2> a(A.size_x, A.size_y, A.values);
	array_view<MatrixValue, 2> result(A.size_y, A.size_x, C.values);
	result.discard_data();
	parallel_for_each(result.extent,
		[=](index<2> idx) restrict(amp) { result(idx[0], idx[1]) = a(idx[1], idx[0]); });
	result.synchronize();
}

void matrix_transpose::openmp(const Matrix& A, Matrix& C) {
	const auto pA = A.values;
	const auto pC = C.values;
#pragma omp parallel for
	for (int y = 0; y < A.size_y; y++) {
		for (int x = 0; x < A.size_x; x++) {
			const auto idx = y * A.size_x + x;
			const auto transposed_idx = x * A.size_y + y;
			pC[idx] = pA[transposed_idx];
		}
	}
}

FunctionSignature matrix_transpose::from_string(const std::string& name) {
	if (name == seqentalTitle) return &sequental;
	if (name == ampTitle) return &amp_impl;
	else if (name == openmpTitle) return &openmp;
	else return nullptr;
}

std::string matrix_transpose::to_string(FunctionSignature name) {
	if (name == &sequental) return seqentalTitle;
	else if (name == &amp_impl) return ampTitle;
	else if (name == &openmp) return openmpTitle;
	else return unknownTitle;
}
