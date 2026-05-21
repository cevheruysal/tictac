/*
 * CS_AnalogLoopStatement.cpp
 *
 *  Created on: Mar 7, 2013
 *      Author: benk
 */

#include "CS_AnalogLoopStatement.hpp"

CS_AnalogLoopStatement::CS_AnalogLoopStatement(
    const CS_ModuleDeclaration* module, int startLine, int startColumn,
    VerilogLoopType loopType) :
    CS_AnalogStatement(module, startLine, startColumn)
{
  // initialize the fields
  loopBody_ = 0;
  initialStatement_ = 0;
  loopCondition_ = 0;
  loopType_ = loopType;
}


