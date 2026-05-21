/*
 * IP_Measurement.hpp
 *
 *  Created on: Feb 4, 2015
 *      Author: benk
 */

#ifndef IP_MEASUREMENT_HPP_
#define IP_MEASUREMENT_HPP_

#include "IP_ControlBase.hpp"

/** class to model all measurements */
class IP_Measurement : public IP_ControlBase
{
public:

  /** Ctor with all the tokens */
  IP_Measurement(std::vector<IP_Token> &tokens);

  /** empty Dtor */
  virtual ~IP_Measurement() {;}

  /** method for the data structure traversal*/
  virtual void Accept(IP_NetlistVisitBase &visitor) { visitor.Visit( (IP_Measurement &)(*this)); }

private:
};

#endif /* IP_MEASUREMENT_HPP_ */
