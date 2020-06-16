#include "stdafx.h"
#include "../AMPIntro/matrix_transpose.h"

using namespace std;
using namespace matrix_transpose;

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

struct MatrixTransposeTestData {
	int size_x;
	int size_y;
	FunctionSignature calc;
	friend std::ostream& operator<<(std::ostream& os, MatrixTransposeTestData data) {
		return os << "size: (" << data.size_x
			<< ", " << data.size_y << ")"
			<< " type: " << to_string(data.calc);
	}
};

class MatrixTransposeTest : public testing::TestWithParam<MatrixTransposeTestData> {};

TEST_P(MatrixTransposeTest, IsTransposed) {
	const auto param = GetParam();
	const auto data = TestDataSet(param.size_x, param.size_y);

	const auto A = data.A.get();
	auto result_buffer = data.C.get();
	param.calc(
		Matrix { param.size_x, param.size_y, A },
		Matrix { param.size_y, param.size_x, result_buffer }
	);
	const auto size_x = param.size_x;
	const auto size_y = param.size_y;
	for (int y = 0; y < size_y; y++) {
		for (int x = 0; x < size_x; x++) {
			const auto idx = y * size_x + x;
			const auto transposed_idx = x * size_y + y;
			EXPECT_EQ(result_buffer[idx], A[transposed_idx]);
		}
	}
}

const auto high = 64;
const auto low = 32;
auto* seq_ptr = sequental;
auto* amp_ptr = amp_impl;
auto* omp_ptr = openmp;
auto* blocked = block<16>;

INSTANTIATE_TEST_CASE_P(Default, MatrixTransposeTest, testing::Values(
	MatrixTransposeTestData{ 1, 1, seq_ptr },
	MatrixTransposeTestData{ high, low, seq_ptr },
	MatrixTransposeTestData{ low, high, seq_ptr },
	MatrixTransposeTestData{ high, high, seq_ptr },

	MatrixTransposeTestData{ 1, 1, amp_ptr },
	MatrixTransposeTestData{ high, low, amp_ptr },
	MatrixTransposeTestData{ low, high, amp_ptr },
	MatrixTransposeTestData{ high, high, amp_ptr },

	MatrixTransposeTestData{ 1, 1, omp_ptr },
	MatrixTransposeTestData{ high, low, omp_ptr },
	MatrixTransposeTestData{ low, high, omp_ptr },
	MatrixTransposeTestData{ high, high, omp_ptr },

	MatrixTransposeTestData{ 1, 1, blocked },
	MatrixTransposeTestData{ high, low, blocked },
	MatrixTransposeTestData{ low, high, blocked },
	MatrixTransposeTestData{ high, high, blocked })
);
