// Direct form II transposed IIR filter
	.text
    .align  4
	.global dspal_esp32_4th_order_iir
	.type   dspal_esp32_4th_order_iir,@function

dspal_esp32_4th_order_iir: 
// void dspal_esp32_4th_order_iir(float *input, float *output, uint32_t len, float *coeffs, float *w);
// input    - a2
// output   - a3
// len      - a4
// coeffs   - a5
// w        - a6

	entry	a1, 16
	
	lsi     f0, a5, 0  // f0 = b0
	lsi     f1, a5, 4  // f1 = b1
	lsi     f2, a5, 8  // f2 = b2
	lsi     f3, a5, 12 // f3 = b3
	lsi     f4, a5, 16 // f4 = b4
	lsi     f5, a5, 20 // f5 = -a1
	lsi     f6, a5, 24 // f6 = -a2
	lsi     f7, a5, 28 // f7 = -a3
	lsi     f8, a5, 32 // f8 = -a4
	
	lsi     f14, a6, 0 // f14 = w0
	lsi     f13, a6, 4 // f13 = w1
	lsi     f12, a6, 8 // f12 = w2
	lsi     f11, a6, 12 // f11 = w3
	
	// Preset output address, loop starts by incrementing it
	addi    a3, a3, -4       // output-- 
	lsi     f9, a2, 0        // f9 = input[0]
	loopnez a4, loop_end
	    // TODO look into store with increment
	    // output++
	    addi a3, a3, 4
	
        // out_tmp = (w[0] + b0 * input[i]);
        mov.s   f15, f14    // out_tmp = w[0]
        madd.s  f15, f0, f9 // out_tmp += b0 * input[i]
        
        /* manually unrolled loop follows: */
        /* unrolled iteration 0 */
        
        // TODO chain of madd, add might be pipeline stall?
        // result depends on previous value which is still in the pipe
        
        /* update the state */
        // w[0] = -a[1] * out_tmp;
        mul.s   f14, f5, f15
        // w[0] += b[1] * input[i];
        madd.s  f14, f1, f9
        // w[0] += w[1];
        add.s   f14, f14, f13
        
        /* unrolled iteration 1 */
        
        // w[1] = -a[2] * out_tmp;
        mul.s   f13, f6, f15
        // w[1] += b[2] * input[i];
        madd.s  f13, f2, f9
        // w[1] += w[2];
        add.s   f13, f13, f12
        
        /* unrolled iteration 2 */
        
        // w[2] = -a[3] * out_tmp;
        mul.s   f12, f7, f15
        // w[2] += b[3] * input[i];
        madd.s  f12, f3, f9
        // w[2] += w[3];
        add.s   f12, f12, f11
        
        /* unrolled iteration 3 */
        /* for the final stage, there is no delay element to add */
        
        // w[3] = -a[4] * out_tmp;
        mul.s f11, f8, f15
        // w[3] += b[4] * input[i];
        madd.s f11, f4, f9
        
        /* finally, write the output */
        // *output = out_tmp;
		ssi	    f15, a3, 0 
		
		addi    a2, a2, 4   // input++;
		lsi     f9, a2, 0   // f9 = *input 
loop_end:
    // write out delay line
	ssi     f14, a6, 0
	ssi     f13, a6, 4
	ssi     f12, a6, 8
	ssi     f11, a6, 12 

	movi.n	a2, 0
	retw.n
