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

#pragma once

//waveshape extrated from spice simulation
class WaveShape
{
public:
    static inline __attribute__((always_inline)) float waveshape(float input)
    {
        float output;

        //calculate which sample of shape the input sample corresponds to
        //the first sample is -1, last is 1
        float index = (input + 1.0f) * 0.5f * (shape_size_less_one);

        //if input is out of range, hard clip
        if(index <= 0)
        {
            output = shape[0];
        }
        else if((int)index + 1 > shape_size_less_one)
        {
            output = shape[shape_size_less_one];
        }
        else
        {
            // linear interpolation between two nearest elements in shape.
            // conversion to int rounds down
            float ratio = index - ((int)index);
            int index_int = (int)index;
            float shape_left = shape[index_int];
            output = shape_left + ratio * (shape[index_int + 1] - shape_left);
        }

        // TODO preprocess the shape to have normalised range and no DC
        return output;
    }

private:
    static constexpr float shape[100] = {
        260.874,     260.7859598, 260.6953072, 260.601432,  260.5041537,
        260.4031856, 260.2985207, 260.1896056, 260.076319,  259.9582966,
        259.8351546, 259.7065092, 259.5718914, 259.4308215, 259.2828199,
        259.1270814, 258.9629027, 258.7895337, 258.6060523, 258.4113474,
        258.2040441, 257.9827768, 257.7457712, 257.4909677, 257.215841,
        256.9175018, 256.5922455, 256.2355875, 255.8419352, 255.4041851,
        254.9131986, 254.3570307, 253.7197414, 252.9797121, 252.1071154,
        251.0601723, 249.8186298, 248.2374011, 246.2379769, 243.6984809,
        240.5224287, 236.6969109, 232.3051762, 227.4657723, 222.2539886,
        216.6160464, 210.1628519, 202.9424278, 195.5356801, 188.0209338,
        180.4120106, 172.7204129, 164.9578739, 157.138341,  149.2820719,
        141.4227325, 133.6224484, 125.9975098, 118.7442167, 112.1250619,
        106.3720184, 101.573245,  97.7060744,  94.80219447, 92.84731645,
        91.5876897,  90.69125858, 90.0247106,  89.50066273, 89.07172653,
        88.71016277, 88.39855163, 88.12531192, 87.88239154, 87.66399733,
        87.46582419, 87.28474325, 87.11794571, 86.96342626, 86.81954855,
        86.68498918, 86.55863586, 86.4395753,  86.32702159, 86.22032926,
        86.11890837, 86.02227577, 85.93001157, 85.84172799, 85.75711414,
        85.67586468, 85.5977274,  85.52247784, 85.44990292, 85.37981384,
        85.31205745, 85.24646844, 85.18291616, 85.12127864, 85.06124,
    };

    static constexpr int shape_size = sizeof(shape) / sizeof(shape[0]);
    static constexpr int shape_size_less_one = shape_size - 1;
};
