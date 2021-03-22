#include <cstdlib>
#include "immintrin.h"
#include <chrono>
#include <cstdio>

#include <bits/stdc++.h> 
const unsigned int BLOCK_SIZE = 16;

void sort_blocks(int* arr){
    std::sort(arr, arr+BLOCK_SIZE);
	std::sort(arr+BLOCK_SIZE, arr+(BLOCK_SIZE*2));
}

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

int main(){
    int arr1[] = { 26, 61, 29, 47, 67, 28, 49, 35, 95, 99, 9, 20, 43, 45, 42, 42, 4, 56, 33, 72, 0, 70, 50, 4, 6, 68, 98, 43, 64, 47, 76, 48 };
	
	int* a1 = (int*)aligned_alloc(64, sizeof(int) * 32);

	for(int i = 0; i < 32; i++){ // copy arr1 into a1
		a1[i] = arr1[i];
	}

//    int arr2[] = { 03, 60, 91, 42, 55, 37, 22, 40, 26, 55, 37, 45, 37, 74, 88, 00, 45, 54, 13, 06, 80, 61, 63, 91, 86, 51, 66, 22, 38, 26, 84, 44 };
    std::cout << std::endl;
    sort_blocks(a1);
//    sort_blocks(arr2);
	for(int i = 0; i < 32; i++)
		std::cout << a1[i] << " ";
	std::cout << std::endl;
	__m512i first = _mm512_load_si512(&a1[0]);
    // __m512i second = _mm512_load_si512(&arr1[16]);

//	print_vector_int(first, "First");
    // print_vector_int(second, "Second");

    return 0;
}
