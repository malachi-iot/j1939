/**
 *
 * References:
 *
 * 1. https://www.csselectronics.com/pages/j1939-explained-simple-intro-tutorial
 * 2. https://copperhilltech.com/blog/sae-j1939-message-format-and-interpretation-of-pgns/ 
 * 3. https://static.nhtsa.gov/odi/tsbs/2018/MC-10141869-9999.pdf 
 * 4. https://github.com/DanielMartensson/Open-SAE-J1939 
 * 5. https://www.isobus.net/isobus/pGNAndSPN 
 * 6. https://lnx.numeralkod.com/wordpress/docs/errors-index/suspect-parameter-numbers-spn/# 
 * 7. sae_j_1939_spreadsheet_supported_by_TwidoExtreme.xls
 * 8. https://embeddedflakes.com/network-management-in-sae-j1939/
 * 9. https://embeddeduse.com/2020/01/17/introduction-to-the-sae-j1939-standard/
 * 10. https://grayhill.github.io/comm_protocols/sae_j1939.html
 * 11. J1939-71 (REV. DEC2003)
 * 12. J1939-81
 * 13. J1939-71 (REV. MAR2011)
 * 14. J1939-21 (DEC2006)
 */
#pragma once

namespace embr { namespace j1939 {

enum class spns
{
    engine_intercooler_temperature = 52,
    parking_brake_switch = 70,                      // [5]
    wheel_based_vehicle_speed = 84,
    engine_coolant_temperature = 110,
    net_battery_current = 114,                      // [11]
    alternator_current = 115,                       // [5]
    brake_application_pressure = 116,               // [13]
    key_switch_battery_potential = 158,             // [5]
    alternator_potential = 167,                     // [11]
    battery_potential = 168,                        // [5]
    power_input_1 = 168,                            // [5]
    cab_interior_temperature = 170,                 // [13]
    ambient_air_temperature = 171,                  // [13]
    fuel_temperature = 174,
    engine_oil_temperature = 175,
    turbocharger_oil_temperature = 176,
    engine_speed = 190,
    unit_number_power_unit = 233,                   // [11]
    software_identification = 234,                  // [11]
    vehicle_identification_number = 237,            // [11] ASCII VIN
    engine_oil_life_remaining = 355,                // [5]
    drivers_demand_engine_percent_torque = 512,     // [5]
    actual_engine_percent_torque = 513,             // [5]
    brake_pedal_position = 521,                     ///< "Ratio of brake pedal position to maximum pedal position.  Used for electric brake applications. 0% means no braking.
                                                    ///<  Also when there are two brake pedals on the machine (Left Brake Pedal Position SPN 3033 and Right Brake Pedal
                                                    ///< Position SPN 3032) the maximum of the two should be transmitted for Brake Pedal Position." [7]
    make = 586,                                     ///< "Make of the component" [11] ATA/VMRS code
    model = 587,                                    ///< "Model of the component" [11] ASCII
    serial_number = 588,                            ///< "Serial number of the component" [11] ASCII
    brake_switch = 597,                             // [5]
    left_turn_lamp_circuit = 0x36F,                 // [4], [6]
    stop_turn_lamp_circuit = 0x370,                 // [4]
    right_turn_lamp_circuit = 0x371,                // [4]
    headway_controller_right_turn_signal_monitor = 891,     // [6]
    headway_controller_left_turn_signal_monitor = 892,      // [6]
    engine_torque_mode = 899,                       // [5]
    service_component_identification = 911,         // [7]  DEBT: There are many of these, all referring to "TABLE SPN911_A"
    location = 927,                                 ///< To identify to which of several similar devices (such as tires or fuel tanks) the information applies. [13]
    drive_axle_location = 930,                      // [13]
    seconds = 959,                                  // [13]
    minutes = 960,                                  // [13]
    hours = 961,                                    // [13]
    day = 962,                                      // [13] DEBT: Change name so plural is consistent
    month = 963,                                    // [13] DEBT: Change name so plural is consistent
    year = 964,                                     // [13] DEBT: Change name so plural is consistent
    number_of_software_identification_fields = 965, // [11]
    estimated_percent_fan_speed = 975,              // [7]
    fan_drive_state = 977,                          // [7]
    ac_high_pressure_fan_switch = 985,              // [7]
    requested_percent_fan_speed = 986,              // [13]
    trip_group_1 = 988,                             // [13]
    trip_fan_on_time = 994,                         // [7]
    trip_fan_on_time_due_to_engine = 995,           // [7]
    trip_fan_on_time_due_to_manual_switch = 996,    // [7]
    trip_fan_on_time_due_to_ac = 997,               // [7]
    engine_intercooler_thermostat_opening = 1134,
    anti_theft_encryption_seed_present_indicator = 1194, // [13]
    anti_theft_password_valid_indicator = 1195,     // [13]
    anti_theft_password_random_number = 1198,       // [11] ASCII
    anti_theft_password_representation = 1202,      // [11] ASCII
    engine_synchronization_switch = 1377,           // [13]
    source_address_of_controlling_device_for_engine_control = 1483, //[5]
    background_illumination_level_cmd = 1487,       // [5]
    instrument_panel_1_backlighting_driver = 1491,  // [6]
    instrument_panel_2_backlighting_driver = 1492,  // [6]
    armrest_1_switch_1 = 1503,                      // [5]
    front_axle_left_wheel_speed = 1592,             // [7]
    front_axle_right_wheel_speed = 1593,            // [7]
    rear_axle_left_wheel_speed = 1594,              // [7]
    rear_axle_right_wheel_speed = 1595,             // [7]
    local_minute_offset = 1601,                     // [13]
    local_hour_offset = 1602,                       // [13]
    adjust_seconds = 1603,                          // [13]
    adjust_minutes = 1604,                          // [13]
    adjust_hours = 1605,                            // [13]
    adjust_month = 1606,                            // [13]
    adjust_year = 1607,                             // [13]
    adjust_local_minute_offset = 1608,              // [13]
    adjust_local_hour_offset = 1609,                // [13]
    driver1_identification = 1625,                  // [11] ASCII
    driver2_identification = 1626,                  // [11] ASCII
    fan_speed = 1639,                               // [7]
    engine_automatic_start_enable_switch = 1656,    // [13]
    engine_starter_mode = 1675,                     // [5]
    auxiliary_heater_water_pump_status = 1676,      // [11]
    auxiliary_heater_mode = 1677,                   // [13]
    cab_ventilation = 1678,                         // [11]
    cab_heating_zone = 1680,                        // [11]
    battery_main_switch_hold_request = 1682,        // [13]
    auxiliary_heater_mode_request = 1683,           // [13]
    request_cab_zone_heating = 1686,                // [13]
    cab_interior_temperature_command = 1691,        // [13]
    operator_seat_direction_switch = 1714,          // [7]
    alternator_current_highres = 1795,              // [7]
    battery1_temperature = 1800,                    // [7]
    battery2_temperature = 1801,                    // [7]
    steering_wheel_turn_angle = 1807,               // [7]
    vdc_information_signal = 1813,                  // [11]
    vdc_fully_operational = 1814,                   // [11]
    vdc_brake_light_request = 1815,                 // [13]
    total_average_fuel_rate = 1834,                 // [11]
    total_average_fuel_economy = 1835,              // [11]
    seat_belt_switch = 1856,                        // [11]
    high_beam_headlight_cmd = 2347,                 // [11]
    high_beam_headlight_data = 2348,                // [7]
    low_beam_headlight_cmd = 2349,                  // [11]
    low_beam_headlight_data = 2350,                 // [11]
    alternate_headlights_cmd = 2351,                // [5]
    left_turn_signal_lights_cmd = 2367,             // [5]
    left_turn_signal_lights = 2368,                 // [5]
    right_turn_signal_lights_cmd = 2369,            // [5]
    right_turn_signal_lights = 2370,                // [5]
    left_stop_lights_cmd = 2371,                    // [5], [6]
    left_stop_lights = 2372,                        // [5]
    right_stop_lights_cmd = 2373,                   // [5], [6]
    right_stop_lights = 2374,                       // [5]
    center_stop_lights_cmd = 2375,                  // [5]
    center_stop_lights = 2376,                      // [5]
    front_fog_lights_cmd = 2387,
    tractor_fog_lights = 2388,                      // [7]
    rear_fog_lights_cmd = 2389,
    backup_lights_and_alarm_horn_cmd = 2391,        // [5]
    lighting_data_message_request = 2393,           ///< "requesting the lighting data message from all lighting controllers, providing the state of all lights." [5]
                                                    ///< ISO 11783-7
    implement_oem_option1_light_command = 2395,     // [13]
    implement_oem_option2_light_command = 2397,     // [13]
    implement_left_facing_work_light_command = 2399,// [13]
    daytime_running_lights_cmd = 2403,              // [5]
    daytime_running_lights = 2404,                  // [5]
    running_light = 2404,                           // [7]
    implement_right_facing_work_light_command = 2406,   // [13]
    number_of_members = 2409,                       ///< "Provides the count of the number of members in a particular Working Set" [12] 4.2.4.3
    engine_demand_percent_torque = 2432,            // [5]
    parameter_group_number_rqst = 2540,             // [5]
    controller_application_identity = 2554,         // [5]
    rqst2 = 2574,                                   // [14] Section D.1
    use_transfer_mode = 2575,                       // [14] Section D.1
    short_name_of_device = 2554,                    // [14] Section D.1
    joystick1_x_lever_left_neg_pos_status = 2670,   // [7]
    fms_sw_supported = 2806,                        // [11]
    NAME_of_working_set_member = 2845,              ///<  [12] 4.2.4.5
    address_assignment = 2847,                      ///<  "new source address" [5], [12] Section 4.2.3.3
    NAME_of_controller_application = 2848,          // [5], [12] Section 4.1.1
    NAME_of_commanded_address_target = 2849,        // [5]
    main_light_switch = 2872,                       // [5]
    work_light_switch = 2873,                       // [7]
    high_low_beam_switch = 2874,                    // [7]
    hazard_light_switch = 2875,                     // [5], [13]
    turn_signal_switch = 2876,                      // [5], [6]
    operators_desired_delay_lamp_off_time = 2877,   // [5]
    operators_desired_back_light = 2878,            // [5]
    total_count_of_config_changes_made = 2887,      // [13]
    ecu_part_number = 2901,                         // [7]
    ecu_serial_number = 2902,                       // [7]
    ecu_location = 2903,                            // [7]
    ecu_type = 2904,                                // [7]
    halt_brake_swith = 2911,                        // [13]
    engine_multi_unit_sync_state = 2890,            // [13]
    auxiliary_io_17 = 3840,                         // [13]
    auxiliary_io_112 = 3935,                        // [13]
    actual_engine_percent_torque_fractional = 4154, // [5]
    auxiliary_io_channel_3 = 4158,                  // [13]
    message_counter = 4206,                         ///< The message counter is used to detect situations where the transmitting ECU malfunction repeats the same frame all the time. [13]
    message_checksum = 4207,                        // [13]
    ecu_manufacturer_name = 4304,                   // [7]
    isobus_compliance_cert_type_vt = 4322,          // [5]
    armrest_1_switch_2 = 5492,                      // [5]
    armrest_2_switch_1 = 5493,                      // [5]
    armrest_2_switch_2 = 5494,                      // [5]
    park_brake_command = 5630,                      ///< [13] - "Request by a secondary device"
    industry_group_qualifier_flag = 5664,           // [5]
    arbitrary_address_capable_flag = 5665,          // [5]
    commanded_industry_group = 5673,                // [5]
    commanded_arbitrary_address_capable = 5674,     // [5]
    driver_warning_system_indicator_status = 5825,  // [5]
    tire_location = 6971,                           ///<  Linked to tire wireless signal strength indicator
                                                    ///<  https://www.isobus.net/isobus/pGNAndSPN/9507?type=SPN
    transmit_pgns_commanded_port = 8595,            // [5]
    transmit_pgns_transaction_mode = 8596,          // [5]
    commanded_receive_spn_source_address = 8841,    // [5]
    configurable_receive_spn_source_address = 8846, // [5]
    imposter_pg_source_address = 10841,             // [5]
    imposter_pg_destination_address = 10842,        // [5]
    auxiliary_lamp_group_switch = 12964,            // [5]
    hvess_estimated_remaining_distance = 15268,     // [5]
    operator_key_switch_off = 21127,                // [5]
};

namespace spn {

// [11] Section 5.1.4, Table 2
enum class discrete_parameters
{
    disabled = 0,
    enabled,
    error,
    not_available,
    // My own name...
    noop = not_available
};


typedef discrete_parameters measured;


// [11] Section 5.1.4, Table 3
enum class control_commands
{
    disable = 0,
    enable,
    reserved,
    noop
};

typedef control_commands status;

}


}}
