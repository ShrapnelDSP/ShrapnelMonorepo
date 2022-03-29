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

    auto input = std::array {0.f, 0.f};

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

TEST_F(FastFir, SignalIsBufferedCorrectly)
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

    auto input = std::array{
        std::array {10.f, 20.f},
        std::array {30.f, 40.f},
        std::array {50.f, 60.f},
        std::array {70.f, 80.f},
        std::array {90.f, 100.f},
    };

    EXPECT_CALL(convolution, process(_,
                ElementsAre(
                    30.f,
                    40.f,
                    50.f,
                    60.f,
                    70.f,
                    80.f,
                    90.f,
                    100.f), _)).Times(1);

    EXPECT_CALL(convolution, process(_,
                ElementsAre(
                    10.f,
                    20.f,
                    30.f,
                    40.f,
                    50.f,
                    60.f,
                    70.f,
                    80.f), _)).Times(1);

    EXPECT_CALL(convolution, process(_,
                ElementsAre(
                    0.f,
                    0.f,
                    10.f,
                    20.f,
                    30.f,
                    40.f,
                    50.f,
                    60.f), _)).Times(1);

    EXPECT_CALL(convolution, process(_,
                ElementsAre(
                    0.f,
                    0.f,
                    0.f,
                    0.f,
                    10.f,
                    20.f,
                    30.f,
                    40.f), _)).Times(1);

    // TODO this must be called first, right now the order of calls does not
    //      matter at all
    EXPECT_CALL(convolution, process(_,
                ElementsAre(
                    0.f,
                    0.f,
                    0.f,
                    0.f,
                    0.f,
                    0.f,
                    10.f,
                    20.f), _)).Times(1);

    for(auto &in : input)
    {
        uut.process(in.data());
    }
}
