#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "fred_lib.h"

typedef uint64_t data_t;

// make sure these constants match with the hw design
#define  ARRAY_SIZE (1024)
#define  BLOCK_SIZE_BYTE (ARRAY_SIZE * sizeof(data_t))

data_t *A_in, *B_in, *C_out;

const int hw_id = 100;

void init_vect(data_t * base, data_t value)
{
	for (unsigned int i = 0; i < ARRAY_SIZE; ++i) {
		base[i] = value;
	}
}

void print_vect(data_t *base, unsigned int size)
{
	for (unsigned int i = 0; i < size; ++i) {
		//std::cout << mem_in[base_idx + i] << "\t" << mem_out[base_idx + i] << "\n";
		printf("%d\t%d\n", i, base[i]);
	}
	printf("\n");
	//std::cout << std::endl;
}

uint32_t check_output(data_t *base, unsigned int size, data_t expected_value)
{
	uint32_t sum=0;
	for (unsigned int i = 0; i < size; ++i) {
		if (base[i] != expected_value)
			return 0;
	}
	return 1;
}

int main (int argc, char **argv)
{
	//std::cout << " starting memcpy \n";
	printf(" starting vector nor \n");

	struct fred_data *fred;
	struct fred_hw_task *hw_ip;
	int retval;
	int error_code = 0;
    
	//sw_task_init(&fred);

	retval = fred_init(&fred);
	if (retval) {
		printf("fred_init failed for hw-task %u\n", hw_id);
		error_code = 1;
	}	
	
	// Bind with HW-memcpy having hw-id 100
	retval = fred_bind(fred, &hw_ip, hw_id);
	if (retval) {
		printf("fred_bind failed for hw-task %u\n", hw_id);
		error_code = 1;
	}	

	C_out = fred_map_buff(fred, hw_ip, 0);
	if (!C_out) {
		printf("fred_map_buff failed on buff 0 for C_out\n");
		error_code = 1;
	}	
	A_in  = fred_map_buff(fred, hw_ip, 1);
	if (!A_in) {
		printf("fred_map_buff failed on buff 1 for A_in\n");
		error_code = 1;
	}	
	B_in  = fred_map_buff(fred, hw_ip, 2);
	if (!B_in) {
		printf("fred_map_buff failed on buff 2 for B_in\n");
		error_code = 1;
	}	

	// set the memcpy parameters (dest, source)
	init_vect(A_in, 2);
	init_vect(B_in, 1);

	// Call fred IP
	retval = fred_accel(fred, hw_ip);
	if (retval) {
		printf("fred_accel failed for hw-task %u\n", hw_id);
		error_code = 1;
	}	

	//validate
	if (check_output(C_out, ARRAY_SIZE,~(2|1)) != 1){
		//std::cout << "Mismatch!\n";
		printf("Mismatch!\n");
		printf("Content of A[0:9]:\n");
		print_vect(A_in, 10);
		printf("Content of B[0:9]:\n");
		print_vect(B_in, 10);
		printf("Content of C[0:9]:\n");
		print_vect(C_out, 10);
		error_code = 1;
	}else{
		//std::cout << "Match!\n";
		printf("Match!\n");
	}

	//cleanup and finish
	fred_free(fred);

	//std::cout << "Fred finished\n";
	printf("Fred finished\n");

	return(error_code);
}
