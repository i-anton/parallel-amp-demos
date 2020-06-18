#include "logic.h"

void logic::sequental(PixelVector& input_v, PixelVector& output_v, PixelVector& source_v, int field_size, int k, int iter) {
	const auto input = &(input_v[0]);
	const auto output = &(output_v[0]);
	const auto source = &(source_v[0]);
	const auto n = field_size - 1;
	for (int y = 0; y < field_size; y++)
	{
		const auto y0 = (y - 1) * field_size;
		const auto y1 = y0 + field_size;
		const auto y2 = y1 + field_size;
		for (int x = 0; x < field_size; x++)
		{
			const auto top = y0 + x;
			const auto bottom = y2 + x;
			const auto center = y1 + x;
			const auto right = center + 1;
			const auto left = center - 1;
			auto neighbours_sum = 0;
			if (x != 0) neighbours_sum += input[left];
			if (x != n) neighbours_sum += input[right];
			if (y != 0) neighbours_sum += input[top];
			if (y != n) neighbours_sum += input[bottom];
			output[center] = input[center] + (neighbours_sum - 4 * input[center]) * k;
		}
	}
	for (int y = 0; y < field_size; y++)
	{
		const auto y1 = y * field_size;
		for (int x = 0; x < field_size; x++)
		{
			const auto center = y1 + x;
			output[center] = source[center] != 0 ? source[center] : output[center];
		}
	}
}

void logic::global(PixelVector& input_v, PixelVector& output_v, PixelVector& source_v, int field_size, int k, int iter) {
	using namespace concurrency;
	const auto input = &(input_v[0]);
	const auto output = &(output_v[0]);
	const auto source = &(source_v[0]);
	const auto n = field_size - 1;
	array_view<const PixelData, 2> in(field_size, field_size, input);
	array_view<PixelData, 2> out(field_size, field_size, output);
	array_view<const PixelData, 2> heat(field_size, field_size, source);
	out.discard_data();
	parallel_for_each(out.extent, [=](index<2>idx)restrict(amp)
		{
			const auto x = idx[0];
			const auto y = idx[1];
			auto neighbours_sum = 0;
			if (x != 0) neighbours_sum += in(x - 1, y);
			if (x != n) neighbours_sum += in(x + 1, y);
			if (y != 0) neighbours_sum += in(x, y - 1);
			if (y != n) neighbours_sum += in(x, y + 1);
			out[idx] = in[idx] + (neighbours_sum - 4 * in[idx]) * k;
		}
	);
	parallel_for_each(out.extent, [=](index<2>idx)restrict(amp)
		{
			if (heat[idx] != 0)
				out[idx] = heat[idx];
		}
	);
	out.synchronize();
}

void logic::textured(PixelVector& input_v, PixelVector& output_v, PixelVector& source_v, int field_size, int k, int iter) {
	using namespace concurrency;
	const auto output = &(output_v[0]);
	const auto n = field_size - 1;
	graphics::texture<PixelData, 2> tex(field_size, field_size, input_v.cbegin(), input_v.cend());
	graphics::texture<PixelData, 2> heat(field_size, field_size, source_v.cbegin(), source_v.cend());
	array_view<PixelData, 2> out(field_size, field_size, output);
	out.discard_data();
	parallel_for_each(out.extent, [=, &tex](index<2>idx)restrict(amp)
		{
			const auto x = idx[0];
			const auto y = idx[1];
			auto neighbours_sum = 0;
			if (x != 0) neighbours_sum += tex(x - 1, y);
			if (x != n) neighbours_sum += tex(x + 1, y);
			if (y != 0) neighbours_sum += tex(x, y - 1);
			if (y != n) neighbours_sum += tex(x, y + 1);
			out[idx] = tex[idx] + (neighbours_sum - 4 * tex[idx]) * k;
		}
	);
	parallel_for_each(out.extent, [=, &heat](index<2>idx) restrict(amp)
		{
			if (heat[idx] != 0)
				out[idx] = heat[idx];
		}
	);
	out.synchronize();
}
