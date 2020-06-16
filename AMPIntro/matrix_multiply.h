#pragma once
#include "stdafx.h"

namespace matrix_multiply {
	using FunctionSignature = void (*)(const Matrix& A, const Matrix& B, Matrix& C);

	void sequental(const Matrix& A, const Matrix& B, Matrix& C);
	void amp_impl(const Matrix& A, const Matrix& B, Matrix& C);
	void openmp(const Matrix& A, const Matrix& B, Matrix& C);

	FunctionSignature from_string(const std::string& name);
	std::string to_string(FunctionSignature name);
	
	template<int tileSize>
	void block_no_shared(const Matrix& A, const Matrix& B, Matrix& C) 
	{
		using namespace Concurrency;
		array_view<const MatrixValue, 2> a(A.size_x, A.size_y, A.values);
		array_view<const MatrixValue, 2> b(B.size_x, B.size_y, B.values);
		array_view<MatrixValue, 2> product(C.size_x, C.size_y, C.values);
		parallel_for_each(product.extent.tile<tileSize, tileSize>(),
			[=](tiled_index<tileSize, tileSize> t_idx) restrict(amp)
			{
				const int row = t_idx.global[0];
				const int col = t_idx.global[1];
				MatrixValue sum = 0;
				for (int i = 0; i < b.extent[0]; i++)
					sum += a(row, i) * b(i, col);
				product[t_idx.global] = sum;
			});
		product.synchronize();
	}
	
	template<int tileSize>
	void block_shared(const Matrix& A, const Matrix& B, Matrix& C) 
	{
		using namespace Concurrency;
		array_view<const MatrixValue, 2> a(A.size_x, A.size_y, A.values);
		array_view<const MatrixValue, 2> b(B.size_x, B.size_y, B.values);
		array_view<MatrixValue, 2> product(C.size_x, C.size_y, C.values);
		const int inner_size = A.size_y;
		parallel_for_each(product.extent.tile<tileSize, tileSize>(),
			[=](tiled_index<tileSize, tileSize> tidx) restrict(amp)
			{
				const int row = tidx.local[0];
				const int col = tidx.local[1];
				MatrixValue sum = 0.0;
				tile_static MatrixValue sA[tileSize][tileSize];
				tile_static MatrixValue sB[tileSize][tileSize];
				for (int i = 0; i < a.extent[1]; i += tileSize)
				{
					sA[row][col] = a(tidx.global[0], col + i);
					sB[row][col] = b(row + i, tidx.global[1]);
					tidx.barrier.wait();
					for (int k = 0; k < tileSize; k++)
						sum += sA[row][k] * sB[k][col];
					tidx.barrier.wait();
				}
				product[tidx.global] = sum;
			});
		product.synchronize();
	}
	void enlarged(const Matrix& A, const Matrix& B, Matrix& C);
}