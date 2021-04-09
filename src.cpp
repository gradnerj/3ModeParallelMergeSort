//#define _GNU_SOURCE
#include "utils.h"
#include "bitonic_sort.h"
#include "random"
#include <omp.h>
#include <unistd.h>
#include <sched.h>
#include <cstdio>
#include <math.h>
#include "index_logic.h"
const unsigned int BLOCK_SIZE = 16;
const unsigned int N = 33'554'432;
//const unsigned int N = 1073741824;
//const unsigned int N = 1024;
const unsigned int BLOCKS = N / BLOCK_SIZE;
const unsigned int END_BLOCK_SIZE = 16384;
const unsigned int BLOCK_OF_SORTED_BLOCKS = 65536;

// Sorts all blocks of 16 given an array.
void sort_blocks(int* arr, int block_size){
    int blocks = block_size / 16;
	for(int i = 0; i < blocks; i++){
		std::sort(arr + (16 * i), arr + (16*(i+1)));	
	}
}

// Makes copy of unsorted data in chunks of END_BLOCK_SIZE and
// compares each element to the output from the bitonic sort/merge.
bool validator(int* checking){
	int blocks = N / (END_BLOCK_SIZE);
	int i = 0;
	while(i < blocks){
		int tmp[END_BLOCK_SIZE] = {0};
	    std::memcpy(&tmp[0], &checking[i*END_BLOCK_SIZE], sizeof(int)*END_BLOCK_SIZE);
		std::sort(tmp, tmp+END_BLOCK_SIZE);
		for(int v = 0; v < END_BLOCK_SIZE; v++){
			if(tmp[v] != checking[i * END_BLOCK_SIZE + v]){
				std::cout << tmp[v] << " != " << checking[i * END_BLOCK_SIZE + v] << std::endl;
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
	
	int* arr1 = (int*)aligned_alloc(64, sizeof(int) * N);
	int* out = (int*)aligned_alloc(64, sizeof(int) * N);

/*    for(int i = 0; i < N; i++){
		arr1[i] = values[i];
	}
*/
	for(int i = 0; i < N; i++){ // Generate N normally distributed integers.
		arr1[i] = distribution(generator);
	}

   	
  //  sort_blocks(arr1); // Sort blocks of 16 with O(nlgn) sort
	int power_of_n = log2(END_BLOCK_SIZE);
	int current_work_unit = 0;	
	StopWatch sw;
	#pragma omp parallel for schedule(dynamic) num_threads(64)
	for(unsigned int i=0; i < N; i+= BLOCK_OF_SORTED_BLOCKS){
		int local_work_unit;
		#pragma omp critical
		{
			local_work_unit = current_work_unit;
			current_work_unit++;
		}
		int sorted_block_size = 16;
		int start_idx = local_work_unit * BLOCK_OF_SORTED_BLOCKS;
		int end_idx = (local_work_unit+1) * BLOCK_OF_SORTED_BLOCKS;
		int * arr = arr1;
		sort_blocks(&arr[start_idx], BLOCK_OF_SORTED_BLOCKS); 
		int * output = out;	
		
		if(power_of_n % 2 == 0){
			n_is_even(sorted_block_size, END_BLOCK_SIZE,  start_idx, end_idx, arr, output);
		}else{
			n_is_odd(sorted_block_size, END_BLOCK_SIZE,  start_idx, end_idx, arr, output);
		}
	}
	printf("Time elapsed: %f seconds\n", sw.elapsed());
/*	sw.reset();	
	std::cout << "Printing the sorted blocks in input: \n";
	for(int i = 0; i < N; i++){
		
		if(i > 0 && i % END_BLOCK_SIZE == 0){
			std::cout << std::endl << std::endl;
		}
		else if(i > 0 && i % 16 == 0){
			std::cout << "\n";
		}
		std::cout << arr1[i] << " ";

	}
	std::cout << std::endl;

	std::cout << "Printing the sorted blocks in ouput: \n";
	for(int i = 0; i < N; i++){
		
		if(i > 0 && i % END_BLOCK_SIZE == 0){
			std::cout << std::endl << std::endl;
		}
		else if(i > 0 && i % 16 == 0){
			std::cout << "\n";
		}
		std::cout << output[i] << " ";

	}
	std::cout << std::endl;
*/	
	std::cout << "Validation Results: ";
	int match = validator(arr1);
	if(match){
		std::cout << "Passed";
	}	
	else{
		std::cout << "Failed";
	}
	std::cout << std::endl;
	
//	printf("Time elapsed: %f seconds\n", sw.elapsed());
    return 0;
}
