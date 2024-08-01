#include "messages.h"
#include "os/queue.h"
#include <juce_core/juce_core.h>

namespace shrapnel {

class Server final : private juce::Thread
{
public:
    Server(shrapnel::QueueBase<AppMessage> *in_queue,
           shrapnel::QueueBase<AppMessage> *out_queue);

    void start();

    void send_message(const AppMessage &message);

private:
    void run() override;

    QueueBase<AppMessage> *in_queue;
    QueueBase<AppMessage> *out_queue;
};

} // namespace shrapnel
