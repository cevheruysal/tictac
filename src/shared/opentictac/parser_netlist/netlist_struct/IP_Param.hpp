/*
 * IP_Params.hpp
 *
 *  Created on: Jan 19, 2015
 *      Author: benk
 */

#ifndef IP_PARAMS_HPP_
#define IP_PARAMS_HPP_

#include "IP_NetlistStructBase.hpp"

/** this class represents a parameter declaration in a given context */
class IP_Param : public IP_NetlistStructBase
{
public:

  /** Ctor
   * ATTENTION the input must be always ONLY 2 tokens
   * first one is the parameter name, and the second is the expression */
  IP_Param(std::vector<IP_Token> &tokens);
  IP_Param(std::vector<IP_Token>&& tokens);
  IP_Param(const std::string& param_name, const std::string& param)
      : IP_Param(std::vector<IP_Token>{IP_Token(param_name), IP_Token(param)})
  {
    ;
  };

  /** Copy Ctor */
  IP_Param(const IP_Param &p);

  /** empty Dtor */
  virtual ~IP_Param() { /*std::cout << "Kill Param " << name_ << "\n"*/;}

  /** returns the name of the param */
  const std::string& getName() const { return name_; }
  /// sets the name of the parameter
  void setName(std::string &name) { name_ = name; }

  /** returns the initialization token as an expression */
  const IP_Token& getInitExprToken() const { return exprToken_; }

  /** method for the data structure traversal*/
  virtual void Accept(IP_NetlistVisitBase &visitor) { visitor.Visit( (IP_Param &)(*this)); }

private:

  /** parameter name in lower case format */
  std::string name_;

  /** the token which represents the initialization expression*/
  IP_Token    exprToken_;

};

#endif /* IP_PARAMS_HPP_ */
