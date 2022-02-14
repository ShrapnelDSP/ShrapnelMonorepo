#include "valvestate.h"

namespace shrapnel {
namespace effect {
namespace valvestate {

Valvestate::Valvestate() : volume(0) {}

void Valvestate::set_gain(float gain, float channel)
{
    gaincontrol.set_parameters(gain, channel);
}

void Valvestate::set_fmv(float bass, float middle, float treble)
{
    fmv.set_parameters(bass, middle, treble);
}

void Valvestate::set_contour(float a_contour)
{
    contour.set_parameter(a_contour);
}

void Valvestate::set_volume(float a_volume)
{
    volume = a_volume;
}

void Valvestate::process(float *buffer, std::size_t buffer_size)
{
    input.process(buffer, buffer_size);
    gaincontrol.process(buffer, buffer_size);
    clipping.process(buffer, buffer_size);
    fmv.process(buffer, buffer_size);
    contour.process(buffer, buffer_size);

    for(std::size_t i = 0; i < buffer_size; i++)
    {
        buffer[i] *= volume;
    }
}

void Valvestate::prepare(float samplerate)
{
    input.prepare(samplerate);
    gaincontrol.prepare(samplerate);
    fmv.prepare(samplerate);
    contour.prepare(samplerate);
}

void Valvestate::reset(void)
{
    input.reset();
    gaincontrol.reset();
    fmv.reset();
    contour.reset();
}

}
}
}
