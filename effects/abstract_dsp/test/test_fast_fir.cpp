#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "fast_fir.h"

using ::testing::ElementsAre;
using ::testing::FloatEq;
using ::testing::_;

class FastFir : public ::testing::Test {};


template <size_t N>
class MockFastConvolution final {
    public:
    MOCK_METHOD(void, process, ((const std::array<float, N> &a),
                                (const std::array<float, N> &b),
                                (std::array<float, N> &out)), ());

};

TEST_F(FastFir, CoefficientsCopiedAndPaddedCorrectly)
{
    MockFastConvolution<8> convolution;
    std::array coefficients{
            1.f,
            2.f,
            3.f,
            4.f,
    };

    shrapnel::dsp::FastFir<2, 8, 4, MockFastConvolution<8>> uut(
            coefficients,
            convolution);

    auto input = std::array {0.f, 0.f, 0.f, 0.f};

    // expect that the coefficients are passed to the convolution engine in the
    // first input, and that they have been right padded with zeros.
    EXPECT_CALL(convolution, process(
                ElementsAre(
                    1.f,
                    2.f,
                    3.f,
                    4.f,
                    0.f,
                    0.f,
                    0.f,
                    0.f) , _, _)).Times(1);

    uut.process(input.data());
}
