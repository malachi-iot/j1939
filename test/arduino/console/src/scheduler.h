#include <estd/chrono.h>

#ifdef ESP_PLATFORM

// DEBT: Even though steady clock scheduler by default works for esp,
// the time_point/duration math between std::chrono and estd::chrono falls down,
// particularly in the case of operator +


#include <embr/scheduler.hpp>

using scheduler_impl_type = embr::internal::scheduler::impl::Function<estd::chrono::freertos_clock::time_point>;

using scheduler_type = 
    embr::internal::Scheduler<
    estd::layer1::vector<typename scheduler_impl_type::value_type, 10>,
    scheduler_impl_type>;



#else

// DEBT: Not sure how I'm feeling about it.  I removed steady clock alias a little
// while ago from estd in this context.  Do I miss it?
// FIX: Because of this DEBT, we can't compile on esp32 because its steady_clock
// does exist as an alias to std::system clock
namespace estd { namespace chrono {

using steady_clock = arduino_clock;

}}

#include <embr/platform/arduino/scheduler.h>
#include <embr/scheduler.hpp>

// DEBT: I think scheduler's just about ready to emerge from internal namespace
// ~100 bytes for scheduler on Pro micro
using scheduler_type = embr::internal::layer1::Scheduler<10>;

#endif
