//  Copyright (c) 2021 Intel Corporation                                  
//  SPDX-License-Identifier: MIT                                          

#include "HLS/hls.h"

#include <stdio.h> //printf
#include <stdlib.h> //srand, rand
#include "HLS/ac_fixed.h"

typedef ac_fixed<16, 1, true> fixed_data_t; // 16 bit total width
                                            // 1 bit integer width 
                                            // 15 bits fraction width

// Number of taps
#define NUM_TAPS 32

component fixed_data_t fir_filter (fixed_data_t x) {
  static const fixed_data_t coeffs[NUM_TAPS] = {
    (fixed_data_t)-0.003852168201892195,
    (fixed_data_t)-0.013081856594749709,
    (fixed_data_t)0.010010260080449414,
    (fixed_data_t)0.031291002556661535,
    (fixed_data_t)0.016086283040442007,
    (fixed_data_t)-0.038895648712939476,
    (fixed_data_t)-0.030625680181133576,
    (fixed_data_t)0.006840427139813927,
    (fixed_data_t)0.007537995674188784,
    (fixed_data_t)-0.011211077287440443,
    (fixed_data_t)0.06365912824643992,
    (fixed_data_t)0.10016861585467923,
    (fixed_data_t)-0.05201892745373349,
    (fixed_data_t)-0.2194681135600675,
    (fixed_data_t)-0.0767203890034225,
    (fixed_data_t)0.21884627232526455,
    (fixed_data_t)0.21884627232526455,
    (fixed_data_t)-0.0767203890034225,
    (fixed_data_t)-0.2194681135600675,
    (fixed_data_t)-0.05201892745373349,
    (fixed_data_t)0.10016861585467923,
    (fixed_data_t)0.06365912824643992,
    (fixed_data_t)-0.011211077287440443,
    (fixed_data_t)0.007537995674188777,
    (fixed_data_t)0.006840427139813927,
    (fixed_data_t)-0.030625680181133576,
    (fixed_data_t)-0.038895648712939476,
    (fixed_data_t)0.016086283040442007,
    (fixed_data_t)0.031291002556661535,
    (fixed_data_t)0.010010260080449421,
    (fixed_data_t)-0.013081856594749709,
    (fixed_data_t)-0.003852168201892195
  };

  // The FIFO is sized based on the number of taps. Note that in this case, the
  // capacity of the FIFO is larger than required. The compiler will optimize
  // the extra storage away.
  static fixed_data_t taps[NUM_TAPS];

  fixed_data_t y = 0;

  // shift in input data
  #pragma unroll
  for(int i = (NUM_TAPS - 1); i > 0; --i) {
    taps[i] = taps[i-1];
  }
  taps[0] = x;

  // accumulate over all taps
  //#pragma unroll
  for(int i = 0; i <= (NUM_TAPS - 1); ++i) {
    y += taps[i] * coeffs[i];
  }

  return y;
}
