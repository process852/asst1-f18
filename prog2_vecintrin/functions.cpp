#include <stdio.h>
#include <algorithm>
#include <math.h>
#include "CMU418intrin.h"
#include "logger.h"
using namespace std;


void absSerial(float* values, float* output, int N) {
    for (int i=0; i<N; i++) {
	float x = values[i];
	if (x < 0) {
	    output[i] = -x;
	} else {
	    output[i] = x;
	}
    }
}

// implementation of absolute value using 15418 instrinsics
void absVector(float* values, float* output, int N) {
    __cmu418_vec_float x;
    __cmu418_vec_float result;
    __cmu418_vec_float zero = _cmu418_vset_float(0.f);
    __cmu418_mask maskAll, maskIsNegative, maskIsNotNegative;
	__cmu418_mask maskInN;

    //  Note: Take a careful look at this loop indexing.  This example
    //  code is not guaranteed to work when (N % VECTOR_WIDTH) != 0.
    //  Why is that the case?
    for (int i=0; i<N + VECTOR_WIDTH; i+=VECTOR_WIDTH) {

	// All ones
	maskAll = _cmu418_init_ones();

	// All zeros
	maskIsNegative = _cmu418_init_ones(0);

	// Load vector of values from contiguous memory addresses
	// 不能加载超过数组的范围
	maskInN = _cmu418_init_ones(N - i);
	maskAll = _cmu418_mask_and(maskAll, maskInN);
	_cmu418_vload_float(x, values+i, maskAll);               // x = values[i];

	// Set mask according to predicate 小于0的设置为 1
	_cmu418_vlt_float(maskIsNegative, x, zero, maskAll);     // if (x < 0) {

	// Execute instruction using mask ("if" clause)
	// 确保数据范围数组长度之内
	maskIsNegative = _cmu418_mask_and(maskIsNegative, maskInN);
	_cmu418_vsub_float(result, zero, x, maskIsNegative);      //   output[i] = -x;

	// Inverse maskIsNegative to generate "else" mask
	maskIsNotNegative = _cmu418_mask_not(maskIsNegative);     // } else {

	// Execute instruction ("else" clause)
	maskIsNotNegative = _cmu418_mask_and(maskIsNotNegative, maskInN);
	_cmu418_vload_float(result, values+i, maskIsNotNegative); //   output[i] = x; }

	// Write results back to memory
	_cmu418_vstore_float(output+i, result, maskAll);
    }
}

// Accepts an array of values and an array of exponents
// For each element, compute values[i]^exponents[i] and clamp value to
// 4.18.  Store result in outputs.
// Uses iterative squaring, so that total iterations is proportional
// to the log_2 of the exponent
void clampedExpSerial(float* values, int* exponents, float* output, int N) {
    for (int i=0; i<N; i++) {
	float x = values[i];
	float result = 1.f;
	int y = exponents[i];
	float xpower = x;
	while (y > 0) {
	    if (y & 0x1)
		result *= xpower;
	    xpower = xpower * xpower;
	    y >>= 1;
	}
	if (result > 4.18f) {
	    result = 4.18f;
	}
	output[i] = result;
    }
}

void clampedExpVector(float* values, int* exponents, float* output, int N) {
    // Implement your vectorized version of clampedExpSerial here
    //  ...
	__cmu418_vec_float x, result;
	__cmu418_vec_int y;
	__cmu418_vec_int zero = _cmu418_vset_int(0);
	__cmu418_vec_int one = _cmu418_vset_int(1);
	__cmu418_vec_float clamp = _cmu418_vset_float(4.18f);
	__cmu418_mask maskInN, maskYisPositive;
	__cmu418_mask maskRightBit, maskGreater;
	__cmu418_vec_int RightBit;
	for(int i = 0; i < N + VECTOR_WIDTH; i += VECTOR_WIDTH){
		maskInN = _cmu418_init_ones(N - i); // 在数组有效范围内的元素 mask 值为 1
		_cmu418_vload_float(x, values + i, maskInN); // 加载数组元素
		_cmu418_vload_int(y, exponents + i, maskInN); // 加载指数
		_cmu418_vset_float(result, 1.0f, maskInN); // 初始化结果为 1.0
		_cmu418_vset_int(RightBit, 1, maskInN);
		maskYisPositive = _cmu418_init_ones(0); // 全部初始化为 0
		maskGreater = _cmu418_init_ones(0);
		maskRightBit = _cmu418_init_ones(0);

		// 进入 while 循环部分
		_cmu418_vgt_int(maskYisPositive, y, zero, maskInN); // y > 0
		while(_cmu418_cntbits(maskYisPositive)){ // 统计向量中 y > 0 的个数
			_cmu418_vbitand_int(RightBit, y, one, maskInN);
			_cmu418_vgt_int(maskRightBit, RightBit, zero, maskInN);
			if(_cmu418_cntbits(maskRightBit))
				_cmu418_vmult_float(result, result, x, maskRightBit);
			_cmu418_vmult_float(x, x, x, maskInN);
			_cmu418_vshiftright_int(y, y, one, maskInN); // y >>= 1
			maskYisPositive = _cmu418_init_ones(0);
			_cmu418_vset_int(RightBit, 1, maskInN);
			maskRightBit = _cmu418_init_ones(0);
			_cmu418_vgt_int(maskYisPositive, y, zero, maskInN);
		}

		// result > 4.18f
		_cmu418_vgt_float(maskGreater, result, clamp, maskInN);
		_cmu418_vset_float(result, 4.18f, maskGreater);

		// store
		_cmu418_vstore_float(output + i, result, maskInN);
		

	}

}


float arraySumSerial(float* values, int N) {
    float sum = 0;
    for (int i=0; i<N; i++) {
	sum += values[i];
    }

    return sum;
}

// Assume N % VECTOR_WIDTH == 0
// Assume VECTOR_WIDTH is a power of 2
float arraySumVector(float* values, int N) {
    // Implement your vectorized version here
    //  ...
}
