//#define _GNU_SOURCE
#include "utils.h"
#include "bitonic_sort.h"
#include "random"
#include <omp.h>
#include <sched.h>

const unsigned int BLOCK_SIZE = 16;
const unsigned int N = 256;
const unsigned int BLOCKS = N / BLOCK_SIZE;
const unsigned int END_BLOCK_SIZE = 64;

// Sorts all blocks of 16 given an array.
void sort_blocks(int* arr){
    int blocks = N / BLOCK_SIZE;
	for(int i = 0; i < BLOCKS; i++){
		std::sort(arr + (BLOCK_SIZE * i), arr + (BLOCK_SIZE*(i+1)));	
	}
}

// Makes copy of unsorted data in chunks of END_BLOCK_SIZE and
// compares each element to the output from the bitonic sort/merge.
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
	std::default_random_engine generator;
  	std::uniform_int_distribution<int> distribution(0,100);
	
	int* arr = (int*)aligned_alloc(64, sizeof(int) * N);
	
	for(int i = 0; i < N; i++){ // Generate N normally distributed integers.
		arr[i] = distribution(generator);
	}

   	
    sort_blocks(arr); // Sort blocks of 16 with O(nlgn) sort
	
	// Start of psuedocode implementation
	__m512i A1in, A2in, B1in, B2in, C1in, C2in, D1in, D2in,A1out, A2out, B1out, B2out, C1out, C2out, D1out, D2out;
	int sorted_block_size = 16;
	int end_sorted_block_size = END_BLOCK_SIZE;

	while(sorted_block_size <= end_sorted_block_size){
		int start_idx = 0;
		int end_idx = N;

		for(int arr_idx = start_idx; arr_idx < end_idx; arr_idx += sorted_block_size * 8){
			// 8 starting indices
			int stA1 = arr_idx,
				stA2 = arr_idx + sorted_block_size,
				stB1 = arr_idx + sorted_block_size * 2,
				stB2 = arr_idx + sorted_block_size * 3,
				stC1 = arr_idx + sorted_block_size * 4,
				stC2 = arr_idx + sorted_block_size * 5,
				stD1 = arr_idx + sorted_block_size * 6, 
				stD2 = arr_idx + sorted_block_size * 7; 
			
			// 8 end indices
			int	eA1 = stA1 + (sorted_block_size),
				eA2 = stA2 + (sorted_block_size),
				eB1 = stB1 + (sorted_block_size), 
				eB2 = stB2 + (sorted_block_size), 
				eC1 = stC1 + (sorted_block_size), 
				eC2 = stC2 + (sorted_block_size), 
				eD1 = stD1 + (sorted_block_size), 
				eD2 = stD2 + (sorted_block_size);
			
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
			
			for(int j = 0; j < (sorted_block_size / 8)-1; j++){

				std::cout << "j:  "  << j << " Sorted block size: " << sorted_block_size << std::endl;		
				
				bitonic_sort(A1in, A2in, B1in, B2in, C1in, C2in, D1in, D2in, A1out, A2out, B1out, 
								B2out, C1out, C2out, D1out, D2out);
				
				// Write first output to memory
				_mm512_store_si512(arr+write_A, A1out);
				_mm512_store_si512(arr+write_B, B1out);
				_mm512_store_si512(arr+write_C, C1out);
				_mm512_store_si512(arr+write_D, D1out);
				
				// Increment write indices
				write_A += 16;
				write_B += 16;
				write_C += 16;
				write_D += 16;
				
				// Reuse second outputs	
				A1in = A2out;
				B1in = B2out;
				C1in = C2out;
			    D1in = D2out;	
				
				
				if(j + 1 == ((sorted_block_size / 8) - 1)){ // if on the last iteration, write second outputs to memory
					_mm512_store_si512(arr+write_A, A2out);
					_mm512_store_si512(arr+write_B, B2out);
					_mm512_store_si512(arr+write_C, C2out);
					_mm512_store_si512(arr+write_D, D2out);
				}
				else{ // Determine the second input for the next iteration
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
		sorted_block_size = sorted_block_size * 2;
	}
	
	print_vector_int(A1in, "A1in: ");
	print_vector_int(A2in, "A2in: ");
	print_vector_int(A1out, "A1out: ");
	print_vector_int(A2out, "A2out: ");
	
	std::cout << "Printing the sorted blocks:\n";
	for(int i = 0; i < N; i++){
		
		if(i > 0 && i % END_BLOCK_SIZE == 0){
			std::cout << std::endl << std::endl;
		}
		else if(i > 0 && i % 16 == 0){
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
