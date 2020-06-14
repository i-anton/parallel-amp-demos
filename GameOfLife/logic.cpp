#include "logic.h"
#include <cassert>

// field_size is shadow_row_size
void logic::sequental(std::vector<PixelData>& shadow_state, int field_size, std::vector<PixelData>& shadow_state_double)
{
	const auto data = &(shadow_state[0]);
	const auto new_data = &(shadow_state_double[0]);
	const auto size = shadow_state.size();
	for (size_t i = 0; i < size; i++)
		new_data[i] = data[i];

	const auto n = field_size - 2;
	for (size_t y = 0; y < field_size; y++)
	{
		const auto row_idx = y * field_size;
		new_data[row_idx] = new_data[row_idx + n];
		new_data[row_idx + field_size - 1] = new_data[row_idx + 1];
	}
	const auto lastrow = field_size * (field_size - 1);
	const auto prelastrow = lastrow - field_size;
	for (size_t x = 0; x < field_size; x++)
	{
		new_data[x] = new_data[prelastrow + x];
		new_data[lastrow + x] = new_data[field_size + x];
	}
	for (int y = 1; y < field_size - 1; y++)
	{
		for (int x = 1; x < field_size - 1; x++)
		{
			const auto dest_idx = x + y * field_size;
			assert(dest_idx < size);
			const int sum = new_data[dest_idx - 1]
				+ new_data[dest_idx + 1]
				+ new_data[dest_idx + field_size - 1]
				+ new_data[dest_idx + field_size]
				+ new_data[dest_idx + field_size + 1]
				+ new_data[dest_idx - field_size - 1]
				+ new_data[dest_idx - field_size]
				+ new_data[dest_idx - field_size + 1];
			if (sum == 3)
			{
				data[dest_idx] = 1;
			}
			else if (sum == 2)
			{
				data[dest_idx] = new_data[dest_idx];
			}
			else
			{
				data[dest_idx] = 0;
			}
		}
	}
}

void logic::parallel(std::vector<PixelData>& shadow_state, int field_size, std::vector<PixelData>& shadow_state_double)
{
}

void logic::parallel_branchless(std::vector<PixelData>& shadow_state, int field_size, std::vector<PixelData>& shadow_state_double)
{
}

void logic::parallel_branchless_const(std::vector<PixelData>& shadow_state, int field_size, std::vector<PixelData>& shadow_state_double)
{
}

void logic::parallel_branchless_shared(std::vector<PixelData>& shadow_state, int field_size, std::vector<PixelData>& shadow_state_double)
{
}
