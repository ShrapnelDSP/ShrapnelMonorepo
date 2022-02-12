/*
 * Copyright 2022 Barabas Raffai
 *
 * This file is part of ShrapnelDSP.
 *
 * ShrapnelDSP is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * ShrapnelDSP is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ShrapnelDSP. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "stddef.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Initialise the effect */
int gate_init(void);

/** \brief Reset the state */
void gate_reset(void);

/** \brief Set the buffer size in number of samples */
void gate_set_buffer_size(size_t buffer_size);

/** \brief Set the sample rate in Hz */
void gate_set_sample_rate(float a_sample_rate);

/** \brief Set the threshold and hysteresis value in dBFS
 *
 * When the level is above the threshold, the gate starts opening. When the
 * level falls below the threshold minus hysteresis, the gate starts closing.
 */
void gate_set_threshold(float a_threshold, float a_hysteresis);

/** \brief Set the attack time in milliseconds
 *
 * \note must be called after gate_set_sample_rate
 */
void gate_set_attack(float a_attack);

/** \brief Set the hold time in milliseconds
 *
 * \note must be called after gate_set_sample_rate
 */
void gate_set_hold(float a_hold);

/** \brief Set the release time in milliseconds
 *
 * \note must be called after gate_set_sample_rate
 */
void gate_set_release(float a_release);

/** \brief Analyse the audio
 *
 * This calculates the gain values to use in the gate_process function.
 *
 * \note Make sure to use the same buffer with this function and gate_process.
 */
void gate_analyse(const float *buf, size_t sample_count);

/** \brief Apply the precalculated gain to the audio
 *
 * Applies the gain from gate_analyse function
 *
 * \note Make sure to use the same buffer with this function and gate_analyse.
 */
void gate_process(float *buf, size_t sample_count);

#ifdef __cplusplus
}
#endif
