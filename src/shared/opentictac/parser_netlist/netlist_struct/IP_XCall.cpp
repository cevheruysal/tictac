/*
 * IP_XCall.cpp
 *
 *  Created on: Jan 19, 2015
 *      Author: benk
 */

#include "IP_XCall.hpp"

IP_XCall::IP_XCall(std::vector<IP_Token> &              tokens,
                   int                                  nrPinns,
                   std::string &                        sucktName,
                   std::vector<TICTAC_SPTR<IP_Param> > &callParams,
                   std::vector<std::string> &           pinNameBinding)
    : IP_NetlistStructBase(tokens),
      nrPinns_(nrPinns),
      sucktName_(sucktName),
      callParams_(callParams),
      pinNameBinding_(pinNameBinding)
{
  // Also put the params into an unordered_map.
  for(const auto& elem : callParams)
    add_param_(elem);
}

IP_XCall::IP_XCall(std::vector<IP_Token> &              tokens,
                   int                                  nrPinns,
                   std::string const &                  sucktName,
                   std::vector<TICTAC_SPTR<IP_Param> > &callParams,
                   std::vector<std::string> &           pinNameBinding)
    : IP_NetlistStructBase(tokens)
    , nrPinns_(nrPinns)
    , sucktName_(sucktName)
    , callParams_(callParams)
    , pinNameBinding_(pinNameBinding)
{
  // Also put the params into an unordered_map.
  for(const auto& elem : callParams)
    add_param_(elem);
}

void IP_XCall::add_param( TICTAC_SPTR<IP_Param> p)
{
  callParams_.push_back(std::move(p));
}