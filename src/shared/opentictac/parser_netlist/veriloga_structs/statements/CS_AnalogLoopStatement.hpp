/*
 * CS_AnalogLoopStatement.hpp
 *
 *  Created on: Mar 7, 2013
 *      Author: benk
 */

#ifndef CS_ANALOGLOOPSTATEMENT_HPP_
#define CS_ANALOGLOOPSTATEMENT_HPP_

#include "verilog_comp.hpp"
#include "CS_AnalogStatement.hpp"
#include "CS_ExpressionBasis.hpp"

/** the enum type do differential between different types of loops */
typedef enum {
  VERILOG_VAC_LOOP_FOR          = 0,  // for loop
  VERILOG_VAC_LOOP_WHILE        = 1,  // while loop
  VERILOG_VAC_LOOP_REPEAT       = 2   // repeat loop
} VerilogLoopType;


/**
 * This statement represents both the FOR and the WHILE loops
 *
 * see in LRM: analog_loop_statement */
class CS_AnalogLoopStatement : public CS_AnalogStatement
{
public:

  /** Ctor with the required parameters*/
  CS_AnalogLoopStatement(const CS_ModuleDeclaration* module, int startLine, int startColumn,
      VerilogLoopType loopType);

  /** empty Dtor*/
  virtual  ~CS_AnalogLoopStatement() {;}

  /** method for the tree traversal */
  virtual void Accept(CS_CompilerVisitorBasis &visitor) { visitor.Visit( (CS_AnalogLoopStatement &)(*this) ); }

  /** returns the body of a loop*/
  CS_AnalogStatement* getLoopBody() { return loopBody_; }
  const CS_AnalogStatement* getLoopBody() const { return loopBody_; }
  /** returns pointer to the body of the loop, for the function expansion purposes */
  CS_AnalogStatement** getLoopBodyPointer()
  { // return pointer to the instruction for function expansion
    return (loopBody_) ? &loopBody_ : 0;
  }
  /** sets the body statements of the loop */
  void setBodyLoop(CS_AnalogStatement* b) { loopBody_ = b; }

  /** returns the initial statement of the loop (used only for FOR)*/
  CS_AnalogStatement* getLoopInitialStatement() { return initialStatement_; }
  const CS_AnalogStatement* getLoopInitialStatement() const { return initialStatement_; }
  /** returns pointer to the initial statement, for the function expansion purposes */
  CS_AnalogStatement** getLoopInitialStatementPointer()
  { // return pointer to the instruction for function expansion
    return (initialStatement_) ? &initialStatement_ : 0;
  }
  /** sets the initial statement of the loop*/
  void setLoopInitialStatement(CS_AnalogStatement* i) { initialStatement_ = i; }

  /** returns the condition of the loop */
  CS_ExpressionBasis* getLoopCondition() { return loopCondition_;}
  const CS_ExpressionBasis* getLoopCondition() const { return loopCondition_;}
  /** sets the condition of the loop*/
  void setLoopCondition(CS_ExpressionBasis* c) { loopCondition_ = c; }

  /** returns the type of the loop */
  VerilogLoopType getLoopType() const { return loopType_; }

public:

  /** the body of the loop that also includes
   * in case of FOR as last statement */
  CS_AnalogStatement* loopBody_;

  /** initial statement, used only for FOR loops, for WHILE this is null */
  CS_AnalogStatement* initialStatement_;

  /** condition of the loop */
  CS_ExpressionBasis* loopCondition_;

  /** the type of the loop */
  VerilogLoopType loopType_;

};

#endif /* CS_ANALOGLOOPSTATEMENT_HPP_ */
