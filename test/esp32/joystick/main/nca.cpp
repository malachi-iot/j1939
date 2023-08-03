#include "nca.h"

using proto_name = embr::j1939::layer0::NAME<true,
    embr::j1939::industry_groups::process_control,
    embr::j1939::vehicle_systems::ig5_not_available, // DEBT: Change to a better IG/Veh Sys,
    embr::j1939::function_fields::ig5_not_available>;

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