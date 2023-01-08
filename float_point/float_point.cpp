//  Copyright (c) 2021 Intel Corporation                                  
//  SPDX-License-Identifier: MIT                                          

#include "HLS/hls.h"
#include "HLS/math_dsp_control.h"
#include <iostream>
#include <math.h>

using namespace ihc;

component float test_lib(float a, float b) {
  math_dsp_control<Preference::Softlogic>([&]{ 
    a += 7.89f; // Soft-logic
    a += b;     // Soft-logic 
  }); 
  return a;
}

component float test_ref(float a, float b) {
  a += 7.89f; // DSP by default
  a += b;     // DSP by default
  return a;
}

int main() {
  float input1 = 1.23f, input2 = 4.56f;

  float result = test_lib(input1, input2);
  float golden = test_ref(input1, input2);

  if(fabs(result - golden) < (result * 1e-5)) {
    std::cout << "PASSED\n";
  } else {
    std::cout << "FAILED\n";
    std::cout << "result = " << result << ", golden = " << golden << "\n";
  }

  return 0;
}
