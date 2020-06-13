#include "stdafx.h"
#include "../AMPIntro/matrix_multiply_scalar.h"

using namespace std;
using namespace matrix_multiply_scalar;

struct TestDataSet {
	unique_ptr<MatrixValue[]> A;
	unique_ptr<MatrixValue[]> C;

	TestDataSet(size_t size_x, size_t size_y) :
		A(new MatrixValue[size_x * size_y]),
		C(new MatrixValue[size_x * size_y]) {
		const auto size = size_x * size_y;
		for (int i = 0; i < size; i++) {
			A[i] = static_cast<MatrixValue>(i);
			C[i] = static_cast<MatrixValue>(0);
		}
	}
};

struct MatrixMulScalarTestData {
	int size_x;
	int size_y;
	FunctionSignature calc;
	friend std::ostream& operator<<(std::ostream& os, MatrixMulScalarTestData data) {
		return os << "size: (" << data.size_x
			<< ", " << data.size_y << ")"
			<< " type: " << to_string(data.calc);
	}
};

class MatrixMulScalarTest : public testing::TestWithParam<MatrixMulScalarTestData> {};

TEST_P(MatrixMulScalarTest, MultiplyOnZero) {
	const auto param = GetParam();
	const auto data = TestDataSet(param.size_x, param.size_y);

	auto result_buffer = data.C.get();
	param.calc(
		Matrix{ param.size_x, param.size_y, data.A.get() },
		0.0,
		Matrix{ param.size_x, param.size_y, result_buffer }
	);
	const auto size = param.size_x * param.size_y;
	for (int i = 0; i < size; i++) {
		EXPECT_EQ(result_buffer[i], 0);
	}
}

TEST_P(MatrixMulScalarTest, MultyplyOnOne) {
	const auto param = GetParam();
	const auto data = TestDataSet(param.size_x, param.size_y);

	auto result_buffer = data.C.get();
	const auto A = data.A.get();
	param.calc(
		Matrix { param.size_x, param.size_y, A },
		1.0,
		Matrix { param.size_x, param.size_y, result_buffer }
	);
	const auto size = param.size_x * param.size_y;
	for (int i = 0; i < size; i++) {
		EXPECT_EQ(result_buffer[i], A[i]);
	}
}

TEST_P(MatrixMulScalarTest, MultyplyOnNumber) {
	const auto param = GetParam();
	const auto data = TestDataSet(param.size_x, param.size_y);

	const auto A = data.A.get();
	auto result_buffer = data.C.get();
	param.calc(
		Matrix { param.size_x, param.size_y, A },
		10.0,
		Matrix { param.size_x, param.size_y, result_buffer }
	);
	const auto size = param.size_x * param.size_y;
	for (int i = 0; i < size; i++) {
		EXPECT_EQ(result_buffer[i], A[i] * 10.0);
	}
}

const auto high = 70;
const auto low = 30;

auto* seq_ptr = sequental;
auto* amp_ptr = amp_impl;
auto* omp_ptr = openmp;

INSTANTIATE_TEST_CASE_P(Default, MatrixMulScalarTest,
	testing::Values(
		MatrixMulScalarTestData{ 1, 1, seq_ptr },
		MatrixMulScalarTestData{ high, low, seq_ptr },
		MatrixMulScalarTestData{ low, high, seq_ptr },
		MatrixMulScalarTestData{ high, high, seq_ptr },

		MatrixMulScalarTestData{ 1, 1, amp_ptr },
		MatrixMulScalarTestData{ high, low, amp_ptr },
		MatrixMulScalarTestData{ low, high, amp_ptr },
		MatrixMulScalarTestData{ high, high, amp_ptr },

		MatrixMulScalarTestData{ 1, 1, omp_ptr },
		MatrixMulScalarTestData{ high, low, omp_ptr },
		MatrixMulScalarTestData{ low, high, omp_ptr },
		MatrixMulScalarTestData{ high, high, omp_ptr }
	)
);
