/*
 * IP_Element.cpp
 *
 *  Created on: Jan 19, 2015
 *      Author: benk
 */

#include "IP_Element.hpp"

IP_Element::IP_Element(std::vector<IP_Token> &tokens, IP_ElemPreType elemType) :
IP_NetlistStructBase(tokens), elemType_(elemType)
{
  //
}

