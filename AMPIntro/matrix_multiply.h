#pragma once
#include "stdafx.h"

namespace matrix_multiply {
	using FunctionSignature = void (*)(const Matrix& A, const Matrix& B, Matrix& C);

	void sequental(const Matrix& A, const Matrix& B, Matrix& C);
	void amp_impl(const Matrix& A, const Matrix& B, Matrix& C);
	void openmp(const Matrix& A, const Matrix& B, Matrix& C);

	FunctionSignature from_string(const std::string& name);
	std::string to_string(FunctionSignature name);
	
	void block_no_shared(const Matrix& A, const Matrix& B, Matrix& C);
	void block_shared(const Matrix& A, const Matrix& B, Matrix& C);
	void enlarged(const Matrix& A, const Matrix& B, Matrix& C);
}