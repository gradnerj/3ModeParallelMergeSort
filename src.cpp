#include "utils.h"
#include "bitonic_sort.h"
const unsigned int BLOCK_SIZE = 16;
const unsigned int N = 64;
const unsigned int BLOCKS = N / BLOCK_SIZE;

int out_idx = 0;

void sort_blocks(int* arr){
    int blocks = N / BLOCK_SIZE;
	for(int i = 0; i < BLOCKS; i++){
		std::sort(arr + (BLOCK_SIZE * i), arr + (BLOCK_SIZE*(i+1)));	
	}
}


int main(){
    
	// Prepare data
	int data[] = { 26, 61, 29, 47, 67, 28, 49, 35, 95, 99, 9, 20, 43, 45, 42, 42, 4, 56, 33, 72, 0, 70, 50, 4, 6, 68, 98, 43, 64, 47, 76, 48,
	 			    3, 60, 91, 42, 55, 37, 22, 40, 26, 55, 37, 45, 37, 74, 88, 0, 45, 54, 13, 6, 80, 61, 63, 91, 86, 51, 66, 22, 38, 26, 84, 44 };

	int* arr = (int*)aligned_alloc(64, sizeof(int) * N);
	
	for(int i = 0; i < N; i++){ // copy arr1 into a1
		arr[i] = data[i];
	}

	
    sort_blocks(arr); // Sort blocks of 16 with O(n^2) sort
    	
	std::cout << "Printing data in sorted blocks of 16" << std::endl;
	for(int i = 0; i < N; i++){
		std::cout << arr[i] << " ";
	}
	std::cout << "\n";
	
	// zmm register for block reversing indices	
	int reverse_idxs[] = {15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0};
	int *rev_idx_ptr = (int*)aligned_alloc(64, sizeof(int)*BLOCK_SIZE);
	for(int i = 0; i < BLOCK_SIZE; i++){
		rev_idx_ptr[i] = reverse_idxs[i];
	}
	__m512i rev_idx_vec = _mm512_load_si512(&rev_idx_ptr[0]);

	
	for(int b = 0; b < BLOCKS / 2; b++){
	
		__m512i first = _mm512_load_si512(&arr[BLOCK_SIZE * b]);  // (a0-a5)
    	__m512i second = _mm512_load_si512(&arr[BLOCK_SIZE * (b+1)]); // (b15-b0)
	
		// Reverse the second
		second = _mm512_permutexvar_epi32(rev_idx_vec, second);

		print_vector_int(first, "First");
    	print_vector_int(second, "Second");
	
		__m512i l = _mm512_min_epi32(first, second); // Get first min (L)
		__m512i h = _mm512_max_epi32(first, second); // Get first max (H)

		print_vector_int(l, "low");
		print_vector_int(h, "high");
	
		int l_idx[] = {0,1,2,3,4,5,6,7,16,17,18,19,20,21,22,23};
		int h_idx[] = {8,9,10,11,12,13,14,15,24,25,26,27,28,29,30,31};

		int *l_idx_ptr = (int*)aligned_alloc(64, sizeof(int)*BLOCK_SIZE);  
		int *h_idx_ptr = (int*)aligned_alloc(64, sizeof(int)*BLOCK_SIZE);

		for(int i = 0; i < BLOCK_SIZE; i++){
			l_idx_ptr[i] = l_idx[i];
			h_idx_ptr[i] = h_idx[i];
		}	
	
		__m512i l_idx_vec = _mm512_load_si512(&l_idx_ptr[0]);
		__m512i h_idx_vec = _mm512_load_si512(&h_idx_ptr[0]);

		__m512i new_l = _mm512_permutex2var_epi32(l,l_idx_vec,h);
		__m512i new_h = _mm512_permutex2var_epi32(l,h_idx_vec,h);
		print_vector_int(new_l, "New L"); // new output (L) in L2
		print_vector_int(new_h, "New H"); // new output (H) in L2

		/******* Start L2 to L3 ***********/	
		// Perform a min and max on the two outputs from the last step, new_l and new_h
		
		__m512i l2_min_l = _mm512_min_epi32(new_l, new_h);
		__m512i l2_max_h = _mm512_max_epi32(new_l, new_h);
	
		print_vector_int(l2_min_l, "L2 min (L)");
		print_vector_int(l2_max_h, "L2 max (H)");
	
		// Start L2 permuting

		int l2_l_idx[] = {0,1,2,3,16,17,18,19,8,9,10,11,24,25,26,27};
		int l2_h_idx[] = {4,5,6,7,20,21,22,23,12,13,14,15,28,29,30,31};	
	
		for(int i = 0; i < 16; i++){ // L2 indices for permuting
			l_idx_ptr[i] = l2_l_idx[i];
			h_idx_ptr[i] = l2_h_idx[i];
		}

		l_idx_vec = _mm512_load_si512(&l_idx_ptr[0]);
		h_idx_vec = _mm512_load_si512(&h_idx_ptr[0]);
	
		new_l = _mm512_permutex2var_epi32(l2_min_l, l_idx_vec, l2_max_h);
		new_h = _mm512_permutex2var_epi32(l2_min_l, h_idx_vec, l2_max_h);
	
	
		print_vector_int(new_l, "L2 first output"); // new output (L) in L3
		print_vector_int(new_h, "L2 second output"); // new output (H) in L3
	

		/******* Start L3 to L4 *********/
	
		__m512i l3_min_l = _mm512_min_epi32(new_l, new_h);
		__m512i l3_max_h = _mm512_max_epi32(new_l, new_h);
	

		print_vector_int(l3_min_l, "L3 min (L)");
		print_vector_int(l3_max_h, "L3 max (H)");
	
		// Start L3 to L4 permuting
		
		int l3_l_idx[] = {0,1,16,17,4,5,20,21,8,9,24,25,12,13,28,29};
		int l3_h_idx[] = {2,3,18,19,6,7,22,23,10,11,26,27,14,15,30,31};	
	

		for(int i = 0; i < BLOCK_SIZE; i++){ // L3 indices for permuting
			l_idx_ptr[i] = l3_l_idx[i];
			h_idx_ptr[i] = l3_h_idx[i];
		}

		l_idx_vec = _mm512_load_si512(&l_idx_ptr[0]);
		h_idx_vec = _mm512_load_si512(&h_idx_ptr[0]);

		new_l = _mm512_permutex2var_epi32(l3_min_l, l_idx_vec, l3_max_h);
		new_h = _mm512_permutex2var_epi32(l3_min_l, h_idx_vec, l3_max_h);

		print_vector_int(new_l, "L3 first output"); // new output (L) in L3
		print_vector_int(new_h, "L3 second output"); // new output (H) in L3
	
		/********** Start L4 to L5 ***************/
	
		__m512i l4_min_l = _mm512_min_epi32(new_l, new_h);
		__m512i l4_max_h = _mm512_max_epi32(new_l, new_h);
	
		print_vector_int(l4_min_l, "L4 min (L)");
		print_vector_int(l4_max_h, "L4 max (H)");

	
		int l4_l_idx[] = {0,16,2,18,4,20,6,22,8,24,10,26,12,28,14,30};
		int l4_h_idx[] = {1,17,3,19,5,21,7,23,9,25,11,27,13,29,15,31};	
	

		for(int i = 0; i < BLOCK_SIZE; i++){ // L4 indices for permuting
			l_idx_ptr[i] = l4_l_idx[i];
			h_idx_ptr[i] = l4_h_idx[i];
		}

		l_idx_vec = _mm512_load_si512(&l_idx_ptr[0]);
		h_idx_vec = _mm512_load_si512(&h_idx_ptr[0]);

		new_l = _mm512_permutex2var_epi32(l4_min_l, l_idx_vec, l4_max_h);
		new_h = _mm512_permutex2var_epi32(l4_min_l, h_idx_vec, l4_max_h);


		print_vector_int(new_l, "L4 first output"); // new output (L) in L4
		print_vector_int(new_h, "L4 second output"); // new output (H) in L4

		/***************** Start L5 ***********************/
	
		__m512i l5_min_l = _mm512_min_epi32(new_l, new_h);
		__m512i l5_max_h = _mm512_max_epi32(new_l, new_h);
	
		int l5_l_idx[] = {0,16,1,17,2,18,3,19,4,20,5,21,6,22,7,23};
		int l5_h_idx[] = {8,24,9,25,10,26,11,27,12,28,13,29,14,30,15,31};	
	

		for(int i = 0; i < BLOCK_SIZE; i++){ // L5 indices for permuting
			l_idx_ptr[i] = l5_l_idx[i];
			h_idx_ptr[i] = l5_h_idx[i];
		}

		l_idx_vec = _mm512_load_si512(&l_idx_ptr[0]);
		h_idx_vec = _mm512_load_si512(&h_idx_ptr[0]);

		new_l = _mm512_permutex2var_epi32(l5_min_l, l_idx_vec, l5_max_h);
		new_h = _mm512_permutex2var_epi32(l5_min_l, h_idx_vec, l5_max_h);

		print_vector_int(new_l, "L5 first output"); // new output (L) in L5
		print_vector_int(new_h, "L5 second output"); // new output (H) in L5

		// Now I have two sorted blocks of 16. Write them out to memory. 
		_mm512_store_si512(arr + out_idx, new_l);
		out_idx += 16;
		_mm512_store_si512(arr + out_idx, new_h);
		out_idx *= 2;

		std::cout << "Block of 32 sorted: \n";    	
		for(int i = 0; i < 32; i++){
			std::cout << arr[i] << " ";
		}
		std::cout << "\n";
	}
	
	for(int i = 0; i < N; i++){
		std::cout << arr[i] << " ";
	}
	std::cout << "\n";
	
// Start of psuedocode implementation
	//__m512 A1in, A2in, B1in, B2in, C1in, C2in, D1in, D2in,A1out, A2out, B1out, B2out, C1out, C2out, D1out, D2out;	
//	int sorted_block_size = 16;
//	int ending_sorted_block_size = 16384;

    return 0;
}
