/*
 * IP_GlobalSettings.cpp
 *
 *  Created on: Feb 4, 2015
 *      Author: benk
 */

#include "IP_GlobalSettings.hpp"

IP_GlobalSettings::IP_GlobalSettings(std::vector<IP_Token> &tokens, GlobalSettingType type):
IP_ControlBase(tokens), type_(type)
{
  //
}

