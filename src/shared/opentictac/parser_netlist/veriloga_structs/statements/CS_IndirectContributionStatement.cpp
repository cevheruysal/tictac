/*
 * CS_IndirectContributionStatement.cpp
 *
 *  Created on: Apr 9, 2013
 *      Author: benk
 */

#include "CS_IndirectContributionStatement.hpp"

CS_IndirectContributionStatement::CS_IndirectContributionStatement(
    const CS_ModuleDeclaration* module,
    int startLine, int startColumn,
    CS_BranchProbeFunction *lvalue,
    CS_ExpressionBasis *indirectExpr,
    CS_ExpressionBasis *rExpr, std::string attribStr):
CS_AnalogStatement(module, startLine, startColumn),
lvalue_(lvalue), indirectExpr_(indirectExpr), rExpr_(rExpr), attribStr_(attribStr)
{
  // nothing to do here (assuming that the checks have been done previously )
}
