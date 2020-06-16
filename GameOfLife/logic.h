#pragma once
#include "stdafx.h"

using PixelData = int;
struct SharedRules {
	const int rules[18] = {
		 0, 0, 0, 1, 0, 0, 0, 0, 0, // dead
		 0, 0, 1, 1, 0, 0, 0, 0, 0 // alive
	};
};

namespace logic {
	using FunctionSignature = void (*)(std::vector<PixelData>& shadow_state, int field_size, std::vector<PixelData>& shadow_state_double);
	void sequental(std::vector<PixelData>& shadow_state, int field_size, std::vector<PixelData>& shadow_state_double);
	void parallel(std::vector<PixelData>& shadow_state, int field_size, std::vector<PixelData>& shadow_state_double);
	void parallel_branchless(std::vector<PixelData>& shadow_state, int field_size, std::vector<PixelData>& shadow_state_double);
	void parallel_branchless_const(std::vector<PixelData>& shadow_state, int field_size, std::vector<PixelData>& shadow_state_double);
	template <int tile_size>
	void parallel_branchless_shared(std::vector<PixelData>& shadow_state, int field_size, std::vector<PixelData>& shadow_state_double)
	{
		using namespace concurrency;
		const auto data = &(shadow_state[0]);
		concurrency::array_view<PixelData, 2> data_out(field_size, field_size, data);
		data_out.discard_data();
		concurrency::array<PixelData, 2> new_buffer(field_size, field_size, data);
		const auto n = field_size - 2;
		const auto t = field_size - 1;
		concurrency::parallel_for_each(
			concurrency::extent<1>(field_size),
			[=, &new_buffer](index<1> idx) restrict(amp)
			{
				const auto row_idx = idx[0];
				new_buffer(0, row_idx) = new_buffer(n, row_idx);
				new_buffer(t, row_idx) = new_buffer(1, row_idx);
			});
		concurrency::parallel_for_each(
			concurrency::extent<1>(field_size),
			[=, &new_buffer](index<1> idx) restrict(amp)
			{
				const auto x = idx[0];
				new_buffer(x, 0) = new_buffer(x, n);
				new_buffer(x, t) = new_buffer(x, 1);
			});
		const int rules[18] = {
		 0, 0, 0, 1, 0, 0, 0, 0, 0, // dead
		 0, 0, 1, 1, 0, 0, 0, 0, 0 // alive
		};
		concurrency::parallel_for_each(
			concurrency::extent<1>(n).tile<tile_size>(),
			[=, &new_buffer](concurrency::tiled_index<tile_size> tidx) restrict(amp)
			{
				const auto y0 = tidx.global[0];
				const auto y1 = y0 + 1;
				const auto y2 = y1 + 1;
				for (int x = 1; x < t; x++)
				{
					const auto xl = x - 1;
					const auto xr = x + 1;
					const int sum = new_buffer(xl, y1)
						+ new_buffer(xr, y1)
						+ new_buffer(xl, y0)
						+ new_buffer(x, y0)
						+ new_buffer(xr, y0)
						+ new_buffer(xl, y2)
						+ new_buffer(x, y2)
						+ new_buffer(xr, y2);
					data_out(x, y1) = rules[new_buffer(x, y1) * 9 + sum];
				}
			});
		data_out.synchronize();
	}
};