/*
 * CS_RANGE_HPP_.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: benk
 */

#include "CS_Range.hpp"

CS_Range::CS_Range(CS_ExpressionBasis *exprL, CS_ExpressionBasis *exprR): exprLeft_(exprL),
exprRight_(exprR)
{
  // just generate the code that takes the difference of the expressions results
  // the ranges must be constantly evaluable expressions
  //VERILOG_ERROR_COND_STOP( exprL->getClasses() != CONST, "Ranges must be constant expressions", exprL);
  //VERILOG_ERROR_COND_STOP( exprR->getClasses() != CONST, "Ranges must be constant expressions", exprR);
}

