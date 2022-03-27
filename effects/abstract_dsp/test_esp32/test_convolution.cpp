#include <cstdlib>
#include "unity.h"
#include "dsp_platform.h"

void setUp()
{
    // Use the same sequence of numbers for every test run
    srand(0);
}

#if 0
//extern "C"
TEST_CASE("dsps_conv_f32 benchmark", "[dsps]")
{
    int max_N = 1024;
    int conv_size = 64;

    std::array<float, max_N> x;
    std::array<float, max_N> y;
    std::array<float, max_N * 2 + 1> z;

    for (int i = 0 ; i < max_N ; i++) {
        x[i] = rand();
        y[i] = rand();
    }

    // warm up cache
    dsps_conv_f32(x, max_N, y, conv_size, z.data());

    unsigned int start_ccound = xthal_get_ccount();
    dsps_conv_f32(x, max_N, y, conv_size, z.data());
    unsigned int end_ccount = xthal_get_ccount();

    float cycles = end_b - start_b;
    ESP_LOGI(TAG, "dsps_conv_f32 - %f cycles for signal %i and pattern %i", cycles, max_N, conv_size);
}

TEST_CASE("fast convolve benchmark", "[dsps]")
{
    int max_N = 1024;
    int conv_size = 64;

    // The buffer sizes are different for fast convolve, we need to go to the
    // next power of two, and perform zero padding to get the correct result
    std::array<float, 2*max_N> x;
    std::array<float, 2*max_N> y;
    std::array<float, 2*max_N> z;

    for (int i = 0 ; i < max_N ; i++) {
        x[i] = 0;
        y[i] = 1000;
    }

    FastConvolution<max_N> conv;

    // warm up cache
    conv(x, y, z.data());

    unsigned int start_ccound = xthal_get_ccount();
    dsps_conv_f32(x, max_N, y, conv_size, z.data());
    unsigned int end_ccount = xthal_get_ccount();

    float cycles = end_b - start_b;
    ESP_LOGI(TAG, "dsps_conv_f32 - %f cycles for signal %i and pattern %i", cycles, max_N, conv_size);
}
#endif

