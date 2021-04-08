//#define _GNU_SOURCE
#include "utils.h"
#include "raw_bitonic.h"
#include "random"


const unsigned int BLOCK_SIZE = 16; 
const unsigned int N = 128;
const unsigned int BLOCKS = N / BLOCK_SIZE;
const unsigned int END_BLOCK_SIZE = 32; 

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


	bit_sort(arr, BLOCK_SIZE, N);
}
