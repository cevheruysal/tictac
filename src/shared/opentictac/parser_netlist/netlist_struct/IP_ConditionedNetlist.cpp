/*
 * IP_ConditionedNetlist.cpp
 *
 *  Created on: Jan 19, 2015
 *      Author: benk
 */

#include "IP_ConditionedNetlist.hpp"

IP_ConditionedNetlist::IP_ConditionedNetlist(std::vector<IP_Token> &tokens)
: IP_NetlistStructBase(tokens)
{
  // do not set the true and false branches DUMM, they should be set separately
  //thenBody_ = TICTAC_SPTR<IP_NetlistStructSequence>(new IP_NetlistStructSequence(tokens) );
  // else branch is also defined even though it might not be used
  //falseBody_ = TICTAC_SPTR<IP_NetlistStructSequence>(new IP_NetlistStructSequence(tokens) );

  // default value if false
  hasFalseBranch_ = false;

  // this is the expression of the IF condition
  condExpression_ = tokens[1];
  condExpression_.setTokenType(TK_EXPRESSION);
}
