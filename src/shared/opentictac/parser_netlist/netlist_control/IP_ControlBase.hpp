/*
 * IP_ControlBase.hpp
 *
 *  Created on: Jan 19, 2015
 *      Author: benk
 */

#ifndef IP_CONTROLBASE_HPP_
#define IP_CONTROLBASE_HPP_

#include "IP_NetlistStructBase.hpp"

/** class to represent the super class for all control statements in the
 * netlist */
class IP_ControlBase : public IP_NetlistStructBase
{
public:

  /** Ctor with the tokens forming this controll statement */
  IP_ControlBase(std::vector<IP_Token> &tokens);

  /** Dtor */
  virtual ~IP_ControlBase() {;}

  /** method for the data structure traversal*/
  virtual void Accept(IP_NetlistVisitBase &visitor) { visitor.Visit( (IP_ControlBase &)(*this)); }

};

#endif /* IP_CONTROLBASE_HPP_ */
