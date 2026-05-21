/*
 * CS_AnalogSeqBlock.hpp
 *
 *  Created on: Dec 18, 2012
 *      Author: benk
 */

#ifndef CS_ANALOGSEQBLOCK_HPP_
#define CS_ANALOGSEQBLOCK_HPP_

#include "verilog_comp.hpp"
#include "CS_AnalogStatement.hpp"

/** class to model the collection of instructions
 *
 * see analog_seq_block from LRM */
class CS_AnalogSeqBlock : public CS_AnalogStatement
{
public:

  CS_AnalogSeqBlock(const CS_ModuleDeclaration* module, int startLine, int startColumn);

  virtual ~CS_AnalogSeqBlock(){;}

  /** returns one instruction */
  CS_AnalogStatement* getInstruction( int i ) { return instructions_[i]; }

  const CS_AnalogStatement* getInstruction( int i ) const { return instructions_[i]; }

  /** adds one instruction to the end of the sequence */
  void addInstruction(CS_AnalogStatement* newInstr) { instructions_.push_back(newInstr); nrInstructions_++; }

  /** replaces one instruction into the stream of statement */
  void replaceInstruction(CS_AnalogStatement* newInstr, int position)
  {
    instructions_[position] = newInstr;
  }

  /** returns the number of instructions contained in this sequence */
  int getNrInstruction() const { return nrInstructions_; }

  /** method for the tree traversal */
  virtual void Accept(CS_CompilerVisitorBasis &visitor) { visitor.Visit( (CS_AnalogSeqBlock &)(*this) ); }

private:

  /** vector containing the instructions */
  std::vector<CS_AnalogStatement*> instructions_;

  /** number of instructions */
  int nrInstructions_;

};

#endif /* CS_ANALOGSEQBLOCK_HPP_ */
