#include "../ca.h"

// EXPERIMENTAL
// Not sure this wrapper CA is of much value vs emitting commands directly

namespace embr { namespace j1939 { namespace ca { namespace source {

template <class Transport>
struct oel : impl::controller_application<Transport>
{

};

}}}}
