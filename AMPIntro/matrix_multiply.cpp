#include "matrix_multiply.h"

using namespace matrix_multiply;

void matrix_multiply::sequental(const Matrix& A, const Matrix& B, Matrix& C) {
	const auto pA = A.values;
	const auto pB = B.values;
	const auto pC = C.values;
	const auto a_x = A.size_x;
	const auto inner_size = A.size_y;
	const auto b_x = B.size_x;
	const auto size_y = B.size_y;
	for (int row = 0; row < a_x; row++) {
		for (int col = 0; col < size_y; col++) {
			MatrixValue acc = 0.0;
			for (int inner = 0; inner < inner_size; inner++) {
				acc += pA[row + inner * a_x] * pB[inner + col * b_x];
			}
			pC[row + col * a_x] = acc;
		}
	}
}

void matrix_multiply::amp_impl(const Matrix& A, const Matrix& B, Matrix& C) {
	using namespace Concurrency;
	array_view<const MatrixValue, 2> a(A.size_x, A.size_y, A.values);
	array_view<const MatrixValue, 2> b(B.size_x, B.size_y, B.values);
	array_view<MatrixValue, 2> product(C.size_x, C.size_y, C.values);
	const int inner_size = A.size_y;
	parallel_for_each(product.extent,
		[=](index<2> idx) restrict(amp) {
			MatrixValue acc = 0.0;
			for (int inner = 0; inner < inner_size; inner++) {
				acc += a(idx[0], inner) * b(inner, idx[1]);
			}
			product[idx] = acc;
		});

	product.synchronize();
}

void matrix_multiply::openmp(const Matrix& A, const Matrix& B, Matrix& C) {
	const auto pA = A.values;
	const auto pB = B.values;
	const auto pC = C.values;
	const auto a_x = A.size_x;
	const auto inner_size = A.size_y;
	const auto b_x = B.size_x;
	const auto size_y = B.size_y;
#pragma omp parallel for
	for (int row = 0; row < a_x; row++) {
		for (int col = 0; col < size_y; col++) {
			MatrixValue acc = 0.0;
			for (int inner = 0; inner < inner_size; inner++) {
				acc += pA[row + inner * a_x] * pB[inner + col * b_x];
			}
			pC[row + col * a_x] = acc;
		}
	}
}

FunctionSignature matrix_multiply::from_string(const std::string& name) {
	if (name == seqentalTitle) return &sequental;
	else if (name == ampTitle) return &amp_impl;
	else if (name == openmpTitle) return &openmp;
	else if (name == "block_no_shared") return &block_no_shared<32>;
	else if (name == "block_shared") return &block_shared<32>;
	else if (name == "enlarged") return &enlarged;
	else return nullptr;
}

std::string matrix_multiply::to_string(FunctionSignature name) {
	if (name == &sequental) return seqentalTitle;
	else if (name == &amp_impl) return ampTitle;
	else if (name == &openmp) return openmpTitle;
	else if (name == &block_no_shared<32>) return "block_no_shared";
	else if (name == &block_shared<32>) return "block_shared";
	else if (name == &enlarged) return "enlarged";
	else return unknownTitle;
}

void matrix_multiply::enlarged(const Matrix& A, const Matrix& B, Matrix& C)
{
	using namespace Concurrency;
	array_view<const MatrixValue, 2> a(A.size_x, A.size_y, A.values);
	array_view<const MatrixValue, 2> b(B.size_x, B.size_y, B.values);
	array_view<MatrixValue, 2> product(C.size_x, C.size_y, C.values);
	const auto a_x = A.size_x;
	const auto inner_size = A.size_y;
	const auto size_y = C.size_y;
	extent<1> e(a_x);
	parallel_for_each(e, [=](index<1>idx) restrict(amp)
		{
			for (int col = 0; col < size_y; col++)
			{
				MatrixValue sum = 0;
				for (int j = 0; j < inner_size; j++)
					sum += a(idx[0], j) * b(j, col);
				product(idx[0], col) = sum;
			}
		});
	product.synchronize();
}
