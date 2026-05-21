/*
 * CS_AnalogLoopGenerateStatement.cpp
 *
 *  Created on: Apr 24, 2013
 *      Author: benk
 */

#include "CS_AnalogLoopGenerateStatement.hpp"

CS_AnalogLoopGenerateStatement::CS_AnalogLoopGenerateStatement(
    CS_VariableDeclaration* variableDeclr,
    const CS_ModuleDeclaration* module, int startLine, int startColumn)
: CS_AnalogStatement(module, startLine, startColumn), variableDeclr_(variableDeclr)
{
  // initialize the internal fields
  loopBody_ = 0;
  initialStatement_ = 0;
  loopCondition_ = 0;
  incementExpression_ = 0;
}
