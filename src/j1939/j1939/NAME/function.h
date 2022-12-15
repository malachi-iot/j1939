/**
 * References:
 *
 * 1. J1939 (OCT2007)
 * 2. J1939-81 (MAY2003)
 */
#pragma once

namespace embr { namespace j1939 {

// [1] Table B11 and B12
// [2] 4.1.1.7
enum class function_fields
{
    // [1] Table B11 portion
    engine = 0,
    auxiliary_power_unit,
    electric_propulsion_control,
    transmission,
    battery_pack_monitor,
    brakes_system_controller = 9,
    network_interconnect_ecu = 25,
    virtual_terminal_in_cab = 29,       ///< "J1939-72 or ISO 11783-6" [1]
    system_monitor = 33,
    cab_controller = 37,
    tire_pressure_control,
    lighting_operator_controls = 41,
    information_system_controller = 47,
    alternator_charging_system = 53,
    communications_unit_cellular,
    communications_unit_radio = 56,
    steering_column_unit,
    cab_display = 60,
    file_server_printer,
    on_board_diagnostic_unit,
    io_controller = 66,
    electrical_system_controller,       ///< "Load Centers, Fuseboxes, & Power Distribution boards" [1]

    // [1] Table B12 portion, overlapping

    // IG=0, VehSys=0
    pc_keyboard = 131,
    keypad = 135,

    // IG=1, VehSys=0
    tachograph = 128,
    body_to_vehicle_interface_control = 131,

    // IG=1, VehSys=1 (Tractor)
    adaptive_front_lighting_system = 131,

    // IG=2, VehSys=0
    operator_controls_machine_specific = 129,
    machine_control = 132,

    // IG=2, VehSys=4
    section_on_off_control = 129,

    // IG=4, VehSys=80 (instrumentation/general)
    time_date_systems = 130,
    integrated_instrumentation = 150,
    general_sensor_box = 170,

    // IG=5, VehSys=0
    generator_voltage_regulator = 130,

};

/*!
 * @enum function_field
 * TEST2
 */

/*!
 * @var function_field::steering_column_unit
 * Device that gathers the operator inputs from switches/levers/etc
 * located in and/or around the steering wheel/column and transmits
 * the associated messages on the network., when a separate
 * NAME is needed for this device (i.e. other devices might be
 * sending the messages and this device not exist on the network).
 */


}}