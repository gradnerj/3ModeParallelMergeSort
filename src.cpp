#include "utils.h"

const unsigned int BLOCK_SIZE = 16;

void sort_blocks(int* arr){
    std::sort(arr, arr+BLOCK_SIZE);
	std::sort(arr+BLOCK_SIZE, arr+(BLOCK_SIZE*2));
	std::reverse(arr+BLOCK_SIZE, arr+(BLOCK_SIZE*2));
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
	//sort_blocks(arr2);
	for(int i = 0; i < 32; i++){
		std::cout << a1[i] << " ";
	}
	std::cout << std::endl;
	__m512i first = _mm512_load_si512(&a1[0]);
    __m512i second = _mm512_load_si512(&a1[16]);

	print_vector_int(first, "First");
    print_vector_int(second, "Second");
	
	__m512i l = _mm512_min_epi32(first, second);
	__m512i h = _mm512_max_epi32(first, second);

	print_vector_int(l, "low");
	print_vector_int(h, "high");
	
	int l_idx[] = {0,1,2,3,4,5,6,7,16,17,18,19,20,21,22,23};
	int h_idx[] = {8,9,10,11,12,13,14,15,24,25,26,27,28,29,30,31};

	int *l_idx_ptr = (int*)aligned_alloc(64, sizeof(int)*16);
	int *h_idx_ptr = (int*)aligned_alloc(64, sizeof(int)*16);
	for(int i = 0; i < 16; i++){
		l_idx_ptr[i] = l_idx[i];
		h_idx_ptr[i] = h_idx[i];
	}	
	
	__m512i l_idx_vec = _mm512_load_si512(&l_idx_ptr[0]);
	__m512i h_idx_vec = _mm512_load_si512(&h_idx_ptr[0]);

	__m512i new_l = _mm512_permutex2var_epi32(l,l_idx_vec,h);
	__m512i new_h = _mm512_permutex2var_epi32(l,h_idx_vec,h);
	print_vector_int(new_l, "New L");
	print_vector_int(new_h, "New H");
    return 0;
}
