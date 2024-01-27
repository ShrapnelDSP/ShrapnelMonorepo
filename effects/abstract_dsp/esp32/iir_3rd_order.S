// Direct form II transposed IIR filter
	.text
    .align  4
	.global dspal_esp32_3rd_order_iir
	.type   dspal_esp32_3rd_order_iir,@function

dspal_esp32_3rd_order_iir: 
// esp_err_t dspal_esp32_3rd_order_iir(float *input, float *output, uint32_t len, float *coeffs, float *w);
// input    - a2
// output   - a3
// len      - a4
// coeffs   - a5
// w        - a6

	entry	a1, 16
	
	lsi   f0, a5, 0  // f0 = b0
	lsi   f1, a5, 4  // f1 = b1
	lsi   f2, a5, 8  // f2 = b2
	lsi   f3, a5, 12 // f3 = b3
	lsi   f4, a5, 16 // f4 = -a1
	lsi   f5, a5, 20 // f5 = -a2
	lsi   f6, a5, 24 // f6 = -a3
	
	// Preset output address, loop starts by incrementing it
	addi    a3, a3, -4       // output-- 
	lsi     f9, a2, 0        // f9 = input[i]
	loopnez a4, loop_end
        // out_tmp = (w[0] + b0 * input[i]);
        lsi.s   f15, a6, 0  // out_tmp = w0
        madd.s  f15, f0, f9 // out_tmp += b0 * input[i]
        
// TODO remove, copied from esp-dsp for syntax reference
#if 0
		madd.s  f9, f7, f5   // f9 += -a1*w0
		addi    a3, a3, 4    // out++;
		mul.s   f10, f1, f7  // f10 = b1*w0
		madd.s  f9, f8, f6   // f9 += -a2*w1
		madd.s  f10, f9, f0  // f10 += b0*d0
		addi    a2, a2, 4    // in++;
		madd.s  f10, f2, f8  // f10+= b2*w1, f10 - result
		mov.s   f8, f7       // w1 = w0
		mov.s   f7, f9       // w0 = d0
		lsi     f9, a2,  0   // f9 = x[i]
		ssi	    f10, a3, 0   // y[i] = result
#endif
loop_end:

	movi.n	a2, 0 // return status ESP_OK
	retw.n
