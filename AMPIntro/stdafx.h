#pragma once

#include <iostream>	
#include <memory>
#include <amp.h>   
#include <omp.h>

static const std::string unknownTitle = "unknown";
static const std::string seqentalTitle = "seq";
static const std::string ampTitle = "amp";
static const std::string openmpTitle = "omp";

using MatrixValue = float;

struct Matrix {
	const int size_x;
	const int size_y;
	MatrixValue* const values;
};