#pragma once

#include "abstract_dsp.h"

namespace shrapnel {
namespace effect {

class Chorus {
    public:
    Chorus();

    /** Set the rate of modulation
     *
     * This parameter is in units of Hz.
     */
    void set_modulation_rate_hz(float rate);

    /** Set the depth of modulation
     *
     * This parameter is normalised to the range [0, 1].
     */
    void set_modulation_depth(float depth);

    void set_sample_rate(float rate);

    /** Set the mix ratio of modulation
     *
     * This parameter is normalised to the range [0, 1].
     */
    void set_modulation_mix(float depth);

    void process(float *samples, int sample_count);

    private:
    float modulation_rate = 0;
    float modulation_depth = 0;
    float modulation_mix = 0;

    float sample_rate = 0;

    float phase = 0;

    dspal_delayline_t delayline = nullptr;
};

}
}
