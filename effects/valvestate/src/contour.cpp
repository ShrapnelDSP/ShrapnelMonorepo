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
 *
 */

#include "contour.h"
#include <cmath>
#include <cassert>

ContourFilter::ContourFilter()
{
    //we must set the filter order before the first prepare call, so that
    //the correct amount of memory gets allocated
    set_parameter(0.5);
}

void ContourFilter::prepare(float a_samplerate, size_t)
{
    filter.reset();
    samplerate = a_samplerate;
}

void ContourFilter::set_parameter(float p)
{
    float K = 2*samplerate;

    float B0 = 9.05251759610179e-11f*std::pow(K, 3.f)*std::pow(p, 2.f) -
        1.08075708355892e-10f*std::pow(K, 3.f)*p + 1.13156469951272e-6f*std::pow(K,
                2.f)*std::pow(p, 2.f) - 1.15683089695001e-6f*std::pow(K, 2.f)*p -
        1.89606569211334e-8f*std::pow(K, 2.f) - 0.000315827467966071f*K*p -
        0.000205739036275041f*K - 0.0902364194188775f;

    float B1 = -1.81050351922036e-10f*std::pow(K, 3.f)*std::pow(p, 2.f) +
        2.16151416711785e-10f*std::pow(K, 3.f)*p - 0.000631654935932142f*K*p -
        0.000411478072550081f*K - 0.36094567767551f;

    float B2 = -2.26312939902545e-6f*std::pow(K, 2.f)*std::pow(p, 2.f) +
        2.31366179390002e-6f*std::pow(K, 2.f)*p + 3.79213138422667e-8f*std::pow(K,
                2.f) - 0.541418516513265f;

    float B3 = 1.81050351922036e-10f*std::pow(K, 3.f)*std::pow(p, 2.f) -
        2.16151416711785e-10f*std::pow(K, 3.f)*p + 0.000631654935932142f*K*p +
        0.000411478072550081f*K - 0.36094567767551f;

    float B4 = -9.05251759610179e-11f*std::pow(K, 3.f)*std::pow(p, 2.f) +
        1.08075708355892e-10f*std::pow(K, 3.f)*p + 1.13156469951272e-6f*std::pow(K,
                2.f)*std::pow(p, 2.f) - 1.15683089695001e-6f*std::pow(K, 2.f)*p -
        1.89606569211334e-8f*std::pow(K, 2.f) + 0.000315827467966071f*K*p +
        0.000205739036275041f*K - 0.0902364194188775f;

    float A0 = 3.50824760873489e-15f*std::pow(K, 4.f)*std::pow(p, 2.f) -
        4.18840913192565e-15f*std::pow(K, 4.f)*p +
        1.73715033387475e-10f*std::pow(K, 3.f)*std::pow(p, 2.f) -
        2.15704710341094e-10f*std::pow(K, 3.f)*p -
        4.18840913192564e-12f*std::pow(K, 3.f) + 2.09000180472839e-6f*std::pow(K,
                2.f)*std::pow(p, 2.f) - 2.70050081212777e-6f*std::pow(K, 2.f)*p -
        1.22402003248511e-7f*std::pow(K, 2.f) - 0.000583333333333333f*K*p -
        0.00052012272153041f*K - 0.166666666666667f;

    float A1 = -1.40329904349395e-14f*std::pow(K, 4.f)*std::pow(p, 2.f) +
        1.67536365277026e-14f*std::pow(K, 4.f)*p - 3.4743006677495e-10f*std::pow(K,
                3.f)*std::pow(p, 2.f) + 4.31409420682187e-10f*std::pow(K, 3.f)*p +
        8.37681826385129e-12f*std::pow(K, 3.f) - 0.00116666666666667f*K*p -
        0.00104024544306082f*K - 0.666666666666667f;

    float A2 = 2.10494856524093e-14f*std::pow(K, 4.f)*std::pow(p, 2.f) -
        2.51304547915539e-14f*std::pow(K, 4.f)*p - 4.18000360945678e-6f*std::pow(K,
                2.f)*std::pow(p, 2.f) + 5.40100162425555e-6f*std::pow(K, 2.f)*p +
        2.44804006497022e-7f*std::pow(K, 2.f) - 1.0f;

    float A3 = -1.40329904349395e-14f*std::pow(K, 4.f)*std::pow(p, 2.f) +
        1.67536365277026e-14f*std::pow(K, 4.f)*p + 3.4743006677495e-10f*std::pow(K,
                3.f)*std::pow(p, 2.f) - 4.31409420682187e-10f*std::pow(K, 3.f)*p -
        8.37681826385129e-12f*std::pow(K, 3.f) + 0.00116666666666667f*K*p +
        0.00104024544306082f*K - 0.666666666666667f;

    float A4 = 3.50824760873489e-15f*std::pow(K, 4.f)*std::pow(p, 2.f) -
        4.18840913192565e-15f*std::pow(K, 4.f)*p -
        1.73715033387475e-10f*std::pow(K, 3.f)*std::pow(p, 2.f) +
        2.15704710341094e-10f*std::pow(K, 3.f)*p +
        4.18840913192564e-12f*std::pow(K, 3.f) + 2.09000180472839e-6f*std::pow(K,
                2.f)*std::pow(p, 2.f) - 2.70050081212777e-6f*std::pow(K, 2.f)*p -
        1.22402003248511e-7f*std::pow(K, 2.f) + 0.000583333333333333f*K*p +
        0.00052012272153041f*K - 0.166666666666667f;

    assert (A0 != 0);

    filter.set_coefficients(std::array<float, 10>{B0, B1, B2, B3, B4, A0, A1, A2, A3, A4});
}

void ContourFilter::process(std::span<float> buffer)
{
    filter.process(buffer.data(), buffer.data(), buffer.size());
}

void ContourFilter::reset()
{
    filter.reset();
}
