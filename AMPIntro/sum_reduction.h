#pragma once
#include "stdafx.h"

namespace sum_reduction {
	using FunctionSignature = MatrixValue (*)(const std::vector<MatrixValue> & input);
	
	MatrixValue simple(const std::vector<MatrixValue>& input);
	template <int window_size = 8>
	MatrixValue simple_windowed(const std::vector<MatrixValue>& input);
	template <int tile_size = 16>
	MatrixValue block_strided(const std::vector<MatrixValue>& input);
	template <int tile_size = 16>
	MatrixValue block_cascaded(const std::vector<MatrixValue>& input);

	FunctionSignature from_string(const std::string& name);
	std::string to_string(FunctionSignature name);
}
template <int window_size>
MatrixValue sum_reduction::simple_windowed(const std::vector<MatrixValue>& input)
{
	const auto size = static_cast<int>(input.size());
	concurrency::array<MatrixValue, 1> data(size, &input[0]);
	int prev_stride = size;
	for (int stride = (size / window_size); stride > 0; stride /= window_size)
	{
		concurrency::parallel_for_each(
			concurrency::extent<1>(stride),
			[=, &data](concurrency::index<1> idx) restrict(amp)
			{
				MatrixValue sum = 0;
				for (int i = 0; i < window_size; i++)
					sum += data[idx + i * stride];
				data[idx] = sum;
			});
		prev_stride = stride;
	}
	std::vector<MatrixValue> res(prev_stride);
	concurrency::copy(data.section(0, prev_stride), res.begin());
	MatrixValue result = MatrixValue();
	for (int i = 0; i < prev_stride; i++) { result += res[i]; }
	return result;
}

template <int tile_size>
MatrixValue sum_reduction::block_strided(const std::vector<MatrixValue>& input)
{
	auto input_size = static_cast<int>(input.size());
	concurrency::array<MatrixValue, 1> data(input_size, &input[0]);
	concurrency::array_view<MatrixValue, 1> inputData(data);
	concurrency::array_view<MatrixValue, 1> outputData(input_size / tile_size);
	outputData.discard_data();
	while (input_size % tile_size == 0)
	{
		concurrency::parallel_for_each(
			concurrency::extent<1>(input_size).tile<tile_size>(),
			[=](concurrency::tiled_index<tile_size> tidx) restrict(amp)
			{
				const int tid = tidx.local[0];
				tile_static MatrixValue localData[tile_size];
				localData[tid] = inputData[tidx.global[0]];
				tidx.barrier.wait();
				for (int stride = 1; stride < tile_size; stride *= 2)
				{
					if (tid % (stride * 2) == 0)
						localData[tid] += localData[tid + stride];
					tidx.barrier.wait();
				}
				if (tid == 0) outputData[tidx.tile[0]] = localData[0];
			});
		input_size /= tile_size;
		std::swap(outputData, inputData);
		outputData.discard_data();
	}
	std::vector<MatrixValue> res(input_size);
	concurrency::copy(inputData.section(0, input_size), res.begin());
	MatrixValue result = MatrixValue();
	for (int i = 0; i < input_size; i++) { result += res[i]; }
	return result;
}

template <int tile_size>
MatrixValue sum_reduction::block_cascaded(const std::vector<MatrixValue>& input)
{
	const auto input_size = static_cast<int>(input.size());
	concurrency::array<MatrixValue, 1> data(input_size, &input[0]);
	concurrency::array_view<const MatrixValue, 1> inputData(data);
	const auto block_count = input_size / tile_size;
	concurrency::array_view<MatrixValue, 1> partial_result(block_count);
	partial_result.discard_data();
	concurrency::parallel_for_each(
		concurrency::extent<1>(input_size).tile<tile_size>(),
		[=](concurrency::tiled_index<tile_size> tidx) restrict(amp)
		{
			const int tid = tidx.local[0];
			tile_static MatrixValue localData[tile_size];
			const int target_start_idx = tidx.tile[0] * tile_size + tid;
			localData[tid] = inputData[target_start_idx + tile_size];
			tidx.barrier.wait();

			for (int reduce_stride = tile_size / 2; reduce_stride > 0; reduce_stride /= 2)
			{
				if (tid < reduce_stride)
					localData[tid] += localData[tid + reduce_stride];
				tidx.barrier.wait();
			}
			if (tid == 0) partial_result[tidx.tile[0]] = localData[0];
		});
	partial_result.synchronize();
	std::vector<MatrixValue> res(block_count);
	concurrency::copy(partial_result.section(0, block_count), res.begin());
	MatrixValue result = MatrixValue();
	for (int i = 0; i < block_count; i++) { result += res[i]; }
	return result;
}