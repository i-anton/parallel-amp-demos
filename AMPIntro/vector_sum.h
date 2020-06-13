#pragma once
#include "stdafx.h"

namespace vector_sum {
	using FunctionSignature = void (*)(int n, const MatrixValue* const pA, const MatrixValue* const pB, MatrixValue* const pC);
	
	void sequental(int n, const MatrixValue* const pA, const MatrixValue* const pB, MatrixValue* const pC);
	void amp_impl(int n, const MatrixValue* const pA, const MatrixValue* const pB, MatrixValue* const pC);
	void openmp(int n, const MatrixValue* const pA, const MatrixValue* const pB, MatrixValue* const pC);

	FunctionSignature from_string(const std::string& name);
	std::string to_string(FunctionSignature name);
}