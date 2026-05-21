/*
 * IP_SubcirSettings.cpp
 *
 *  Created on: Feb 4, 2015
 *      Author: benk
 */

#include "IP_SubcirSettings.hpp"

IP_SubcirSettings::IP_SubcirSettings(std::vector<IP_Token> &tokens,
    SubcktSettingType type) : IP_NetlistStructBase(tokens), type_(type)
{
  //
}
