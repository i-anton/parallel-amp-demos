#pragma once
#include "stdafx.h"

using PixelData = int;

namespace logic {
	void sequental(std::vector<PixelData>& shadow_state, int field_size, std::vector<PixelData>& shadow_state_double);
	void parallel(std::vector<PixelData>& shadow_state, int field_size, std::vector<PixelData>& shadow_state_double);
	void parallel_branchless(std::vector<PixelData>& shadow_state, int field_size, std::vector<PixelData>& shadow_state_double);
	void parallel_branchless_const(std::vector<PixelData>& shadow_state, int field_size, std::vector<PixelData>& shadow_state_double);
	void parallel_branchless_shared(std::vector<PixelData>& shadow_state, int field_size, std::vector<PixelData>& shadow_state_double);
};