# CMU 15-418/618, Fall 2018

# Assignment 1

This is the starter code for Assignment 1 of CMU class 15-418/618, Fall 2018

Please review the course's policy on [academic
integrity](http://www.cs.cmu.edu/~418/academicintegrity.html),
regarding your obligation to keep your own solutions private from now
until eternity.


# asst1-f18
并行计算课程作业记录仓库

## Problem 2: Vectorizing Code Using SIMD Intrinsics (20 points)

1. Implement a vectorized version of clampedExpSerial() as the function clampedExpVector()
in file functions.cpp. Your implementation should work with any combination of input array
size N and vector width W.

需要考虑数组长度无法整除向量操作指令一次性可以处理的元素，需要利用mask来过滤超出部分。

2. Run ./vrun -s 10000 and sweep the vector width over the values {2, 4, 8, 16, 32}. Record the resulting vector utilization. You can do this by changing the defined value of VECTOR_WIDTH in CMU418intrin.h and recompiling the code each time. How much does the vector utilization change as W changes? Explain the reason for these changes and the degree of sensitivity the utilization has on the vector width. Explain how the total number of vector instructions varies with W.

| 数组长度 | 向量宽度 | 向量利用率 | vector instructions
| :------| :------: | ------: | ------: |
| 10000 | 2 | 95.690028% | 372427
| 10000 | 4 | 93.505313% | 194917
| 10000 | 8 | 92.811708% | 98690
| 10000 | 16 | 92.755052% | 49387
| 10000 | 32 | 92.634984% | 24739

* 向量宽度越大，向量利用率越低，但是降低的速率逐渐减低
* 向量宽度越大，向量指令数越少，大致减少一半


3. Extra credit: (1 point) Implement a vectorized version of arraySumSerial() as the function
arraySumVector() in file functions.cpp. Your implementation may assume that W is a
factor of the input array size N. Whereas the serial implementation has O(N) span, your implementation should have at most O(N/W + log2 W) span. You may find the hadd and interleave
operations useful.



