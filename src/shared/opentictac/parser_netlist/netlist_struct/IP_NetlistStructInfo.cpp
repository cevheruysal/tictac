/*
 * IP_NetlistStructInfo.cpp
 *
 *  Created on: Mar 16, 2015
 *      Author: benk
 */

#include "IP_NetlistStructInfo.hpp"

IP_NetlistStructInfo::IP_NetlistStructInfo()
{
  isUsed_ = true;
}
IP_NetlistStructInfo::IP_NetlistStructInfo(const IP_NetlistStructInfo &c)
{
  isUsed_ = c.isUsed_;
}


