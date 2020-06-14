#include "stdafx.h"
#include "../AMPIntro/sum_reduction.h"

using namespace std;
using namespace sum_reduction;

auto* simple_ptr = simple;
auto* simple_windowed_ptr = simple_windowed<8>;
auto* block_strided_ptr = block_strided<16>;
auto* block_cascaded_ptr = block_cascaded<16>;

struct TestDataSet {
	unique_ptr<vector<MatrixValue>> vec;
	TestDataSet(int size) : vec(new vector<MatrixValue>(size)) {
		for (int i = 1; i < size; i++) {
			vec->push_back(static_cast<MatrixValue>(i));
		}
	}
};

struct SumReductionData {
	int size;
	FunctionSignature calc;
	friend std::ostream& operator<<(std::ostream& os, SumReductionData data) {
		return os << "size: " << data.size << " type: " << to_string(data.calc);
	}
};

class TestSumReduction : public testing::TestWithParam<SumReductionData> {};

TEST_P(TestSumReduction, IsArithmProgression) {
	const auto param = GetParam();
	const auto size = param.size;
	unique_ptr<vector<MatrixValue>> data(new vector<MatrixValue>(size));
	for (int i = 0; i < size; i++) {
		data->push_back(static_cast<MatrixValue>(i));
	}
	auto result = param.calc(*data);
	EXPECT_EQ(result, (data->front() + data->back()) * size / 2);
}


INSTANTIATE_TEST_CASE_P(Default, TestSumReduction,
	testing::Values(
		SumReductionData{ 64, simple_ptr },
		SumReductionData{ 128, simple_ptr },

		SumReductionData{ 64, simple_windowed_ptr },
		SumReductionData{ 128, simple_windowed_ptr },

		SumReductionData{ 64, block_strided_ptr },
		SumReductionData{ 128, block_strided_ptr },

		SumReductionData{ 128, block_cascaded_ptr },
		SumReductionData{ 256, block_cascaded_ptr }
	)
);
