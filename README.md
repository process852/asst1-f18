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

## Problem 3: Parallel Fractal Generation Using ISPC (15 points)

#### 3.1 Problem 3, Part 1. A Few ISPC Basics (7 of 15 points)

熟悉利用 [ISPC](https://github.com/ispc/ispc) (Implicit SPMD Program Compiler)是一种程序编译器，用于处理单程序多数据的模式。ISPC是一种基于C语言扩展的编程语言用于在CPUS或GPUS上运行单程序多数据(SIMD)的单元。它提供了基于 4-wide vector SEE单元加速以及 8-wide AVX vector单元加速。同时也支持多个核的并行加速计算。

* ISPC 安装

```bash
snap install ispc # wsl暂时不支持

wget -O- https://apt.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB | gpg --dearmor | sudo tee /usr/share/keyrings/oneapi-archive-keyring.gpg > /dev/null

echo "deb [signed-by=/usr/share/keyrings/oneapi-archive-keyring.gpg] https://apt.repos.intel.com/oneapi all main" | sudo tee /etc/apt/sources.list.d/oneAPI.list

sudo apt-get update
sudo apt-get install intel-oneapi-ispc

// /opt/intel/oneapi/ispc/latest/bin/ispc 安装所在位置
// 添加到 PATH 路径
source /opt/intel/oneapi/ispc/latest/env/vars.sh
```