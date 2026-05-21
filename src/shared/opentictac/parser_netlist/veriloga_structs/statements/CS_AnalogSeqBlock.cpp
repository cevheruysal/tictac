/*
 * CS_AnalogSeqBlock.cpp
 *
 *  Created on: Dec 18, 2012
 *      Author: benk
 */

#include "CS_AnalogSeqBlock.hpp"

CS_AnalogSeqBlock::CS_AnalogSeqBlock(const CS_ModuleDeclaration* module, int startLine, int startColumn):
CS_AnalogStatement( module, startLine, startColumn), instructions_(), nrInstructions_(0)
{
  // nothing to do here
}
