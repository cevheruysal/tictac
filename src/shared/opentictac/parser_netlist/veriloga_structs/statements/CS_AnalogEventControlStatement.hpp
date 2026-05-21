/*
 * CS_AnalogEventControlStatement.hpp
 *
 *  Created on: Dec 20, 2012
 *      Author: benk
 */

#ifndef CS_ANALOGEVENTCONTROLSTATEMENT_HPP_
#define CS_ANALOGEVENTCONTROLSTATEMENT_HPP_

#include "CS_AnalogStatement.hpp"
#include "CS_AnalogEventFunction.hpp"

/** class that represents the event based blocks
 *
 * see analog_event_control_statement in LRM*/
class CS_AnalogEventControlStatement : public CS_AnalogStatement
{
public:

  /** Ctor */
  CS_AnalogEventControlStatement(CS_AnalogStatement* condInstr,
      const CS_ModuleDeclaration* module, int startLine, int startColumn);

  /** Dtor */
  virtual ~CS_AnalogEventControlStatement() { ; }

  /** add one event condition to this instruction, the logical link between these events is always or*/
  void addEvent(CS_AnalogEventFunction* event) {
    if (event != 0) {
        eventConditions_.push_back(event); nrEvents_++;
    }
  }

  /** returns the event function at the given position */
  CS_AnalogEventFunction* getEventFunction(int i) { return eventConditions_[i]; }

  /** returns the instructions in this block */
  CS_AnalogStatement* getInstruction() { return condInstr_; }

  /** returns the instructions in this block */
  CS_AnalogStatement** getInstructionPointer() { return &(condInstr_); }

  /** returns the number of event conditions (which are OR-ed)*/
  int getNrEventConditions() const { return nrEvents_; }

  /** method for the tree traversal */
  virtual void Accept(CS_CompilerVisitorBasis &visitor) { visitor.Visit( (CS_AnalogEventControlStatement &)(*this)); }

private:

  /** vector with all the conditions (event functions) */
  std::vector<CS_AnalogEventFunction*> eventConditions_;

  /** number of eventConditions */
  int nrEvents_;

  /** the instruction(s) that should be executed if the condition is satisfied */
  CS_AnalogStatement* condInstr_;
};

#endif /* CS_ANALOGEVENTCONTROLSTATEMENT_HPP_ */
