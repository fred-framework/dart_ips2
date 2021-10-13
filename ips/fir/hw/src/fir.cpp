/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
*/

#include "fir.hpp"

#define PERFORMANCE

void fir(volatile data_t *mem_port_in, volatile data_t *mem_port_out)
{
    int j, i;
    data_t in_buff [BUFF_SIZE], out_buff [BUFF_SIZE];
    //Shift registers
    int shift_reg[FIR_WINDOW_SIZE];
#ifdef PERFORMANCE
    #pragma HLS ARRAY_PARTITION variable = shift_reg dim = 1 complete
#endif
	// it seems that this loop could be avoided by using the reset pragma. TODO: confirm it 
	#pragma HLS reset variable=shift_reg
    /*
    reset_shift_reg:for (j = 0; j < FIR_WINDOW_SIZE; j++) {
		#pragma HLS pipeline
        shift_reg[j] = 0;
    }
    */

    //fetch data
    fetch_loop:for (unsigned i = 0; i < BUFF_SIZE; i++) {
        #pragma HLS pipeline
        in_buff[i] = mem_port_in[i];        
    }

    //FIR coeff
    int coeff[FIR_WINDOW_SIZE] = {
        13, -2, 9, 11, 26, 18, 95, -43, 6, 74, 13, -2, 9, 11, 26, 18, 95, -43, 6, 74, 26, 18, 95, -43, 6,
        13, -2, 9, 11, 26, 18, 95, -43, 6, 74, 13, -2, 9, 11, 26, 18, 95, -43, 6, 74, 26, 18, 95, -43, 6,
        13, -2, 9, 11, 26, 18, 95, -43, 6, 74, 13, -2, 9, 11, 26, 18, 95, -43, 6, 74, 26, 18, 95, -43, 6,
        13, -2, 9, 11, 26, 18, 95, -43, 6, 74, 13, -2, 9, 11, 26, 18, 95, -43, 6, 74, 26, 18, 95, -43, 6
        };
    //int coeff[FIR_WINDOW_SIZE] = {13, -2, 9, 11, 26, 18, 95, -43, 6, 74};

    // loop through each output
    main_loop:for (i = 0; i < BUFF_SIZE; i++ ) {
        #pragma HLS pipeline
        int acc = 0;

        // shift registers
        shift_reg:for (j = FIR_WINDOW_SIZE - 1; j > 0; j--) {
            //#pragma HLS pipeline
            shift_reg[j] = shift_reg[j -1];
        }

        // put the new input value into the first register
        shift_reg[0] = in_buff[i];

        // do multiply-accumulate operation
        mac:for (j = 0; j < FIR_WINDOW_SIZE; j++) {
            //#pragma HLS pipeline
            acc += shift_reg[j] * coeff[j];
        }
        out_buff[i] = acc;
    }
    
    //store
    store_loop:for (unsigned i = 0; i < BUFF_SIZE; i++) {
        #pragma HLS pipeline
        mem_port_out[i] = out_buff[i];
    }
}
