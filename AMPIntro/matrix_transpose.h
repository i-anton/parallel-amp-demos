#pragma once
#include "stdafx.h"

namespace matrix_transpose {
	using FunctionSignature = void (*)(const Matrix & A, Matrix & C);

	void sequental(const Matrix& A, Matrix& C);
	void amp_impl(const Matrix& A, Matrix& C);
	void openmp(const Matrix& A, Matrix& C);

	FunctionSignature from_string(const std::string& name);
	std::string to_string(FunctionSignature name);
}