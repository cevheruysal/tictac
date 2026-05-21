/*
 * CS_AnalogConditionalStatement.cpp
 *
 *  Created on: Dec 19, 2012
 *      Author: benk
 */

#include "CS_AnalogConditionalStatement.hpp"

CS_AnalogConditionalStatement::CS_AnalogConditionalStatement(const CS_ModuleDeclaration* module, int startLine, int startColumn,
    CS_ExpressionBasis *condition, CS_AnalogStatement *trueBranch,  CS_AnalogStatement *falseBranch ):
    CS_AnalogStatement( module, startLine, startColumn), condition_(condition), trueBranch_(trueBranch), falseBranch_(falseBranch)
{
  // at this stage nothing to do yet
}
