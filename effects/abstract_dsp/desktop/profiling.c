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

#include "profiling.h"

void profiling_init(size_t buffer_size, float sample_rate) {
    (void) buffer_size;
    (void) sample_rate;
}
void profiling_start(void) {}
void profiling_mark_stage(unsigned int stage) {
    (void) stage;
}
void profiling_stop(void) {}

void profiling_task(void *param) {
    (void) param;
}
