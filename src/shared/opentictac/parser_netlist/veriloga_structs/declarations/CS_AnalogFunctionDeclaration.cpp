/*
 * CS_AnalogFunctionDeclaration.cpp
 *
 *  Created on: Dec 14, 2012
 *      Author: benk
 */

#include "CS_AnalogFunctionDeclaration.hpp"
#include "CS_VariableDeclaration.hpp"

CS_AnalogFunctionDeclaration::CS_AnalogFunctionDeclaration(
    std::string functionName,
    VarType funcType,
    CS_VariableDeclaration *functionVariable,
    const CS_ModuleDeclaration* module,
    int startLine, int startColumn)
{
  // set the initial values of all fields
  module_ = module;
  nrInputArgs_ = 0;
  startLine_ = startLine;
  startColumn_ = startColumn;
  functionName_ = functionName;
  functionVariable_ = functionVariable;
  localVarDeclrs_.resize(0);
  nrLocalVars_ = 0;
  argVariables_.resize(0);
  argVariableTypes_.resize(0);
  nrArgsInstance_ = 0;
  funcType_ = funcType;
  instructions_ =  new CS_AnalogSeqBlock(module,startLine, startColumn);
}

std::string CS_AnalogFunctionDeclaration::addPrefixForLocalVars(CS_AnalogFunctionDeclaration* func, std::string name)
 {
    return "__funcLCV__" + func->getFunctionName() + "_" + name;
 }
