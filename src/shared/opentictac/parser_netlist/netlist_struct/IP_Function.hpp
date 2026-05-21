/*
 * IP_Function.hpp
 *
 *  Created on: Jan 19, 2015
 *      Author: benk
 */

#ifndef IP_FUNCTION_HPP_
#define IP_FUNCTION_HPP_

#include "IP_NetlistStructBase.hpp"

/** class representing a function declaration in a given context */
class IP_Function : public IP_NetlistStructBase
{
public:

  /** Ctor */
  IP_Function(std::vector<IP_Token> &tokens);

  /** empty Dtor */
  virtual ~IP_Function() {;}

  /** method for the data structure traversal*/
  virtual void Accept(IP_NetlistVisitBase &visitor) { visitor.Visit( (IP_Function &)(*this)); }

  const std::string& getName() const { return this->getToken(1).getValue(); }
  /// returns the number of input arguments
  MYINT getNrParms() const { return nrParams_; }

private:

  // TODO: extend this later

  /// number of input arguments
  MYINT nrParams_;
};

#endif /* IP_FUNCTION_HPP_ */
