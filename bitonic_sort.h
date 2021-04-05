#ifndef _BITONIC_SORT_H_
#define _BITONIC_SORT_H_

int reverse_idxs[] = {15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0};

int l1_idx[] = {0,1,2,3,4,5,6,7,16,17,18,19,20,21,22,23};
int h1_idx[] = {8,9,10,11,12,13,14,15,24,25,26,27,28,29,30,31};

int l2_idx[] = {0,1,2,3,16,17,18,19,8,9,10,11,24,25,26,27};
int h2_idx[] = {4,5,6,7,20,21,22,23,12,13,14,15,28,29,30,31};

int l3_idx[] = {0,1,16,17,4,5,20,21,8,9,24,25,12,13,28,29};
int h3_idx[] = {2,3,18,19,6,7,22,23,10,11,26,27,14,15,30,31}; 

int l4_idx[] = {0,16,2,18,4,20,6,22,8,24,10,26,12,28,14,30};
int h4_idx[] = {1,17,3,19,5,21,7,23,9,25,11,27,13,29,15,31};

int l5_idx[] = {0,16,1,17,2,18,3,19,4,20,5,21,6,22,7,23};
int h5_idx[] = {8,24,9,25,10,26,11,27,12,28,13,29,14,30,15,31};


void bitonic_sort(__m512i &A1in, __m512i &A2in, __m512i &B1in, __m512i &B2in, 
		__m512i &C1in, __m512i &C2in, __m512i &D1in, __m512i &D2in, __m512i &A1out, 
		__m512i &A2out, __m512i &B1out, __m512i &B2out,
		__m512i &C1out, __m512i &C2out, __m512i &D1out, __m512i &D2out)
{
	// Reversing the second input vectors
	A2in = _mm512_permutexvar_epi32(_mm512_load_si512(&reverse_idxs[0]), A2in);
	B2in = _mm512_permutexvar_epi32(_mm512_load_si512(&reverse_idxs[0]), B2in);	

	print_vector_int(A1in, "A1in");
    print_vector_int(A2in, "A2in");
 
  //  print_vector_int(B1in, "B1in");
    //  print_vector_int(B2in, "B2in");


//	C2in = _mm512_permutexvar_epi32(_mm512_load_si512(&reverse_idxs[0]), C2in);
//	D2in = _mm512_permutexvar_epi32(_mm512_load_si512(&reverse_idxs[0]), D2in);
	
	// For each bitonic merge level 
	for(int l = 1; l <= 5; l++){
		// Get the mins
		auto minA = _mm512_min_epi32(A1in, A2in);
		auto minB = _mm512_min_epi32(B1in, B2in);
//		auto minC = _mm512_min_epi32(C1in, C2in);
//		auto minD = _mm512_min_epi32(D1in, D2in);
		// Get the maxes
		auto maxA = _mm512_max_epi32(A1in, A2in);
		auto maxB = _mm512_max_epi32(B1in, B2in);
//		auto maxC = _mm512_max_epi32(C1in, C2in);
//		auto maxD = _mm512_max_epi32(D1in, D2in);
		
		// Get the permute indices based on the level
		__m512i l_idx_vec;
		__m512i h_idx_vec;
		if(l == 1){
			l_idx_vec = _mm512_load_si512(&l1_idx[0]);
			h_idx_vec = _mm512_load_si512(&h1_idx[0]);
		}
		else if(l == 2){
			l_idx_vec = _mm512_load_si512(&l2_idx[0]);
			h_idx_vec = _mm512_load_si512(&h2_idx[0]);
		}
		else if(l == 3){
			l_idx_vec = _mm512_load_si512(&l3_idx[0]);
			h_idx_vec = _mm512_load_si512(&h3_idx[0]);
		}
		else if(l == 4){
			l_idx_vec = _mm512_load_si512(&l4_idx[0]);
			h_idx_vec = _mm512_load_si512(&h4_idx[0]);
		}
		else{
			l_idx_vec = _mm512_load_si512(&l5_idx[0]);
			h_idx_vec = _mm512_load_si512(&h5_idx[0]);
		}
		
		// Permute
		A1out = _mm512_permutex2var_epi32(minA, l_idx_vec, maxA);
		B1out = _mm512_permutex2var_epi32(minB, l_idx_vec, maxB);
		print_vector_int(A1out, "A1out contains:");
//		C1out = _mm512_permutex2var_epi32(minC, l_idx_vec, maxC);
//		D1out = _mm512_permutex2var_epi32(minD, l_idx_vec, maxD);
	
		A2out = _mm512_permutex2var_epi32(minA, h_idx_vec, maxA);
		print_vector_int(A2out, "A2out contains");
		B2out = _mm512_permutex2var_epi32(minB, h_idx_vec, maxB);
//		C2out = _mm512_permutex2var_epi32(minC, h_idx_vec, maxC);
//		D2out = _mm512_permutex2var_epi32(minD, h_idx_vec, maxD);

		A1in = A1out;
		B1in = B1out;
//		C1in = C1out;
//		D1in = D1out;
		
		A2in = A2out;
		B2in = B2out;
//		C2in = C2out;
//		D2in = D2out;		

	}	
	return;
} 

#endif
