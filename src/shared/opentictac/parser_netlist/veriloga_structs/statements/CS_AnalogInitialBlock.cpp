/*
 * CS_AnalogInitialBlock.cpp
 *
 *  Created on: May 28, 2014
 *      Author: benk
 */

#include "CS_AnalogInitialBlock.hpp"
#include "CS_AnalogSeqBlock.hpp"

CS_AnalogInitialBlock::CS_AnalogInitialBlock(
    const CS_ModuleDeclaration* module, int startLine, int startColumn)
:CS_AnalogStatement(module, startLine, startColumn)
{
  // create sequence
  analogInitInstructions_ = new CS_AnalogSeqBlock(module,startLine , startColumn);
}

void CS_AnalogInitialBlock::addInitInstr(CS_AnalogStatement *instruction)
{
  analogInitInstructions_->addInstruction(instruction);
}

int CS_AnalogInitialBlock::getNrInitInstrSequence() const
{
  return analogInitInstructions_->getNrInstruction();
}
