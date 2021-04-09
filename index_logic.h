
void n_is_odd(int sorted_block_size, int end_block_size, int start_idx, int end_idx, int* arr, int * output){

	__m512i A1in, A2in, B1in, B2in, C1in, C2in, D1in, D2in,A1out, A2out, B1out, B2out, C1out, C2out, D1out, D2out;
	while(sorted_block_size <= end_block_size){
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
					bitonic_sort(A1in, A2in, B1in, B2in, C1in, C2in, D1in, D2in, A1out, A2out, B1out, 
									B2out, C1out, C2out, D1out, D2out);
				
					// Write first output to memory
					_mm512_store_si512(output+write_A, A1out);
					_mm512_store_si512(output+write_B, B1out);
					_mm512_store_si512(output+write_C, C1out);
					_mm512_store_si512(output+write_D, D1out);

					// Increment write indices
					write_A += 16;
					write_B += 16;
					write_C += 16;
					write_D += 16;
				
					if(j == (sorted_block_size / 8)-2){ // if on the last iteration, write second outputs to memory
						_mm512_store_si512(output+write_A, A2out);
						_mm512_store_si512(output+write_B, B2out);
						_mm512_store_si512(output+write_C, C2out);
						_mm512_store_si512(output+write_D, D2out);
						write_A += 16;
						write_B += 16;
						write_C += 16;
						write_D += 16;
					}
					else{ // Determine the second input for the next iteration

						// Reuse second outputs	
						A1in = A2out;
						B1in = B2out;
						C1in = C2out;
			    		D1in = D2out;	
						if(stA1+16 == eA1){
							stA2 += 16;
							A2in = _mm512_load_si512(&arr[stA2]);
						}else if (stA2+16 == eA2){
							stA1 += 16;
							A2in = _mm512_load_si512(&arr[stA1]);
						}else if (arr[stA1 + 16] < arr[stA2 + 16]){
							stA1 += 16;
							A2in = _mm512_load_si512(&arr[stA1]);
						}else{
							stA2 +=16;
							A2in = _mm512_load_si512(&arr[stA2]);
						}
					
						if(stB1+16 == eB1){
							stB2 += 16;
							B2in = _mm512_load_si512(&arr[stB2]);
						}else if (stB2+16 == eB2){
							stB1 += 16;
							B2in = _mm512_load_si512(&arr[stB1]);
						}else if (arr[stB1 + 16] < arr[stB2 + 16]){
							stB1 += 16;
							B2in = _mm512_load_si512(&arr[stB1]);
						}else{
							stB2 +=16;
							B2in = _mm512_load_si512(&arr[stB2]);
						}
					
						if(stC1+16 == eC1){
							stC2 += 16;
							C2in = _mm512_load_si512(&arr[stC2]);
						}else if (stC2+16 == eC2){
							stC1 += 16;
							C2in = _mm512_load_si512(&arr[stC1]);
						}else if (arr[stC1 + 16] < arr[stC2 + 16]){
							stC1 += 16;
							C2in = _mm512_load_si512(&arr[stC1]);
						}else{
							stC2 +=16;
							C2in = _mm512_load_si512(&arr[stC2]);
						}

						if(stD1+16 == eD1){
							stD2 += 16;
							D2in = _mm512_load_si512(&arr[stD2]);
						}else if (stD2+16 == eD2){
							stD1 += 16;
							D2in = _mm512_load_si512(&arr[stD1]);
						}else if (arr[stD1 + 16] < arr[stD2 + 16]){
							stD1 += 16;
							D2in = _mm512_load_si512(&arr[stD1]);
						}else{
							stD2 +=16;
							D2in = _mm512_load_si512(&arr[stD2]);
						} 
					}	
				}		
			}
			std::swap(arr, output);
			sorted_block_size = sorted_block_size * 2;
	}
}

void n_is_even(int sorted_block_size, int end_block_size, int start_idx, int end_idx, int* arr, int * output){

		__m512i A1in, A2in, B1in, B2in, C1in, C2in, D1in, D2in,A1out, A2out, B1out, B2out, C1out, C2out, D1out, D2out;
		while(sorted_block_size < end_block_size){
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
					bitonic_sort(A1in, A2in, B1in, B2in, C1in, C2in, D1in, D2in, A1out, A2out, B1out, 
									B2out, C1out, C2out, D1out, D2out);
				
					// Write first output to memory
					_mm512_store_si512(output+write_A, A1out);
					_mm512_store_si512(output+write_B, B1out);
					_mm512_store_si512(output+write_C, C1out);
					_mm512_store_si512(output+write_D, D1out);

					// Increment write indices
					write_A += 16;
					write_B += 16;
					write_C += 16;
					write_D += 16;
				
					if(j == (sorted_block_size / 8)-2){ // if on the last iteration, write second outputs to memory
						_mm512_store_si512(output+write_A, A2out);
						_mm512_store_si512(output+write_B, B2out);
						_mm512_store_si512(output+write_C, C2out);
						_mm512_store_si512(output+write_D, D2out);
						write_A += 16;
						write_B += 16;
						write_C += 16;
						write_D += 16;
					}
					else{ // Determine the second input for the next iteration

						// Reuse second outputs	
						A1in = A2out;
						B1in = B2out;
						C1in = C2out;
			    		D1in = D2out;	
						if(stA1+16 == eA1){
							stA2 += 16;
							A2in = _mm512_load_si512(&arr[stA2]);
						}else if (stA2+16 == eA2){
							stA1 += 16;
							A2in = _mm512_load_si512(&arr[stA1]);
						}else if (arr[stA1 + 16] < arr[stA2 + 16]){
							stA1 += 16;
							A2in = _mm512_load_si512(&arr[stA1]);
						}else{
							stA2 +=16;
							A2in = _mm512_load_si512(&arr[stA2]);
						}
					
						if(stB1+16 == eB1){
							stB2 += 16;
							B2in = _mm512_load_si512(&arr[stB2]);
						}else if (stB2+16 == eB2){
							stB1 += 16;
							B2in = _mm512_load_si512(&arr[stB1]);
						}else if (arr[stB1 + 16] < arr[stB2 + 16]){
							stB1 += 16;
							B2in = _mm512_load_si512(&arr[stB1]);
						}else{
							stB2 +=16;
							B2in = _mm512_load_si512(&arr[stB2]);
						}
					
						if(stC1+16 == eC1){
							stC2 += 16;
							C2in = _mm512_load_si512(&arr[stC2]);
						}else if (stC2+16 == eC2){
							stC1 += 16;
							C2in = _mm512_load_si512(&arr[stC1]);
						}else if (arr[stC1 + 16] < arr[stC2 + 16]){
							stC1 += 16;
							C2in = _mm512_load_si512(&arr[stC1]);
						}else{
							stC2 +=16;
							C2in = _mm512_load_si512(&arr[stC2]);
						}

						if(stD1+16 == eD1){
							stD2 += 16;
							D2in = _mm512_load_si512(&arr[stD2]);
						}else if (stD2+16 == eD2){
							stD1 += 16;
							D2in = _mm512_load_si512(&arr[stD1]);
						}else if (arr[stD1 + 16] < arr[stD2 + 16]){
							stD1 += 16;
							D2in = _mm512_load_si512(&arr[stD1]);
						}else{
							stD2 +=16;
							D2in = _mm512_load_si512(&arr[stD2]);
						} 
					}	
				}		
			}
			std::swap(arr, output);
			sorted_block_size = sorted_block_size * 2;
	}
}
