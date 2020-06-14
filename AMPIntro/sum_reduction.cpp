#include "sum_reduction.h"

using namespace Concurrency;

MatrixValue sum_reduction::simple(const std::vector<MatrixValue>& input)
{
	const auto size = static_cast<int>(input.size());
	array<MatrixValue, 1> data(size, &input[0]);
	for (int stride = (size / 2); stride > 0; stride /= 2)
	{
		parallel_for_each(extent<1>(stride),
			[=, &data](index<1> idx) restrict(amp)
			{
				data[idx] += data[idx + stride];
			});
	}
	return data.section(0, 1)[0];
}

sum_reduction::FunctionSignature sum_reduction::from_string(const std::string& name)
{
	if (name == "simple") return simple;
	else if (name == "simple_windowed") return simple_windowed<8>;
	else if (name == "block_strided") return block_strided<16>;
	else if (name == "block_cascaded") return block_cascaded<16>;
	return nullptr;
}

std::string sum_reduction::to_string(FunctionSignature name)
{
	if (name == simple) return "simple";
	else if (name == simple_windowed<8>) return "simple_windowed";
	else if (name == block_strided<16>) return "block_strided";
	else if (name == block_cascaded<16>) return "block_cascaded";
	return std::string();
}
