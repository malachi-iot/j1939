#include <estd/chrono.h>

// DEBT: Not sure how I'm feeling about it.  I removed steady clock alias a little
// while ago from estd in this context.  Do I miss it?
namespace estd { namespace chrono {

using steady_clock = arduino_clock;

}}

#include <embr/platform/arduino/scheduler.h>
#include <embr/scheduler.hpp>

// DEBT: I think scheduler's just about ready to emerge from internal namespace
// ~100 bytes for scheduler on Pro micro
using scheduler_type = embr::internal::layer1::Scheduler<10>;

