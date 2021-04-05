//#define _GNU_SOURCE
#include "utils.h"
#include "bitonic_sort.h"
#include "random"
#include <omp.h>
#include <sched.h>

const unsigned int BLOCK_SIZE = 16;
const unsigned int N = 256;
const unsigned int BLOCKS = N / BLOCK_SIZE;
const unsigned int END_BLOCK_SIZE = 32;

void sort_blocks(int* arr){
    int blocks = N / BLOCK_SIZE;
	for(int i = 0; i < BLOCKS; i++){
		std::sort(arr + (BLOCK_SIZE * i), arr + (BLOCK_SIZE*(i+1)));	
	}
}


bool validator(int* checking){
	int blocks = N / END_BLOCK_SIZE;
	int i = 0;
	while(i < blocks){
		int tmp[END_BLOCK_SIZE] = {0};
	    std::memcpy(&tmp[0], &checking[i*END_BLOCK_SIZE], sizeof(int)*END_BLOCK_SIZE);
		std::sort(tmp, tmp+END_BLOCK_SIZE);
		for(int v = 0; v < END_BLOCK_SIZE; v++){
			if(tmp[v] != checking[i * END_BLOCK_SIZE + v]){
				return false;
			}
		}
		i++;		
	}
	return true;
}


int main(){
    
	// Prepare data

	// 256 values
	int data[] = {78,  22,  60,  62,  29,  57,   6,  48,  58,  38,  22,  64,  77,
         3,  39,  39,  83,  14,   0,  71,  55,   5,  32,  76,  48,   8,
        65,  34,  40,  91,  42,  48,  19,  89,  56,  55,  19,  72,  91,
        14,  45,  14,  49,  81,  29,  53,  60,  82,  48,  18,  75,   8,
        40,  96,   9,  60,  32,  52,  31,  34,   6,  23,  43,  87,  68,
        25,  73,  74,   5,  63,  52,  96,  70,  52,  26,   2,  80,  22,
        64,  49,  13,  70,  77,  48,  15,   4,  22,  29,  71,  78,  34,
        65,  70,  96,   9,  82,  25,  90,   8,  99,  22,  57,  14,   9,
        17, 100,  26,  93,  89,   0,  52,  66,  28,  10,  15,   9,  46,
         3,  94,  10,   9,  80,  85,  16,  31,  38,  47,  97,  19,  14,
         0,  62, 100,  59,  24,  79,  37,   1,  91,  92,  45,  20,  45,
        58,  50,  60,  19,  68,  99,  26,  24,  50,  55,  76,  28,  82,
        50,  29,  66,  55,  73,  46,   3,  40,  83,  31,  48,  84,  16,
        44,  98,  70,  13,  37,  68,  39,  84,  11,  14,  81,  52,  28,
        83,  40, 100,  97,  87,  16,  81,  86,  69,  32,  62,  80,  59,
        24,  60,  91,  82,  74,  25,  56,  35,  41,  87,  27,  20,   9,
        82,  57,  18,  93,   0,  25,  41,   7,  55,  63,  33,  57,  30,
        32,  97,  20,  27,  72,  87,   1,  31,  24,  66,   0,  24,  28,
        80,  47,  15,   8,  99,  10,  81,  18,  90,   8,  52,  33,  35,
        10,   1,  23,  84,  23,  64,  36,  20,  74};
	
	/*
	int data[N] = {};
	std::default_random_engine generator;
  	std::uniform_int_distribution<int> distribution(0,100);
	for (int i=0; i<N; ++i) {
    	int number = distribution(generator);
    	data[i] = number;
  	}	
	*/
	int* arr = (int*)aligned_alloc(64, sizeof(int) * N);
	
	for(int i = 0; i < N; i++){ // copy arr1 into a1
		arr[i] = data[i];
	}

	
    sort_blocks(arr); // Sort blocks of 16 with O(n^2) sort
	
		
	/*
	bit_sort(arr, BLOCK_SIZE, N);
	return 0;	
    */
	
	/*	
	std::cout << "Printing data in sorted blocks of 16" << std::endl;
	for(int i = 0; i < N; i++){
		std::cout << arr[i] << " ";
	}
	std::cout << "\n";
	*/

	// Start of psuedocode implementation
	__m512i A1in, A2in, B1in, B2in, C1in, C2in, D1in, D2in,A1out, A2out, B1out, B2out, C1out, C2out, D1out, D2out;	
	int sorted_block_size = 16;
	int end_sorted_block_size = END_BLOCK_SIZE;

	while(sorted_block_size <= end_sorted_block_size){
		int start_idx = 0;
		int end_idx = N;

		for(int arr_idx = start_idx; arr_idx < end_idx; arr_idx += sorted_block_size * 8){
			// 8 starting indices and 8 ending indices				
			int stA1 = arr_idx, 
				stA2 = arr_idx + sorted_block_size,
				stB1 = arr_idx + sorted_block_size * 2,
				stB2 = arr_idx + sorted_block_size * 3,
				stC1 = arr_idx + sorted_block_size * 4, 
				stC2 = arr_idx + sorted_block_size * 5,
				stD1 = arr_idx + sorted_block_size * 6, 
				stD2 = arr_idx + sorted_block_size * 7; 
			
			int	eA1 = stA1 + BLOCK_SIZE, 
				eA2 = stA2 + BLOCK_SIZE,
				eB1 = stB1 + BLOCK_SIZE, 
				eB2 = stB2 + BLOCK_SIZE, 
				eC1 = stC1 + BLOCK_SIZE, 
				eC2 = stC2 + BLOCK_SIZE, 
				eD1 = stD1 + BLOCK_SIZE, 
				eD2 = stD2 + BLOCK_SIZE;

			// 4 indices to track where to write back out to the array 
			int write_A = stA1,
		    	write_B = stB1,
				write_C = stC1, 
				write_D = stD1;
		
			// Load 8 vectors
			A1in = _mm512_load_si512(&arr[stA1]);
			A2in = _mm512_load_si512(&arr[stA2]);	
			B1in = _mm512_load_si512(&arr[stB1]);
			B2in = _mm512_load_si512(&arr[stB2]);		
			C1in = _mm512_load_si512(&arr[stC1]);
			C2in = _mm512_load_si512(&arr[stC2]);	
			D1in = _mm512_load_si512(&arr[stD1]);
			D2in = _mm512_load_si512(&arr[stD2]);
			
			for(int j = 0; j < (sorted_block_size / 8) - 1; j++){

				std::cout << "j:  "  << j << " Sorted block size: " << sorted_block_size << std::endl;		
				
				bitonic_sort(A1in, A2in, B1in, B2in, C1in, C2in, D1in, D2in, A1out, A2out, B1out, 
								B2out, C1out, C2out, D1out, D2out);

				_mm512_store_si512(arr+write_A, A1out);
				_mm512_store_si512(arr+write_B, B1out);
				_mm512_store_si512(arr+write_C, C1out);
				_mm512_store_si512(arr+write_D, D1out);
				
				write_A += 16;
				write_B += 16;
				write_C += 16;
				write_D += 16;
				
				A1in = A2out;
				B1in = B2out;
				C1in = C2out;
			    D1in = D2out;	
				
				if(j+1 == ((sorted_block_size / 8) - 1)){
					_mm512_store_si512(arr+write_A, A2out);
					_mm512_store_si512(arr+write_B, B2out);
					_mm512_store_si512(arr+write_C, C2out);
					_mm512_store_si512(arr+write_D, D2out);
				}
				else{
					if(stA1 == eA1){
						stA2 += 16;
						A2in = _mm512_load_si512(&arr[stA2]);
					}else if (stA2 == eA2){
						stA1 += 16;
						A2in = _mm512_load_si512(&arr[stA1]);
					}else if (stA1 + 16 < stA2 + 16){
						stA1 += 16;
						A2in = _mm512_load_si512(&arr[stA1]);
					}else{
						stA2 +=16;
						A2in = _mm512_load_si512(&arr[stA2]);
					}
					
					if(stB1 == eB1){
						stB2 += 16;
						B2in = _mm512_load_si512(&arr[stB2]);
					}else if (stB2 == eB2){
						stB1 += 16;
						B2in = _mm512_load_si512(&arr[stB1]);
					}else if (stB1 + 16 < stB2 + 16){
						stB1 += 16;
						B2in = _mm512_load_si512(&arr[stB1]);
					}else{
						stB2 +=16;
						B2in = _mm512_load_si512(&arr[stB2]);
					}
					
					if(stC1 == eC1){
						stC2 += 16;
						C2in = _mm512_load_si512(&arr[stC2]);
					}else if (stC2 == eC2){
						stC1 += 16;
						C2in = _mm512_load_si512(&arr[stC1]);
					}else if (stC1 + 16 < stC2 + 16){
						stC1 += 16;
						C2in = _mm512_load_si512(&arr[stC1]);
					}else{
						stC2 +=16;
						C2in = _mm512_load_si512(&arr[stC2]);
					}

					if(stD1 == eD1){
						stD2 += 16;
						D2in = _mm512_load_si512(&arr[stD2]);
					}else if (stD2 == eD2){
						stD1 += 16;
						D2in = _mm512_load_si512(&arr[stD1]);
					}else if (stD1 + 16 < stD2 + 16){
						stD1 += 16;
						D2in = _mm512_load_si512(&arr[stD1]);
					}else{
						stD2 +=16;
						D2in = _mm512_load_si512(&arr[stD2]);
					} 
				}	
				
			}		
				
		}
		sorted_block_size *= 2;
	}
	
	std::cout << "Printing the contents of data:\n";
	for(int i = 0; i < N; i++){
		if(i > 0 && i % 16 == 0){
			std::cout << "\n";
		}
		std::cout << arr[i] << " ";
	}
	std::cout << std::endl;

	std::cout << "Validation Results: ";
	int match = validator(arr);
	if(match){
		std::cout << "Passed";
	}	
	else{
		std::cout << "Failed";
	}
	std::cout << std::endl;
    return 0;
}
