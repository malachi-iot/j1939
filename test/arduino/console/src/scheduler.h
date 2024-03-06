#include <estd/chrono.h>

#include <embr/scheduler.hpp>

#ifdef ESP_PLATFORM

// DEBT: Even though steady clock scheduler by default works for esp,
// the time_point/duration math between std::chrono and estd::chrono falls down,
// particularly in the case of operator +
// NOTE: Likely ESP is happy with arduino_clock too, but I just like using
// the freertos clock in this circumstance


using scheduler_impl_type =
    embr::internal::scheduler::impl::Function<estd::chrono::freertos_clock::time_point>;

#else

using scheduler_impl_type =
    embr::internal::scheduler::impl::Function<estd::chrono::arduino_clock::time_point>;

#endif

using scheduler_type = 
    embr::internal::Scheduler<
    estd::layer1::vector<typename scheduler_impl_type::value_type, 10>,
    scheduler_impl_type>;

