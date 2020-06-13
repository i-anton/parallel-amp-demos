#include "vector_sum.h"

using namespace vector_sum;

void vector_sum::sequental(int n, const MatrixValue* const pA, const MatrixValue* const pB, MatrixValue* const pC) {
	for (size_t idx = 0; idx < n; idx++)
		pC[idx] = pA[idx] + pB[idx];
}

void vector_sum::amp_impl(int n, const MatrixValue* const pA, const MatrixValue* const pB, MatrixValue* const pC) {
	using namespace Concurrency;
	array_view<const MatrixValue, 1> a(n, pA);
	array_view<const MatrixValue, 1> b(n, pB);
	array_view<MatrixValue, 1> sum(n, pC);
	sum.discard_data();
	parallel_for_each(sum.extent,
		[=](index<1> idx) restrict(amp) { sum[idx] = a[idx] + b[idx]; });
	sum.synchronize();
}

void vector_sum::openmp(int n, const MatrixValue* const pA, const MatrixValue* const pB, MatrixValue* const pC) {
#pragma omp parallel for
	for (int idx = 0; idx < n; idx++)
		pC[idx] = pA[idx] + pB[idx];
}

FunctionSignature vector_sum::from_string(const std::string& name) {
	if (name == seqentalTitle) return &sequental;
	if (name == ampTitle) return &amp_impl;
	else if (name == openmpTitle) return &openmp;
	else return nullptr;
}

std::string vector_sum::to_string(FunctionSignature name) {
	if (name == &sequental) return seqentalTitle;
	else if (name == &amp_impl) return ampTitle;
	else if (name == &openmp) return openmpTitle;
	else return unknownTitle;
}
