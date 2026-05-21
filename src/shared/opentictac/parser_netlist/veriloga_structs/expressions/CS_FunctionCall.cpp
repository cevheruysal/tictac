/*
 * CS_FunctionCall.cpp
 *
 *  Created on: Dec 14, 2012
 *      Author: benk
 */


#include "CS_FunctionCall.hpp"

  /** Ctor with array of arguments  arguments */
CS_FunctionCall::CS_FunctionCall( CS_ExpressionBasis** arg1, int nrArg,
      const CS_ModuleDeclaration* module, int startLine, int startColumn,
      bool canHaveDefaultParams) :
      CS_ExpressionBasis(module, startLine, startColumn),
      nrArgs_(nrArg), args_(0)
{
  VERILOG_PRINT_L3( verb(), "CS_FunctionCall::CS_FunctionCall  nrArgs_ = " << nrArgs_);
  args_.resize(nrArgs_);
  int realFuncArg = 0;
  // count the real arguments, those which are not null
  for (int i = 0 ; i < nrArgs_ ; i++ ){
      // only when this is not null
      if ( canHaveDefaultParams || (arg1[i] != 0) )
        {
          args_[realFuncArg] = arg1[i];
          realFuncArg++;
        }
  }
  nrArgs_ = realFuncArg;
}
