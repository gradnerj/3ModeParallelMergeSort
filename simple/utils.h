#include "immintrin.h"
#include <cstdlib>
#include <chrono>
#include <cstdio>
#include <bits/stdc++.h>

void print_vector_int(__m512i v, std::string name){
    #if defined (__GNUC__)
        int* temp = (int*)aligned_alloc(64, sizeof(int) * 16);
    #elif defined (_MSC_VER)
        int* temp = (int*)_aligned_malloc(sizeof(int) * 16, 64);
    #endif
    _mm512_store_si512(temp, v);
    printf("The vector called %s contains: ", name.c_str());
    for (int i = 0; i < 16; i++){
        printf("%02d ", temp[i]);
    }
    printf("\n");
    #if defined (__GNUC__)
        free(temp);
    #elif defined (_MSC_VER)
        _aligned_free(temp);
    #endif
}

void print_vector_float(__m512 v, std::string name){
    #if defined (__GNUC__)
    float* temp = (float*)aligned_alloc(64, sizeof(float) * 16);
    #elif defined (_MSC_VER)
    float* temp = (float*)_aligned_malloc(sizeof(float) * 16, 64);
    #endif
    _mm512_store_ps(temp, v);
    printf("The vector called %s contains: ", name.c_str());
    for (int i = 0; i < 16; i++){
        printf("%3f ", temp[i]);
    }
    printf("\n");
    #if defined (__GNUC__)
        free(temp);
    #elif defined (_MSC_VER)
        _aligned_free(temp);
    #endif
}

