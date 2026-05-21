/*
 * IP_Function.cpp
 *
 *  Created on: Jan 19, 2015
 *      Author: benk
 */

#include "IP_Function.hpp"

IP_Function::IP_Function(std::vector<IP_Token> &tokens) : IP_NetlistStructBase(tokens)
{
  //

  nrParams_ = 0;

  MYINT i = 0;
  // count the number of input parameters to the function
  for (i = 2; i < (MYINT)tokens.size(); ++i)
    {
      // if we are at the end then stop
      if (   (tokens[i].getTokenType() == TK_CLOSE_BR)
          || (tokens[i].getTokenType() == TK_EQUAL) )
        break;
      nrParams_++;
    }
}


