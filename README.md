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

ispc example:

```bash
ispc simple.ispc -h simple_ispc.h
ispc -O2 simple.ispc -o simple.obj
g++ simple.cpp simple_ispc.h simple.obj -o main
```

* `export` 限定符表示 ISPC 函数可以被C/C++代码调用
* 函数直接使用C/C++传递的参数，没有API调用和动态库使用
* 所有参数必须都使用 `uniform` 限定符标识，即所有程序实例拥有相同的变量值
* 输出缓冲区的指针被传递为无大小的数组首地址元素

```C++
export void mandelbrot_ispc(uniform float x0, uniform float y0,
                            uniform float x1, uniform float y1,
                            uniform int width, uniform int height,
                            uniform int maxIterations,
                            uniform int output[])
{
    float dx = (x1 - x0) / width;
    float dy = (y1 - y0) / height;

    for (uniform int j = 0; j < height; j++) {
        // Note that we'll be doing programCount computations in parallel,
        // so increment i by that much.  This assumes that width evenly
        // divides programCount.
        foreach (i = 0 ... width) {
            // Figure out the position on the complex plane to compute the
            // number of iterations at.  Note that the x values are
            // different across different program instances, since its
            // initializer incorporates the value of the programIndex
            // variable.
            float x = x0 + i * dx;
            float y = y0 + j * dy;

            int index = j * width + i;
            output[index] = mandel(x, y, maxIterations);
        }
    }
}
```

基本概念：

* 一系列ISPC程序实例并发的运行，正在运行的程序实例组称为 `gang`。一个ispc程序实例类似于CUDA编程中的"thread"概念。

1. Compile and run the program mandelbrot.ispc. The ISPC compiler is configured to emit 8-
wide AVX2 vector instructions. What is the maximum speedup you expect given what you know
about these CPUs? Why might the number you observe be less than this ideal? Hint: Consider the characteristics of the computation you are performing. What parts of the image present challenges for SIMD execution? Comparing the performance of rendering the different views of the Mandelbrot set may help confirm your hypothesis.

输出结果：

```text
[mandelbrot serial]:            [185.511] ms
Wrote image file mandelbrot-serial.ppm
[mandelbrot ispc]:              [39.122] ms
Wrote image file mandelbrot-ispc.ppm
                                (4.74x speedup from ISPC)
```

加速效果不理想的原因：
* 不同图像视角的计算困难度是不一致的

| view | speedup | 
| :------| :------: | 
| 1 | 4.74x | 
| 2 | 4.09x | 
| 3 | 4.65x | 
| 4 | 4.49x | 
| 5 | 3.35x | 
| 6 | 4.93x |

* 并没有充分利用多核心处理单元


#### 3.2 Problem 3, Part 2: ISPC Tasks (8 of 15 points)

ispc 不仅可以提高在单个处理器核心上的加速效果，还可以促进在多个处理核心上的并行执行（通过`launch`关键字进行异步函数调用）。任何函数需要使用`launch`发布的任务都需要利用关键字`task`标识函数，且该函数必须返回`void`。

```C++
task void func(uniform float a[], uniform int index) {
    ...
    a[index] = ....
}

利用关键字 `launch` 发布异步的任务流

```C++
uniform float a[...] = ...;
launch func(a, 1);
```

改用多核心处理后，加速明显，执行命令 `./mandelbrot_ispc -v 1 --tasks`

```
[mandelbrot serial]:            [185.371] ms
Wrote image file mandelbrot-serial.ppm
[mandelbrot ispc]:              [38.675] ms
Wrote image file mandelbrot-ispc.ppm
[mandelbrot multicore ispc]:    [9.589] ms
Wrote image file mandelbrot-task-ispc.ppm
                                (4.79x speedup from ISPC)
                                (19.33x speedup from task ISPC)
```

## 4 Problem 4: Iterative Square Root (10 points)

