/*
 * CS_AnalogDispOrSysFunc.cpp
 *
 *  Created on: Mar 4, 2013
 *      Author: benk
 */

#include "CS_AnalogDispOrSysFunc.hpp"

CS_AnalogDispOrSysFunc::CS_AnalogDispOrSysFunc(
    CS_SystemFunction *funcCalled,
    const CS_ModuleDeclaration* module, int startLine, int startColumn)
:CS_AnalogStatement( module, startLine, startColumn), funcCalled_(funcCalled)
{
  // nothing to do
  //
}


