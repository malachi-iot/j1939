#include "nca.h"

using proto_name = embr::j1939::layer0::NAME<true,
    embr::j1939::industry_groups::construction,
    embr::j1939::vehicle_systems::non_specific,
    embr::j1939::function_fields::joystick_control>;

static scheduler_type scheduler;

nca_type nca(proto_name::sparse(1, 0, 0), scheduler);

void nca_init(transport_type& t)
{
    // DEBT: Can't init this name with proto_name::sparse.
    // Not important for this test app, but important overall
    //static embr::j1939::layer1::NAME name;

    // Kicks off its own FreeRTOS task.
    scheduler.start();

    nca.start(t);
}