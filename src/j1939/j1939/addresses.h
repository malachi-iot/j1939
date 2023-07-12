/**
 * 
 * References:
 *
 * 1. https://www.isobus.net/isobus/sourceAddress
 * 2. J1939 (2007)
 * 3. https://github.com/wireshark/wireshark/blob/496dc0fcb37f7b1d5952d3eab49c47621ea7cbef/epan/dissectors/packet-j1939.c
 */
#pragma once

namespace embr { namespace j1939 {

// All from [1], [2]
// See [2] Table B2 for full descriptions
// "Preferred Addresses Industry Group #0 - Global"
enum addresses
{
    // NAME Function 0 (engine)
    engine1 = 0,
    engine2 = 1,
    turbocharger = 2,

    // NAME Function 3 (transmission)
    transmission1 = 3,
    transmission2 = 4,

    // NAME Function 6
    power_take_off = 7,

    // NAME Function 7
    axle_steering = 8,

    // NAME Function 8
    axle_drive1 = 9,

    // NAME Function 9 (brakes system controller)
    brakes_system_controller = 11,

    // NAME Function 10
    brakes_steer_axle = 12,

    // NAME Function 11
    brakes_drive_axle1 = 13,
    brakes_drive_axle2 = 14,

    // NAME Function 14
    cruise_control = 17,

    // NAME Function 15
    fuel_system = 18,

    // NAME Function 16
    steering_controller = 19,

    // NAME Function 19
    instrument_cluster1 = 23,

    // NAME Function 20
    trip_recorder = 24,

    // NAME Function 21 (cab climate control)
    passenger_operator_climate_control = 25,

    // NAME Function 53
    electrical_charging_system = 26,

    // NAME Function 24
    vehicle_security = 29,

    // NAME Function 67 (electrical_system_controller)
    electrical_system = 30,

    // NAME Function 26 (body controller)
    body_controller = 33,

    // NAME Function 28
    off_vehicle_gateway = 37,

    // NAME Function 29 (virtual_terminal_in_cab)
    virtuaL_terminal_in_cab = 38,       // [3]

    // NAME Function 60
    cab_display1 = 40,                  ///< The first Cab Display - may only be used for the NAME Function of 60, Function Instance 0, and an ecu instance of 0.

    // NAME Function 62 (on_board_diagnostic_unit)
    onboard_diagnostic_unit = 43,

    // NAME Function 37 (cab_controller)
    cab_controller_primary = 49,
    cab_controller_secondary,

    // NAME Function 38 (tire_pressure_control)
    tire_pressure_controller = 51,

    // NAME Function 39
    ignition_control_module = 52,

    // NAME Function 40
    seat_control1 = 54,

    // NAME Function 41 (lighting_operator_controls)
    lighting_operator_controls = 55,    ///< "The controller for sending the operator lighting controls messages
                                        ///<  when they are coming from a device dedicated to transmitting
                                        ///<  these specific messages on the network." [2]

    // NAME Function 21 (cab_climate_control)
    passenger_operator_climate_control_2 = 58,

    // NAME Function 52
    chassis_controller1 = 71,
    chassis_controller2 = 72,

    // NAME Function 9 (brakes_system_controller)
    parking_brake_controller = 80,

    // NAME Function ??
    safety_restraint_system = 83,

    // NAME Function ??
    cab_display2 = 84,                  // [3]

    // NAME Function ??
    power_systems_manager = 91,

    // [2] Table B3 (On Highway Equipment)
    // ------------
    steering_input_unit = 228,
    body_to_vehicle_interface_control = 230,
    tachograph = 238,
    battery_pack_monitor_1 = 243,
    auxiliary_power_unit = 247,

    // [2] Table B4 (ig2: Agricultural / Forestry Equipment)
    // ------------
    non_virtual_terminal_display = 245, // [1]
    operator_controls_machine_specific = 246,

    // [2] Table B5 (ig3: Construction Equipment)
    rotation_sensor = 224,

    // [2] Table B7 (ig5: Industrial)
    // ------------
    generator_voltage_regulator = 230,
    generator_set_controller = 234,
    supplemental_sensor_processing_unit_1,
    supplemental_sensor_processing_unit_2,
    supplemental_sensor_processing_unit_3,

    // Back to global
    // -----------
    file_server_printer = 248,          // [3]
    on_board_data_logger = 251,         // [3]
    reserved_for_experimental_use = 252,    // [1]
    reserved_for_oem = 253,                 // [1]

    null_address = 254,
    global = 255

};

}}