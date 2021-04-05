#include "utils.h"
#include "bitonic_sort.h"
#include "raw_bitonic.h"

const unsigned int BLOCK_SIZE = 16;
const unsigned int N = 128;
const unsigned int BLOCKS = N / BLOCK_SIZE;

void sort_blocks(int* arr){
    int blocks = N / BLOCK_SIZE;
	for(int i = 0; i < BLOCKS; i++){
		std::sort(arr + (BLOCK_SIZE * i), arr + (BLOCK_SIZE*(i+1)));	
	}
}


int main(){
    
	// Prepare data
	// 64 values

	/*
	int data[] = { 26, 61, 29, 47, 67, 28, 49, 35, 95, 99, 9, 20, 43, 45, 42, 42,
					 4, 56, 33, 72, 0, 70, 50, 4, 6, 68, 98, 43, 64, 47, 76, 48,
	 			   	 3, 60, 91, 42, 55, 37, 22, 40, 26, 55, 37, 45, 37, 74, 88, 0,
					 45, 54, 13, 6, 80, 61, 63, 91, 86, 51, 66, 22, 38, 26, 84, 44 };
	*/

	// 128 values
	int data[] = {30,  48,  34,   0,  94,   2,  64,  47,  35,  51,  94,  20,   9,
        20,  51,  67,   5,  72,  67,  26,  68,  93,   3,   6,  83,  99,
        42,  16,  91,  15,  23,  36,  68,  67,  57,  23,  59,  20,  38,
        27,  13,  68,  88,  24,  42,  20,  92,  12,  32,  71,   4,  40,
        18,  10,  79,  95,  94,  47,  43,  75,  83,   4,  27,  32,  83,
        76,   4,  93,  80,  62,   9,  85,  73,   0,  72,  78,  66,  64,
        65,  58,   6,  51,  99,  81,  12,  19,  87,  50,  44,  27,  43,
        52,  42,  60,  95,  19,  79,  50,  62,  46,  18,  25,  84,  35,
         0,  84,  22,  39,   0,  69,  91,  87,  25,  10, 100,   8,  77,
        72,  85,  86,  28,  89,  28,  59,  14,  68,  99,  95};



	int* arr = (int*)aligned_alloc(64, sizeof(int) * N);
	
	for(int i = 0; i < N; i++){ // copy arr1 into a1
		arr[i] = data[i];
	}

	
    sort_blocks(arr); // Sort blocks of 16 with O(n^2) sort
	
	/*
	bit_sort(arr, BLOCK_SIZE, N);
	return 0;	
    */
	
	std::cout << "Printing data in sorted blocks of 16" << std::endl;
	for(int i = 0; i < N; i++){
		std::cout << arr[i] << " ";
	}
	std::cout << "\n";
	
	// Start of psuedocode implementation
	__m512i A1in, A2in, B1in, B2in, C1in, C2in, D1in, D2in,A1out, A2out, B1out, B2out, C1out, C2out, D1out, D2out;	
	int sorted_block_size = 16;
	int end_sorted_block_size = 32;

	while(sorted_block_size < end_sorted_block_size){
		int start_idx = 0;
		int end_idx = sorted_block_size;

		for(int arr_idx = start_idx; arr_idx < end_idx; arr_idx += sorted_block_size * 8){
			// 8 starting indices and 8 ending indices				
			int stA1 = arr_idx, 
				stA2 = arr_idx + BLOCK_SIZE,
				stB1 = arr_idx + BLOCK_SIZE * 2,
				stB2 = arr_idx + BLOCK_SIZE * 3,
				stC1 = arr_idx + BLOCK_SIZE * 4, 
				stC2 = arr_idx + BLOCK_SIZE * 5,
				stD1 = arr_idx + BLOCK_SIZE * 6, 
				stD2 = arr_idx + BLOCK_SIZE * 7; 
			
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
			
			for(int j = 0; j < (sorted_block_size / 8) - 1; ++j){

				std::cout << "Sorted block size: " << sorted_block_size << std::endl;		
				
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
				std::cout << "j: " << j << std::endl;	
				if(j+1 == (sorted_block_size / 8) - 1){
					_mm512_store_si512(arr+write_A, A2out);
					_mm512_store_si512(arr+write_B, B2out);
					_mm512_store_si512(arr+write_C, C2out);
					_mm512_store_si512(arr+write_C, C2out);
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
    return 0;
}
