/*
 * IP_Option.cpp
 *
 *  Created on: Jan 19, 2015
 *      Author: benk
 */

#include "IP_Option.hpp"

IP_Option::IP_Option(std::vector<IP_Token> &tokens) : IP_ControlBase(tokens),
optionName_("")
{
  // set the fields if necessary
  optionName_ = sim_toLowerString( tokens[0].getValue() );
  // the value is optional
  if (tokens.size() > 1)
    optionalValue_ = tokens[1];
}

