#pragma once
#include "stdafx.h"

namespace matrix_multiply_scalar {
	using FunctionSignature = void (*)(const Matrix & A, MatrixValue B, Matrix & C);

	void sequental(const Matrix& A, MatrixValue B, Matrix& C);
	void amp_impl(const Matrix& A, MatrixValue B, Matrix& C);
	void openmp(const Matrix& A, MatrixValue B, Matrix& C);

	FunctionSignature from_string(const std::string& name);
	std::string to_string(FunctionSignature name);
}