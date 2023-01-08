//  Copyright (c) 2021 Intel Corporation                                  
//  SPDX-License-Identifier: MIT                                          

#include "HLS/hls.h"
#include "HLS/math.h"

#include <stdio.h> //printf
#include <stdlib.h> //srand, rand
#include "HLS/ac_fixed.h"

// Number of taps
#define NUM_TAPS 32
// Number of Test Inputs
#define TEST_INPUTS 1024

#define TEST_SEED 3

typedef ac_fixed<16, 1, true> fixed_data_t; // 16 bit total width
                                            // 1 bit integer width 
                                            // 15 bits fraction width

// HLS component to test implemented in filter.cpp
component fixed_data_t fir_filter (fixed_data_t x);

// CPU implementation
void golden_fir_filter(float* test_inputs, float* expected, int num_inputs) {

  static const float coeffs[NUM_TAPS] = {
    -0.003852168201892195f,
    -0.013081856594749709f,
    0.010010260080449414f,
    0.031291002556661535f,
    0.016086283040442007f,
    -0.038895648712939476f,
    -0.030625680181133576f,
    0.006840427139813927f,
    0.007537995674188784f,
    -0.011211077287440443f,
    0.06365912824643992f,
    0.10016861585467923f,
    -0.05201892745373349f,
    -0.2194681135600675f,
    -0.0767203890034225f,
    0.21884627232526455f,
    0.21884627232526455f,
    -0.0767203890034225f,
    -0.2194681135600675f,
    -0.05201892745373349f,
    0.10016861585467923f,
    0.06365912824643992f,
    -0.011211077287440443f,
    0.007537995674188777f,
    0.006840427139813927f,
    -0.030625680181133576f,
    -0.038895648712939476f,
    0.016086283040442007f,
    0.031291002556661535f,
    0.010010260080449421f,
    -0.013081856594749709f,
    -0.003852168201892195f
  };

  for (int i = 0; i < num_inputs; ++i) {
    expected[i] = 0.0f;

    int c = 0;
    // Accumulate over all taps
    for (int t = i; (t > (i - NUM_TAPS)) && (t >= 0); --t) {
      expected[i] += test_inputs[t] * coeffs[c++];
    }
  }

}

int main(void) {

  fixed_data_t test_inputs_dut[TEST_INPUTS]; 
  fixed_data_t out_dut[TEST_INPUTS];
  float test_inputs_golden[TEST_INPUTS];
  float out_golden[TEST_INPUTS];

  srand(TEST_SEED);

  // Generate test inputs
  for (int i = 0; i < TEST_INPUTS; ++i) {
    // generate inputs between 0.0 and 20.0
    float x_golden = (float) rand() / (((float) RAND_MAX));
    ac_fixed<16, 1, true> x_dut(x_golden);

    test_inputs_golden[i] = x_golden;
    test_inputs_dut[i] = x_dut;

    // Enqueue the input to the component for execution in the simulator, but don't run the component right away
    //    When the component is simulated, its pipeline will be completely filled by the input data
    //    The output data is available for the testbench to read from the out stream
    // Emulation runs this immediately
    ihc_hls_enqueue(out_dut+i, &fir_filter, x_dut);
  }
  ihc_hls_component_run_all(fir_filter);

  // Generate expected outputs from golden models
  golden_fir_filter(test_inputs_golden, out_golden, TEST_INPUTS);

  // Run filter component and check outputs
  int fail_count = 0;
  for (int i = 0; i < TEST_INPUTS; ++i) {
    // retrieve the next output from the component
    float y = (float)(out_dut[i]).to_double();

    // verify the correctness of the component's output
    if (fabs(y - out_golden[i]) > 0.001) {
      printf("ERROR: Expected %f, got %f\n", out_golden[i], y);
      fail_count++;
    }
  }

  // print final test results
  if (fail_count == 0) {
    printf("(%d tests run) PASSED\n", TEST_INPUTS);
  } else {
    printf("(%d incorrect results) FAILED\n",fail_count);
  }

  return 0;
}
