/*
 * IP_Assertion.hpp
 *
 *  Created on: Feb 4, 2015
 *      Author: benk
 */

#ifndef IP_ASSERTION_HPP_
#define IP_ASSERTION_HPP_

#include "IP_ControlBase.hpp"

/** class to model all assertions */
class IP_Assertion : public IP_ControlBase
{
public:

  /** Ctor with the tokens */
  IP_Assertion(std::vector<IP_Token> &tokens);

  /** empty Dtor */
  virtual ~IP_Assertion() {;}

  /** method for the data structure traversal*/
  virtual void Accept(IP_NetlistVisitBase &visitor) { visitor.Visit( (IP_Assertion &)(*this)); }

private:
};

#endif /* IP_ASSERTION_HPP_ */
