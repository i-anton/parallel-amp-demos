#include "logic.h"

// field_size is shadow_row_size
void logic::sequental(PixelVector& shadow_state, int field_size, PixelVector& shadow_state_double, int iter)
{
	const auto data = &(shadow_state[0]);
	const auto new_data = &(shadow_state_double[0]);
	const auto size = shadow_state.size();
	for (size_t i = 0; i < size; i++)
		new_data[i] = data[i];

	const auto n = field_size - 2;
	const auto t = field_size - 1;
	for (size_t y = 0; y < field_size; y++)
	{
		const auto row_idx = y * field_size;
		new_data[row_idx] = new_data[row_idx + n];
		new_data[row_idx + t] = new_data[row_idx + 1];
	}
	const auto lastrow = field_size * t;
	const auto prelastrow = lastrow - field_size;
	for (size_t x = 0; x < field_size; x++)
	{
		new_data[x] = new_data[prelastrow + x];
		new_data[lastrow + x] = new_data[field_size + x];
	}
	for (int y = 1; y < t; y++)
	{
		const auto y0 = (y - 1) * field_size;
		const auto y1 = y0 + field_size;
		const auto y2 = y1 + field_size;
		for (int x = 1; x < t; x++)
		{
			const auto xl = x - 1;
			const auto xr = x + 1;
			const int sum = new_data[y1 + xl]
				+ new_data[y1 + xr]
				+ new_data[y2 + xl]
				+ new_data[y2 + x]
				+ new_data[y2 + xr]
				+ new_data[y0 + xl]
				+ new_data[y0 + x]
				+ new_data[y0 + xr];

			const auto dest_idx = x + y1;
			assert(dest_idx < size);
			if (sum == 3)
				data[dest_idx] = 1;
			else if (sum == 2)
				data[dest_idx] = new_data[dest_idx];
			else
				data[dest_idx] = 0;
		}
	}
}

void logic::parallel(PixelVector& shadow_state, int field_size, PixelVector& shadow_state_double, int iter)
{
	using namespace concurrency;
	const auto data = &(shadow_state[0]);
	array_view<PixelData, 2> data_out(field_size, field_size, data);
	data_out.discard_data();
	array<PixelData, 2> new_buffer(field_size, field_size, data);
	const auto n = field_size - 2;
	const auto t = field_size - 1;
	parallel_for_each(extent<1>(field_size),
		[=, &new_buffer](index<1> idx) restrict(amp)
		{
			const auto row_idx = idx[0];
			new_buffer(0, row_idx) = new_buffer(n, row_idx);
			new_buffer(t, row_idx) = new_buffer(1, row_idx);
		});
	parallel_for_each(extent<1>(field_size), [=, &new_buffer](index<1> idx) restrict(amp)
		{
			const auto x = idx[0];
			new_buffer(x, 0) = new_buffer(x, n);
			new_buffer(x, t) = new_buffer(x, 1);
		});
	parallel_for_each(extent<1>(n), [=, &new_buffer](index<1> idx) restrict(amp)
		{
			const auto y0 = idx[0];
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
				if (sum == 3)
					data_out(x, y1) = 1;
				else if (sum == 2)
					data_out(x, y1) = new_buffer(x, y1);
				else
					data_out(x, y1) = 0;
			}
		});
	data_out.synchronize();
}

void logic::parallel_branchless(PixelVector& shadow_state, int field_size, PixelVector& shadow_state_double, int iter)
{
	using namespace concurrency;
	const auto data = &(shadow_state[0]);
	array_view<PixelData, 2> data_out(field_size, field_size, data);
	data_out.discard_data();
	array<PixelData, 2> new_buffer(field_size, field_size, data);
	const auto n = field_size - 2;
	const auto t = field_size - 1;
	parallel_for_each(extent<1>(field_size),
		[=, &new_buffer](index<1> idx) restrict(amp)
		{
			const auto row_idx = idx[0];
			new_buffer(0, row_idx) = new_buffer(n, row_idx);
			new_buffer(t, row_idx) = new_buffer(1, row_idx);
		});
	parallel_for_each(extent<1>(field_size), [=, &new_buffer](index<1> idx) restrict(amp)
		{
			const auto x = idx[0];
			new_buffer(x, 0) = new_buffer(x, n);
			new_buffer(x, t) = new_buffer(x, 1);
		});
	const PixelData rules[18] = {
		0, 0, 0, 1, 0, 0, 0, 0, 0, // dead
		0, 0, 1, 1, 0, 0, 0, 0, 0 // alive
	};
	array_view<const PixelData, 2> rules_in(9, 2, rules);
	parallel_for_each(extent<1>(n), [=, &new_buffer](index<1> idx) restrict(amp)
		{
			const auto y0 = idx[0];
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
				data_out(x, y1) = rules_in(new_buffer(x, y1), sum);
			}
		});
	data_out.synchronize();
}

void logic::parallel_branchless_const(PixelVector& shadow_state, int field_size, PixelVector& shadow_state_double, int iter)
{
	using namespace concurrency;
	const auto data = &(shadow_state[0]);
	array_view<PixelData, 2> data_out(field_size, field_size, data);
	data_out.discard_data();
	array<PixelData, 2> new_buffer(field_size, field_size, data);
	const auto n = field_size - 2;
	const auto t = field_size - 1;
	parallel_for_each(extent<1>(field_size),
		[=, &new_buffer](index<1> idx) restrict(amp)
		{
			const auto row_idx = idx[0];
			new_buffer(0, row_idx) = new_buffer(n, row_idx);
			new_buffer(t, row_idx) = new_buffer(1, row_idx);
		});
	parallel_for_each(extent<1>(field_size), [=, &new_buffer](index<1> idx) restrict(amp)
		{
			const auto x = idx[0];
			new_buffer(x, 0) = new_buffer(x, n);
			new_buffer(x, t) = new_buffer(x, 1);
		});
	SharedRules r;
	parallel_for_each(extent<1>(n), [=, &new_buffer](index<1> idx) restrict(amp)
		{
			const auto y0 = idx[0];
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
				data_out(x, y1) = r.rules[new_buffer(x, y1) * 9 + sum];
			}
		});
	data_out.synchronize();
}

