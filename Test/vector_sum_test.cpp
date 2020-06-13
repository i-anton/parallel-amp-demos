#include "stdafx.h"
#include "../AMPIntro/vector_sum.h"

using namespace std;
using namespace vector_sum;

auto* seq_ptr = sequental;
auto* amp_ptr = amp_impl;
auto* omp_ptr = openmp;

struct TestDataSet {
	unique_ptr<MatrixValue[]> A;
	unique_ptr<MatrixValue[]> B;
	unique_ptr<MatrixValue[]> C;
	TestDataSet(int size) :
		A(new MatrixValue[size]),
		B(new MatrixValue[size]),
		C(new MatrixValue[size])
	{}
};

TestDataSet make_dataset_increased(int size) {
	TestDataSet t(size);
	for (int i = 0; i < size; i++) {
		t.A[i] = i;
		t.B[i] = i * 2.0;
		t.C[i] = 0;
	}
	return t;
}

TestDataSet make_dataset_zeroed(int size) {
	TestDataSet t(size);
	for (int i = 0; i < size; i++) {
		t.A[i] = -i;
		t.B[i] = i;
		t.C[i] = 0;
	}
	return t;
}

struct VectorSumTestData {
	int size;
	FunctionSignature calc;
	friend std::ostream& operator<<(std::ostream& os, VectorSumTestData data) {
		return os << "size: " << data.size << " type: " << to_string(data.calc);
	}
};

class TestVectorSum : public testing::TestWithParam<VectorSumTestData> {};

TEST_P(TestVectorSum, SumOfnegAndPosIsZero) {
	const auto param = GetParam();
	const auto data = make_dataset_zeroed(param.size);

	auto result_buffer = data.C.get();
	param.calc(param.size, data.A.get(), data.B.get(), result_buffer);

	for (int i = 0; i < param.size; i++) {
		EXPECT_EQ(result_buffer[i], 0);
	}
}

TEST_P(TestVectorSum, SumOfSameIsTripled) {
	const auto param = GetParam();
	const auto data = make_dataset_increased(param.size);

	auto result_buffer = data.C.get();
	param.calc(param.size, data.A.get(), data.B.get(), result_buffer);

	for (int i = 0; i < param.size; i++) {
		EXPECT_EQ(result_buffer[i], i*3);
	}
}

INSTANTIATE_TEST_CASE_P(Default, TestVectorSum,
	testing::Values(
		VectorSumTestData{ 1, seq_ptr },
		VectorSumTestData{ 100, seq_ptr }, 
		VectorSumTestData{ 1000, seq_ptr }, 
		VectorSumTestData{ 10000, seq_ptr },
		
		VectorSumTestData{ 1, amp_ptr },
		VectorSumTestData{ 100, amp_ptr },
		VectorSumTestData{ 1000, amp_ptr },
		VectorSumTestData{ 10000, amp_ptr },

		VectorSumTestData{ 1, omp_ptr },
		VectorSumTestData{ 100, omp_ptr },
		VectorSumTestData{ 1000, omp_ptr },
		VectorSumTestData{ 10000, omp_ptr }
	)
);
