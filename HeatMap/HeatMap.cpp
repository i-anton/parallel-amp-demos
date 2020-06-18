#include "ui.h"
#include "timer.h"

using namespace std;
using namespace logic;

double life_check(const FunctionSignature func, int size, int runs) {
	if (size <= 0) exit(1);
	const auto shadow_size = static_cast<size_t>(size);
	const auto data_size = shadow_size * shadow_size;
	unique_ptr<vector<PixelData>> in(new vector<PixelData>(data_size));
	unique_ptr<vector<PixelData>> out(new vector<PixelData>(data_size));
	unique_ptr<vector<PixelData>> heat(new vector<PixelData>(data_size));
	const auto a_ptr = &((*in)[0]);
	const auto h_ptr = &((*in)[0]);
	for (size_t i = 0; i < data_size; i++)
		a_ptr[i] = rand() & 1;

	for (size_t i = 0; i < data_size; i++)
		h_ptr[i] = (rand() & 1) * 100;

	double acc_time = 0.0;
	for (size_t i = 0; i < runs; i++) {
		Timer t;
		t.Start();
		func(*in, *out, *heat, static_cast<int>(shadow_size), 2, 5);
		t.Stop();
		acc_time += t.Elapsed();
	}
	return acc_time / static_cast<double>(runs);
}

void bench()
{
	srand(static_cast<unsigned int>(time(0)));
	const int runs = 2;
	const vector<int> sizes{ 2048, 4096, 8192 };
	const vector<tuple<string, FunctionSignature>> to_check = {
		{"sequental", sequental},
		{"global", global},
		{"textured", textured}
	};
	life_check(global, 64, 1);
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
