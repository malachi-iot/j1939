/**
 *
 * References:
 *
 * 1. https://www.csselectronics.com/pages/j1939-explained-simple-intro-tutorial
 * 2. https://copperhilltech.com/blog/sae-j1939-message-format-and-interpretation-of-pgns/ 
 * 3. https://www.simmasoftware.com/j1939-pgn.html 
 * 4. https://pyramidsolutions.com/wp-content/uploads/2018/07/J1939-Data-Mapping-Explained_Final-1.pdf
 * 5. https://powertraincontrolsolutions.com/download/Released/Public/Developer_Files/PCS%20J1939%20Messages%20v2_1.pdf
 * 6. https://endige.com/2050/nmea-2000-pgns-deciphered/ 
 * 7. https://github.com/DanielMartensson/Open-SAE-J1939
 * 8. https://www.isobus.net/isobus/pGNAndSPN
 * 9. https://intermotive.net/wp-content/uploads/2019/08/J1939CM650-AP-081519.pdf
 * 10. J1939-81
 * 11. J1939-71 (DEC2003)
 * 12. https://copperhilltech.com/blog/sae-j1939-programming-with-arduino-multipacket-broadcast-bam-session/
 * 13. sae_j_1939_spreadsheet_supported_by_TwidoExtreme.xls
 * 14. https://www.csselectronics.com/pages/j1939-pgn-conversion-tool
 * 15. https://www.nmea.org/Assets/july%202010%20nmea2000_v1-301_app_b_pgn_field_list.pdf
 * 16. J1939-21 (DEC2006)
 * 17. J1939-71 (MAR2011)
 * 18. J1939 (2007)
 */
#pragma once

#include <estd/cstdint.h>

// DEBT: Resolve naming convention discrepancy for things like "cab_message1" vs
// "cab_message_3"

namespace embr { namespace j1939 {

// Group extension (pdu2) flavor only
enum class pgns : uint32_t
{
    // +++ pdu1 (PF+DP denoted) here
    tim12 = 0x23,                               ///< Tractor Implement Management (TIM) Server to TIM Client [8]
    auth21 = 0x6F,                              // [8] "Authentication Client to Authentication Server"
    ctpc = 0x71,                                // [8] "Configurable Transmit PGNs Command"
    NAME_mgmtmsg_experimental = 0x93,           // Unknown, I believe this goes here
    text_display = 0xA8,                        // [13]
    request2 = 0xC9,                            // [16]
    transfer_experimental = 0xCA,               // Unknown, I believe this goes here
    time_date_adjust = 0xD5,                    // [11] aka TDA
    anti_theft_status = 0xDC,                   // [13]
    cab_message1 = 0xE0,                        ///< [11] - "Message containing parameters originating from the vehicle cab."
    vt12 = 0xE6,                                ///< Virtual Terminal-to-Node (ISO 11783-6) [8]
    vt21 = 0xE7,                                ///< Virtual Node-to-Terminal (ISO 11783-6) [8]
    acknowledgement = 0xE8,                   // [7]
    request = 0xEA,                           // [7]
    tp_dt = 0xEB,                               ///< Transport Protocol - Data Transfer [12]
    tp_cm = 0xEC,                               ///< Transport Protocol - Connection Management [12]
    address_claimed = 0xEE,                   // [7]
    electronic_brake_system1 = 0x200,           // [14]
    external_brake_request = 0x400,             // [14]
    CANopen_app_message1 = 0x500,               // [14]
    CANopen_app_message2 = 0x600,               // [14]
    // ---

    NAME_management_message = 37632,            // [8] - FIX: Pretty sure this needs to go into pgn_pdu1
    transfer = 51712,                           // [8] - FIX: Pretty sure this needs to go into pgn_pdu1
    pdu2_boundary = 0xF000,                     // EXPERIMENTAL
    electronic_engine_controller_2 = 61443,     // [5]
    electronic_engine_controller_1 = 61444,     // [3], [5]
    turn_signal = 61452,                        // [9] - NOTE: Smells non standard
    twssi = 64581,                              ///<  Tire Wireless Signal Strength Indicator
    evoi1 = 64617,                              ///<  EV Operator Indicators 1 [8]
    armswimc =  64745,                          ///<  Armrest Switch Matrix Commands [8]
    aiso = 64770,                               ///<  All implements stop operations switch state [8]
    wireless_communications_message_2 = 64936,  // [13]
    wireless_communications_message_1 = 64937,  // [13]
    signal_preemption = 64957,                  // [13]
    oel = 64972,                                ///<  Operator External Light Controls Message [8]
    cab_message_3 = 64980,                      // [8]
    wsmem = 65036,                              ///< "Working Set Master Message" [10] 4.2.4.2
    wsmstr = 65037,                             ///< "Working Set Member Message" [10] 4.2.4.4
    vehicle_electrical_power_3 = 65103,         // [11]
    driver_identificationn = 65131,             // [11]
    vehicle_electrical_power_2 = 65165,         // [11]
    trip_fan_information = 65211,               // [12]
    fan_drive_1 = 65213,                        // [17]
    shutdown = 65252,                           // [13]
    time_date = 65254,                          // [13]
    vehicle_hours = 65255,                      // [13]
    vehicle_direction_speed = 65256,            // [13]
    engine_temperature = 65262,
    vehicle_electrical_power_1 = 65271,         // [8]
    operator_indicators = 65279,                ///<   J1939DA [8]
    direct_lamp_control_data2 = 0xFD04,         // [17]
    direct_lamp_control_data1 = 0xFD05,         // [17]
    direct_lamp_control_command2 = 0xFD06,      // [17]
    direct_lamp_control_command1 = 0xFD07,      // [17]
    isobus_compliance_cert = 0xFD42,            // [8]
    ecu_identification = 0xFDC5,                // [7]
    ecm_information = 0xFDC9,                   // [14]
    lighting_data = 0xFE40,
    lighting_command = 0xFE41,                  // [8]  ISO 11783-7
    commanded_address = 0xFED8,                 // [7], [8], [10]
    software_identification = 0xFEDA,           // [7]
    component_identification = 0xFEEB,          // [7]
    engine_fluid = 0xFEEF,                      // [4]
    // Cruise Control/Vehicle Speed
    ccvs = 0xFEF1,                              // [5]
    ambient_conditions = 0xFEF5,                // [17]
    axle_information = 0xFEF9,                  // [17]
    brakes = 0xFEFA,                            // [17]
    dash_display = 0xFEFC,                      // [17]

    // NOTE: Following not necessarily j1939, may be NMEA 2000 only
    nmea_group_function = 126208,               // [15]
    system_time = 126992,                       // [15]
    switch_bank_status = 127501,                // [15]
    switch_bank_control = 127502,               // [8]

    dc_detailed_status = 127506,                // [15]
    charger_status = 127507,                    // [15]
    battery_status = 127508,                    // [15]
    inverter_status = 127509,                   // [15]
    charger_configuration_status = 127510,      // [15]

    position_rapid_update = 129025,             // [15]
    gnss_position_data = 129029,                // [6], [15] (0x1F805)

    time_and_date = 129033,                     // [15]

    time_to_from_mark = 129301,                 // [18]
    ais_utc_date_report = 129793,               // [18] (ITU-R M.1371)
    radio_freq_mode_power = 129799,             // [18]
    ais_utc_date_inquiry = 129800,              // [18]

    environmental_parameters = 130311,          // [18]
    temperature = 130312,                       // [18]
};


}}