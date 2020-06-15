#include "ui.h"
#include "timer.h"

using namespace std;
using namespace logic;

double life_check(const FunctionSignature func, int size, int runs) {
	if (size <= 0) exit(1);
	const auto shadow_size = static_cast<size_t>(size) + 2;
	const auto data_size = shadow_size * shadow_size;
	unique_ptr<vector<PixelData>> A(new vector<PixelData>(data_size));
	unique_ptr<vector<PixelData>> B(new vector<PixelData>(data_size));
	const auto a_ptr = &(*A)[0];
	for (size_t i = 0; i < data_size; i++)
		a_ptr[i] = rand() & 1;
	double acc_time = 0.0;
	for (size_t i = 0; i < runs; i++) {
		Timer t;
		t.Start();
		func(*A, static_cast<int>(shadow_size), *B);
		t.Stop();
		acc_time += t.Elapsed();
	}
	return acc_time / static_cast<double>(runs);
}

void bench()
{
	srand(static_cast<unsigned int>(time(0)));
	const int runs = 5;
	const vector<int> sizes{ 2048, 4096, 8192 };
	life_check(parallel, 1024, 1);
	const vector<tuple<string, FunctionSignature>> to_check = {
		{"seq", sequental},
		{"parallel", parallel},
		{"parallel_branchless", parallel_branchless},
		{"parallel_branchless_const", parallel_branchless_const},
		{"parallel_branchless_shared", parallel_branchless_shared<8>},
	};
	for (const auto& check : to_check)
	{
		cout << std::get<0>(check) << endl;
		for (const auto size : sizes) wcout << life_check(std::get<1>(check), size, runs) << endl;
	}
}

int main()
{
	bench();
	UI app;
	app.start();
	return 0;
}
