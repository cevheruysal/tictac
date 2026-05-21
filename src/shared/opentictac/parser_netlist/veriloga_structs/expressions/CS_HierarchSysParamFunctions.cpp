/*
 * CS_HierarchSysParamFunctions.cpp
 *
 *  Created on: Mar 5, 2013
 *      Author: benk
 */

#include "CS_HierarchSysParamFunctions.hpp"

CS_HierarchSysParamFunctions::CS_HierarchSysParamFunctions(
    HierarchSysParamFuncType ft,
    const CS_ModuleDeclaration* module,
    int startLine, int startColumn) :
    CS_ExpressionBasis(module, startLine, startColumn), functionType_(ft)
{
  // Nothing to do here more ...
}

