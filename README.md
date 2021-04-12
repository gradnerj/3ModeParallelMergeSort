# 3 Mode Parallel Merge Sort

This repository is an implementation based on the research found here <a>https://dl.acm.org/doi/10.14778/1454159.1454171</a>.

This merge sort uses three levels of parallelism:
1. Multithreading with openMP
2. Intel intrinsics for SIMD vectorization
3. Pipeline parallelism through instruction level programming

The paper above uses 128-bit registers and produces results 3.3x faster than the scaler version. I've scaled up to using 512-bit registers and the wall times can be seen below for sorting 2<sup>25</sup> elements.

