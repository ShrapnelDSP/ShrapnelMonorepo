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

#include "abstract_dsp.h"
#include "delayline.h"
#include <memory>

struct dspal_delayline {
    std::unique_ptr<shrapnel::dsp::DelayLine> delayline;
};

dspal_delayline_t dspal_delayline_create(size_t max_samples)
{
    dspal_delayline_t delayline = new dspal_delayline;

    delayline->delayline.reset(new shrapnel::dsp::DelayLine(max_samples));

    return delayline;
}

void dspal_delayline_destroy(dspal_delayline_t delayline)
{
    delete delayline;
}

void dspal_delayline_set_delay(dspal_delayline_t delayline, float delay)
{
    delayline->delayline->set_delay(delay);
}

void dspal_delayline_push_sample(dspal_delayline_t delayline, float sample)
{
    delayline->delayline->push_sample(sample);
}

float dspal_delayline_pop_sample(dspal_delayline_t delayline)
{
    return delayline->delayline->pop_sample();
}

