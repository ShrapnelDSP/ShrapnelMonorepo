/*
    Copyright 2020 Barabas Raffai

    This file is part of Valvestate Sim.

    Valvestate Sim is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the Free
    Software Foundation, either version 3 of the License, or (at your option)
    any later version.

    Valvestate Sim is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License along
    with Valvestate Sim.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "Contour.h"
#include <cmath>

ContourFilter::ContourFilter()
{
    //we must set the filter order before the first prepare call, so that
    //the correct amount of memory gets allocated
    set_parameter(0.5);
}

ContourFilter::~ContourFilter(){}

void ContourFilter::prepare(float a_samplerate)
{
    filter.reset();
    samplerate = a_samplerate;
}

void ContourFilter::set_parameter(float p)
{
    float K = 2*samplerate;

    float B0 = 9.05251759610179e-11f*std::pow(K, 3)*std::pow(p, 2) -
        1.08075708355892e-10f*std::pow(K, 3)*p + 1.13156469951272e-6f*std::pow(K,
                2)*std::pow(p, 2) - 1.15683089695001e-6f*std::pow(K, 2)*p -
        1.89606569211334e-8f*std::pow(K, 2) - 0.000315827467966071f*K*p -
        0.000205739036275041f*K - 0.0902364194188775f;

    float B1 = -1.81050351922036e-10f*std::pow(K, 3)*std::pow(p, 2) +
        2.16151416711785e-10f*std::pow(K, 3)*p - 0.000631654935932142f*K*p -
        0.000411478072550081f*K - 0.36094567767551f;

    float B2 = -2.26312939902545e-6f*std::pow(K, 2)*std::pow(p, 2) +
        2.31366179390002e-6f*std::pow(K, 2)*p + 3.79213138422667e-8f*std::pow(K,
                2) - 0.541418516513265f;

    float B3 = 1.81050351922036e-10f*std::pow(K, 3)*std::pow(p, 2) -
        2.16151416711785e-10f*std::pow(K, 3)*p + 0.000631654935932142f*K*p +
        0.000411478072550081f*K - 0.36094567767551f;

    float B4 = -9.05251759610179e-11f*std::pow(K, 3)*std::pow(p, 2) +
        1.08075708355892e-10f*std::pow(K, 3)*p + 1.13156469951272e-6f*std::pow(K,
                2)*std::pow(p, 2) - 1.15683089695001e-6f*std::pow(K, 2)*p -
        1.89606569211334e-8f*std::pow(K, 2) + 0.000315827467966071f*K*p +
        0.000205739036275041f*K - 0.0902364194188775f;

    float A0 = 3.50824760873489e-15f*std::pow(K, 4)*std::pow(p, 2) -
        4.18840913192565e-15f*std::pow(K, 4)*p +
        1.73715033387475e-10f*std::pow(K, 3)*std::pow(p, 2) -
        2.15704710341094e-10f*std::pow(K, 3)*p -
        4.18840913192564e-12f*std::pow(K, 3) + 2.09000180472839e-6f*std::pow(K,
                2)*std::pow(p, 2) - 2.70050081212777e-6f*std::pow(K, 2)*p -
        1.22402003248511e-7f*std::pow(K, 2) - 0.000583333333333333f*K*p -
        0.00052012272153041f*K - 0.166666666666667f;

    float A1 = -1.40329904349395e-14f*std::pow(K, 4)*std::pow(p, 2) +
        1.67536365277026e-14*std::pow(K, 4)*p - 3.4743006677495e-10*std::pow(K,
                3)*std::pow(p, 2) + 4.31409420682187e-10f*std::pow(K, 3)*p +
        8.37681826385129e-12f*std::pow(K, 3) - 0.00116666666666667f*K*p -
        0.00104024544306082f*K - 0.666666666666667f;

    float A2 = 2.10494856524093e-14f*std::pow(K, 4)*std::pow(p, 2) -
        2.51304547915539e-14f*std::pow(K, 4)*p - 4.18000360945678e-6f*std::pow(K,
                2)*std::pow(p, 2) + 5.40100162425555e-6f*std::pow(K, 2)*p +
        2.44804006497022e-7f*std::pow(K, 2) - 1.0;

    float A3 = -1.40329904349395e-14f*std::pow(K, 4)*std::pow(p, 2) +
        1.67536365277026e-14f*std::pow(K, 4)*p + 3.4743006677495e-10f*std::pow(K,
                3)*std::pow(p, 2) - 4.31409420682187e-10f*std::pow(K, 3)*p -
        8.37681826385129e-12f*std::pow(K, 3) + 0.00116666666666667f*K*p +
        0.00104024544306082f*K - 0.666666666666667f;

    float A4 = 3.50824760873489e-15f*std::pow(K, 4)*std::pow(p, 2) -
        4.18840913192565e-15f*std::pow(K, 4)*p -
        1.73715033387475e-10f*std::pow(K, 3)*std::pow(p, 2) +
        2.15704710341094e-10f*std::pow(K, 3)*p +
        4.18840913192564e-12f*std::pow(K, 3) + 2.09000180472839e-6f*std::pow(K,
                2)*std::pow(p, 2) - 2.70050081212777e-6f*std::pow(K, 2)*p -
        1.22402003248511e-7f*std::pow(K, 2) + 0.000583333333333333f*K*p +
        0.00052012272153041f*K - 0.166666666666667f;

    jassert (A0 != 0);

    filter.set_coefficients(std::array<float, 10>{B0, B1, B2, B3, B4, A0, A1, A2, A3, A4});
}

void ContourFilter::process(float *buffer, std::size_t buffer_size)
{
    filter.process(buffer, buffer, buffer_size);
}

void ContourFilter::reset(void)
{
    filter.reset();
}
