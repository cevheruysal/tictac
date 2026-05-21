/*
 * CS_ContributionStatement.cpp
 *
 *  Created on: Dec 19, 2012
 *      Author: benk
 */

#include "CS_ContributionStatement.hpp"

CS_ContributionStatement::CS_ContributionStatement(const CS_ModuleDeclaration* module, int startLine, int startColumn,
    CS_BranchProbeFunction *targetE, CS_ExpressionBasis *evalExp ):CS_AnalogStatement(module, startLine, startColumn), targetE_(targetE), evalExp_(evalExp)
{
  // nothing to do here (assuming that the checks have been done previously )
  VERILOG_ERROR_COND_STOP(targetE == 0, "Target expression of a contribution must be NOT NULL", this);
  VERILOG_ERROR_COND_STOP(evalExp == 0, "Source expression of a contribution must be NOT NULL", this);
}
