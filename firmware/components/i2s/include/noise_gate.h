#pragma once


#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"

/** \brief Initialise the gate
 *
 * This must be called before the other functions.
 */
esp_err_t gate_init(size_t a_buf_size, float a_threshold, float
        a_hysteresis, float attack_ms, float release_ms, float hold_ms, float
        samplerate);

/** \brief Set the threshold value
 *
 * When the level is above the threshold the gate opens.
 */
void gate_set_threshold(float a_threshold);

/** \brief Analyse the audio
 *
 * This calculates the gain values to use in the gate_process function.
 *
 * \note Make sure to use the same buffer with this function and gate_process.
 */
void gate_analyse(const float *buf);

/** \brief Apply the precalculated gain to the audio
 *
 * Applies the gain from gate_analyse function
 *
 * \note Make sure to use the same buffer with this function and gate_analyse.
 */
void gate_process(float *buf);

#ifdef __cplusplus
}
#endif
