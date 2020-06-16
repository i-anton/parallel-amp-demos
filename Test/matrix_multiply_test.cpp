#include "stdafx.h"
#include "../AMPIntro/matrix_multiply.h"

using namespace std;
using namespace matrix_multiply;

struct MatrixMultiplyTestData {
	const Matrix& A;
	const Matrix& B;
	const Matrix& expected;
	FunctionSignature calc;
	MatrixMultiplyTestData(const Matrix& _a, const Matrix& _b, const Matrix& _c, FunctionSignature calc) :
		A(_a), B(_b), expected(_c), calc(calc) {}
	friend std::ostream& operator<<(std::ostream& os, MatrixMultiplyTestData data) {
		return os << "A size: (" << data.A.size_x
			<< ", " << data.A.size_y << "),"
			<< " B size: (" << data.B.size_x
			<< ", " << data.B.size_y << "),"
			<< " type: " << to_string(data.calc);
	}
};

class MatrixMulTest : public testing::TestWithParam<MatrixMultiplyTestData> {};

TEST_P(MatrixMulTest, ValidateWithTestData) {
	const auto param = GetParam();
	auto& A = param.A;
	auto& B = param.B;
	unique_ptr<MatrixValue[]> data(new MatrixValue[static_cast<size_t>(A.size_x) * static_cast<size_t>(B.size_y)]);
	const auto c_size = A.size_x * B.size_y;
	auto pC = data.get();
	for (int i = 0; i < c_size; i++) pC[i] = 0;

	auto& C = Matrix{ A.size_x, B.size_y, pC };
	param.calc(A, B, C);
	const auto expected = param.expected;
	EXPECT_EQ(expected.size_x, C.size_x);
	EXPECT_EQ(expected.size_y, C.size_y);
	const auto expected_size = expected.size_x * expected.size_y;
	for (int idx = 0; idx < expected_size; ++idx) {
		EXPECT_EQ(pC[idx], expected.values[idx]);
	}
}

const auto high = 25;
const auto low = 10;

const auto size_l = 2;
const auto size_h = 5;

const auto values = vector<tuple<MatrixValue*, MatrixValue*, MatrixValue*>>{
	make_tuple(
		new MatrixValue[10] 
			{ 1.0, 2.0, 3.0, 4.0, 5.0,
			  6.0, 7.0, 8.0, 9.0, 10.0},
		new MatrixValue[10] 
			{ 1.0, 2.0,
			  3.0, 4.0,
			  5.0, 6.0,
			  7.0, 8.0,
			  9.0, 10.0},
		new MatrixValue[4] { 95.0, 110.0,
							220.0, 260.0}
	),
	make_tuple(
		new MatrixValue[10]
			{ 1.0, 2.0,
			  3.0, 4.0,
			  5.0, 6.0,
			  7.0, 8.0,
			  9.0, 10.0},
		new MatrixValue[10]
			{ 1.0, 2.0, 3.0, 4.0, 5.0,
			  6.0, 7.0, 8.0, 9.0, 10.0},
	
		new MatrixValue[25] 
			{ 13.0, 16.0, 19.0, 22.0, 25.0,
			  27.0, 34.0, 41.0, 48.0, 55.0,
			  41.0, 52.0, 63.0, 74.0, 85.0,
			  55.0, 70.0, 85.0, 100.0, 115.0,
			  69.0, 88.0, 107.0, 126.0, 145.0}
	),
	make_tuple(
		new MatrixValue[9]
			{ 1.0, 2.0, 3.0,
			  4.0, 5.0, 6.0,
			  7.0, 8.0, 9.0 },
		new MatrixValue[9]
			{ 1.0, 2.0, 3.0,
			  4.0, 5.0, 6.0,
			  7.0, 8.0, 9.0 },
	
		new MatrixValue[9]
			{ 30.0, 36.0, 42.0,
			  66.0, 81.0, 96.0,
			  102.0, 126.0, 150.0 }
	)
};
const auto columnOnRow = vector<Matrix>{
	Matrix {size_l, size_h, get<0>(values[0])},
	Matrix {size_h, size_l, get<1>(values[0])},
	Matrix {size_l, size_l, get<2>(values[0])}
};

const auto rowOnColumn = vector<Matrix>{
	Matrix {size_h, size_l, get<0>(values[1])},
	Matrix {size_l, size_h, get<1>(values[1])},
	Matrix {size_h, size_h, get<2>(values[1])}
};

const auto squared = vector<Matrix>{
	Matrix {3, 3, get<0>(values[2])},
	Matrix {3, 3, get<1>(values[2])},
	Matrix {3, 3, get<2>(values[2])}
};

auto* seq_ptr = sequental;
auto* amp_ptr = amp_impl;
auto* omp_ptr = openmp;

auto* block_no_shared_ptr = block_no_shared<32>;
auto* block_shared_ptr = block_shared<32>;
auto* enlarged_ptr = enlarged;

INSTANTIATE_TEST_CASE_P(Default, MatrixMulTest,
	testing::Values(
		MatrixMultiplyTestData(columnOnRow[0], columnOnRow[1], columnOnRow[2], seq_ptr),
		MatrixMultiplyTestData(rowOnColumn[0], rowOnColumn[1], rowOnColumn[2], seq_ptr),
		MatrixMultiplyTestData(squared[0], squared[1], squared[2], seq_ptr),

		MatrixMultiplyTestData(columnOnRow[0], columnOnRow[1], columnOnRow[2], amp_ptr),
		MatrixMultiplyTestData(rowOnColumn[0], rowOnColumn[1], rowOnColumn[2], amp_ptr),
		MatrixMultiplyTestData(squared[0], squared[1], squared[2], amp_ptr),

		MatrixMultiplyTestData(columnOnRow[0], columnOnRow[1], columnOnRow[2], omp_ptr),
		MatrixMultiplyTestData(rowOnColumn[0], rowOnColumn[1], rowOnColumn[2], omp_ptr),
		MatrixMultiplyTestData(squared[0], squared[1], squared[2], omp_ptr),

		MatrixMultiplyTestData(columnOnRow[0], columnOnRow[1], columnOnRow[2], block_no_shared_ptr),
		MatrixMultiplyTestData(rowOnColumn[0], rowOnColumn[1], rowOnColumn[2], block_no_shared_ptr),
		MatrixMultiplyTestData(squared[0], squared[1], squared[2], block_no_shared_ptr),

		MatrixMultiplyTestData(columnOnRow[0], columnOnRow[1], columnOnRow[2], block_shared_ptr),
		MatrixMultiplyTestData(rowOnColumn[0], rowOnColumn[1], rowOnColumn[2], block_shared_ptr),
		MatrixMultiplyTestData(squared[0], squared[1], squared[2], block_shared_ptr),

		MatrixMultiplyTestData(columnOnRow[0], columnOnRow[1], columnOnRow[2], enlarged_ptr),
		MatrixMultiplyTestData(rowOnColumn[0], rowOnColumn[1], rowOnColumn[2], enlarged_ptr),
		MatrixMultiplyTestData(squared[0], squared[1], squared[2], enlarged_ptr)
	)
);
