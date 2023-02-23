#include "etl/callback_timer_atomic.h"
#include "os/timer.h"

namespace shrapnel::os {

struct Timer::impl
{
    impl(const char *pcTimerName,
         TickType_t xTimerPeriod,
         const UBaseType_t uxAutoReload,
         std::optional<Callback> callback);

    BaseType_t is_active() const;
    BaseType_t start(TickType_t xBlockTime);
    BaseType_t stop(TickType_t xBlockTime);

    // Advance tick count, firing callbacks for any expired timers

    // TODO how to make this globally accessible? Should we dependency inject
    // something that has a tick method? It seems quite tedious to pass this
    // down to the lowest level code using timers. This is rougly the same
    // problem solved by provider in flutter. Might just have to use a global
    // variable for this.
    static void tick(uint32_t tick_count);

private:
    void do_callback()
    {
        if(callback.has_value())
            (*callback)();
        is_running = false;
    }

    etl::timer::id::type id;
    bool is_running = false;
    std::optional<Callback> callback;
    static etl::callback_timer_atomic<254> timers;
};

} // namespace shrapnel::os