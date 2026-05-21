/*
 * CS_AnalogCaseStatement.cpp
 *
 *  Created on: Feb 28, 2013
 *      Author: benk
 */

#include "CS_AnalogCaseStatement.hpp"

CS_AnalogCaseStatement::CS_AnalogCaseStatement(
    CS_ExpressionBasis* switchExpr,
    const CS_ModuleDeclaration* module, int startLine, int startColumn) :
    CS_AnalogStatement(module, startLine, startColumn)
{
  VERILOG_ERROR_COND_STOP(switchExpr == 0 ,"A case statements needs always a condition", this);
  switchExpr_ = switchExpr;
  nrCases_ = 0;
  defaultIndex_ = -1 ;
}


