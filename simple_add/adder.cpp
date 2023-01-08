#include <stdio.h>
#include <stdlib.h>
#include "HLS/hls.h"

component int adder(int a, int b) {
  return (a + b);
}

int main() {
  bool passed = true;

  printf("Testing adder:\n");

  for (int i=0; i<100; i++) {

    int t1 = rand();
    int t2 = rand();

    int a = t1;
    int b = t2;
    int c = adder(a, b);
    int c_golden = t1 + t2;
    
    if (c != c_golden) {
        passed = false;
        printf("Addition failed\n");
    }

    printf("-----------------------------------------\n");
    printf("DUT: %d + %d = %d\n", (int)a, (int)b, (int)c);
    printf("REF: %d + %d = %d\n", t1, t2, c_golden);
  }

  if (passed) printf("PASSED\n");
  else        printf("FAILED\n");

  return 0;
}

