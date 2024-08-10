#include "messages.h"
#include "os/queue.h"
#include <juce_core/juce_core.h>

namespace shrapnel {

class Server final : private juce::Thread
{
public:
    Server(etl::delegate<void(const std::pair<ApiMessage, int> &in,
                              uint32_t time_to_wait)> output_message);

    void start();

    void send_message(const std::pair<ApiMessage, std::optional<int>> &message);

private:
    void run() override;

    etl::delegate<void(const std::pair<ApiMessage, int> &in,
                       uint32_t time_to_wait)>
        output_message;
};

} // namespace shrapnel
