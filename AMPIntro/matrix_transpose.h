#pragma once
#include "stdafx.h"

namespace matrix_transpose {
	using FunctionSignature = void (*)(const Matrix & A, Matrix & C);

	void sequental(const Matrix& A, Matrix& C);
	void amp_impl(const Matrix& A, Matrix& C);
	void openmp(const Matrix& A, Matrix& C);

	FunctionSignature from_string(const std::string& name);
	std::string to_string(FunctionSignature name);

	template <int tileSize>
	void block(const Matrix& A, Matrix& C)
	{
		using namespace Concurrency;
		if (A.size_x == A.size_y && A.size_y == 1) {
			C.values[0] = A.values[0];
			return;
		}
		array_view<const MatrixValue, 2> a(A.size_x, A.size_y, A.values);
		array_view<MatrixValue, 2> result(A.size_y, A.size_x, C.values);
		result.discard_data();
		parallel_for_each(a.extent.tile<tileSize, tileSize>(),
			[=](tiled_index<tileSize, tileSize> tidx) restrict(amp)
			{
				tile_static MatrixValue localData[tileSize][tileSize];
				localData[tidx.local[1]][tidx.local[0]] = a[tidx.global];
				tidx.barrier.wait();
				index<2> outIdx(index<2>(tidx.tile_origin[1], tidx.tile_origin[0]) + tidx.local);
				result[outIdx] = localData[tidx.local[0]][tidx.local[1]];
			});
		result.synchronize();
	}
}