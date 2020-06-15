#pragma once
#include "stdafx.h"

using PixelData = int;

namespace logic {
	using FunctionSignature = void (*)(std::vector<PixelData>& input_v, std::vector<PixelData>& output_v, std::vector<PixelData>& source_v, int field_size, int k);
	void sequental(std::vector<PixelData>& input_v, std::vector<PixelData>& output_v, std::vector<PixelData>& source_v, int field_size, int k);
	void global(std::vector<PixelData>& input, std::vector<PixelData>& output, std::vector<PixelData>& sources, int field_size, int k);
	void textured(std::vector<PixelData>& input, std::vector<PixelData>& output, std::vector<PixelData>& sources, int field_size, int k);
};