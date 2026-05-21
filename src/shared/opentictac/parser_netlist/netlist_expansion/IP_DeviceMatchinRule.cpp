/*
 * IP_DeviceMatchinRule.cpp
 *
 *  Created on: Feb 20, 2015
 *      Author: benk
 */

#include "IP_DeviceMatchinRule.hpp"

IP_DeviceMatchinRule::IP_DeviceMatchinRule() : UT_SIMVerboseClass("DeviceMatchinRule")
{
  // TODO Auto-generated constructor stub
  // leave fields empty
  devType_ = ERR_DEV_TYPE;
  nrPinns_ = -1;

  firstKeyWord_ = "";
  firstKeyWordValue_ = "";
  modelType_ = "";

  moduleName_ = ""; lastKeyWord_ = "";
  deviceName_ = "";
}


IP_DeviceMatchinRule::IP_DeviceMatchinRule(IP_DeviceMatchinRule &c) :UT_SIMVerboseClass("DeviceMatchinRule")
{
  devType_           = c.devType_;
  deviceName_        = c.deviceName_;
  nrPinns_           = c.nrPinns_;
  optionalPinNames_  = c.optionalPinNames_;
  pinFlags_          = c.pinFlags_;
  firstKeyWord_      = c.firstKeyWord_;
  firstKeyWordValue_ = c.firstKeyWordValue_;
  modelType_         = c.modelType_;

  paramNames_        = c.paramNames_;
  paramTypes_        = c.paramTypes_;
  paramDefaultValue_ = c.paramDefaultValue_;

  paramAliasInd_   = c.paramAliasInd_;
  paramAliasNames_ = c.paramAliasNames_;

  moduleName_ = c.moduleName_;

  lastKeyWord_ = c.lastKeyWord_;
}
