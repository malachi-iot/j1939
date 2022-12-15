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
enum addresses
{
    engine1 = 0,
    engine2 = 1,
    turbocharger = 2,
    transmission1 = 3,
    transmission2 = 4,
    power_take_off = 7,
    axle_steering = 8,
    axle_drive1 = 9,
    brakes_system_controller = 11,
    brakes_steer_axle = 12,
    brakes_drive_axle1 = 13,
    brakes_drive_axle2 = 14,
    cruise_control = 17,
    fuel_system = 18,
    steering_controller = 19,
    instrument_cluster1 = 23,
    trip_recorder = 24,
    passenger_operator_climate_control = 25,
    electrical_charging_system = 26,
    vehicle_security = 29,
    electrical_system = 30,
    body_controller = 33,
    off_vehicle_gateway = 37,
    virtuaL_terminal_in_cab = 38,       // [3]
    cab_display1 = 40,                  ///< The first Cab Display - may only be used for the NAME Function of 60, Function Instance 0, and an ecu instance of 0.
    onboard_diagnostic_unit = 43,
    cab_controller_primary = 49,
    tire_pressure_controller = 51,
    ignition_control_module = 52,
    seat_control1 = 54,
    lighting_operator_controls = 55,    ///< "The controller for sending the operator lighting controls messages
                                        ///<  when they are coming from a device dedicated to transmitting
                                        ///<  these specific messages on the network." [2]
    chassis_controller1 = 71,
    chassis_controller2 = 72,
    parking_brake_controller = 80,
    safety_restraint_system = 83,
    cab_display2 = 84,                  // [3]
    power_systems_manager = 91,

    // [2] Table B3 (On Highway Equipment)
    steering_input_unit = 228,
    body_to_vehicle_interface_control = 230,
    tachograph = 238,

    // [2] Table B4 (Agricultural / Forestry Equipment)
    operator_controls_machine_specific = 246,

    // [2] Table B7 (Industrial)
    generator_voltage_regulator = 230,
    generator_set_controller = 234,

    non_virtual_terminal_display = 245, // [1]
    file_server_printer = 248,          // [3]
    on_board_data_logger = 251,         // [3]
    reserved_for_experimental_use = 252,    // [1]
    reserved_for_oem = 253,                 // [1]

    null_address = 254,
    global = 255

};

}}