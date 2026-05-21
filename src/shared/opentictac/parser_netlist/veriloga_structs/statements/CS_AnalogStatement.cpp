/*
 * CS_AnalogStatement.cpp
 *
 *  Created on: Nov 28, 2012
 *      Author: benk
 */

#include "CS_AnalogStatement.hpp"

CS_AnalogStatement::CS_AnalogStatement(const CS_ModuleDeclaration* module, int startLine, int startColumn):
startLine_(startLine), startColumn_(startColumn), module_(module)
{
  comments_ = "";
}
