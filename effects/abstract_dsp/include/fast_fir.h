/** \file
 *
 * Fast FIR filter implemented using FFT
 */

#pragma once

#include <stddef.h>

/** Initialise the filter
 *
 * \param[in] coefficients A buffer containing the filter coefficients
 * \param     coefficients_length The number of coefficients
 * \param[in] buffer A buffer used internally
 * \param     buffer_length The number of samples in \p buffer
 * \param     block_length The number of samples in input blocks
 */
void dspal_fast_fir_init(float *coefficients, size_t coefficients_length,
                         float *buffer, size_t buffer_length,
                         size_t block_length);

/** Filter samples
 *
 * \param[in,out] buffer Samples to process
 */
void dspal_fast_fir_process(float *buffer);

/** Reset the state of the filter
 *
 * \note Does not reset coefficients
 */
void dspal_fast_fir_reset(void);
