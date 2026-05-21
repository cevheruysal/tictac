/*
 * IP_Output.hpp
 *
 *  Created on: Jan 29, 2015
 *      Author: benk
 */

#ifndef IP_OUTPUT_HPP_
#define IP_OUTPUT_HPP_

#include "IP_ControlBase.hpp"

/** this class represnts various output (but not measurements or assertions or similar)
 * control statements */
class IP_Output : public IP_ControlBase
{
public:

  /** Ctor with the array of cards */
  IP_Output(std::vector<IP_Token> &tokens);

  /** empty Dtor*/
  virtual ~IP_Output() {;}

  /** method for the data structure traversal*/
  virtual void Accept(IP_NetlistVisitBase &visitor) { visitor.Visit( (IP_Output &)(*this)); }

private:

};

#endif /* IP_OUTPUT_HPP_ */
