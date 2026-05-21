/*
 * titan_offsets.hpp
 *
 *  Created on: Sep 12, 2013
 *      Author: benk
 */
/** this header file should contain all the TITAN specific offsets */

#ifndef TITAN_OFFSETS_HPP_
#define TITAN_OFFSETS_HPP_

// TITAN includes
#include "vi-source-def.h"

// ID spec of the resistor
#define RES_ID                1
#define CAP_ID                2
#define IND_ID                3
#define VOLTAGESOURCE_ID      9
#define OUTPUT_ID            42
#define OUTPUT_V_OUT         0
#define OUTPUT_I_OUT         1


//TODO: replace these with defines from TITAN
// INDEPENDENT SOURCE TYPE
#define  INDEPENDENT_SOURCE_DC_TYPE       0
#define  INDEPENDENT_SOURCE_PULSE_TYPE    1
#define  INDEPENDENT_SOURCE_SIN_TYPE      2
#define  INDEPENDENT_SOURCE_PWL_TYPE      4

//
#define  RESISTOR_I3_OFFS             2
#define  RESISTOR_I3_MFACT            14 /*ofs_resv_mfact*/
#define  CAPACITOR_I3_OFFS            7
#define  CAPACITOR_I3_MFACT            9 /*ofs_capv_mfact*/
#define  INDUCTOR_I3_OFFS            4
#define  INDUCTOR_I3_MFACT            5 /*ofs_indv_mfact*/

#define  SOURCE_I5_OFFS            16

#endif /* TITAN_OFFSETS_HPP_ */
