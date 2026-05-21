/*
 * CS_AnalogInitialBlock.hpp
 *
 *  Created on: May 28, 2014
 *      Author: benk
 */

#ifndef CS_ANALOGINITIALBLOCK_HPP_
#define CS_ANALOGINITIALBLOCK_HPP_

#include "CS_AnalogStatement.hpp"
#include "CS_AnalogSeqBlock.hpp"
#include "CS_ModuleDeclaration.hpp"

/** class represents the statments within the analog initial */
class CS_AnalogInitialBlock : public CS_AnalogStatement
{
public:

  /** Ctor
   * @param module  the module
   * @param startLin start line of this initial block
   * @param startColumn  start column of this initial block */
  CS_AnalogInitialBlock(const CS_ModuleDeclaration* module, int startLine, int startColumn);

  /** empty Dtor */
  virtual  ~CS_AnalogInitialBlock() {;}

  /** returns the analog initial instruction sequence of the module */
  const CS_AnalogStatement* getInitInstrSequence() const
      { return (const CS_AnalogStatement*)analogInitInstructions_; }
  CS_AnalogStatement* getInitInstrSequence() { return (CS_AnalogStatement*)analogInitInstructions_; }

  /** @returns the number of analog initial statement */
  int getNrInitInstrSequence() const;

  /** adds one instruction to the analog initial instruction sequence
   * of this module
   * @param instruction the new instruction to be added */
  void addInitInstr(CS_AnalogStatement *instruction);

  /** inherited from the super class */
  virtual void Accept(CS_CompilerVisitorBasis &visitor)
  {
    visitor.Visit((CS_AnalogInitialBlock&)(*this));
  }

private:

  /** the statements within the analog initial block */
  CS_AnalogSeqBlock *analogInitInstructions_;
};

#endif /* CS_ANALOGINITIALBLOCK_HPP_ */
