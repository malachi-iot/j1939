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
 * 11. J1939-71 (REV. DEV2003)
 */
#pragma once

#include "pgn.h"
#include "spn/enum.h"
#include "spn/fwd.h"

// DEBT: Make an estd fill so we don't have to use std
#include <estd/algorithm.h>
#include <estd/ratio.h>

namespace embr { namespace j1939 {


}}