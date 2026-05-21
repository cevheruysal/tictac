/*
 * CS_AnalogLoopGenerateStatement.hpp
 *
 *  Created on: Apr 24, 2013
 *      Author: benk
 */

#ifndef CS_ANALOGLOOPGENERATESTATEMENT_HPP_
#define CS_ANALOGLOOPGENERATESTATEMENT_HPP_

#include "verilog_comp.hpp"
#include "CS_AnalogStatement.hpp"
#include "CS_ExpressionBasis.hpp"
#include "CS_VariableDeclaration.hpp"

/**
 * class for generate for loop, that is used for code generation
 *
 * see analog_loop_generate_statement in LRM */
class CS_AnalogLoopGenerateStatement : public CS_AnalogStatement
{
public:

  CS_AnalogLoopGenerateStatement(CS_VariableDeclaration* variableDeclr,
      const CS_ModuleDeclaration* module, int startLine, int startColumn);

  /** empty Dtor*/
  virtual ~CS_AnalogLoopGenerateStatement() {;}

  /** get the body of the loop */
  CS_AnalogStatement* getLoopBody() { return loopBody_;}
  const CS_AnalogStatement* getLoopBody() const { return loopBody_;}

  /** sets the loop's body */
  void setLoopBody(CS_AnalogStatement* statem) { loopBody_ = statem; }

  /** get the initial expression that sets the genvar variable  */
  CS_ExpressionBasis* getLoopInitStatement() { return initialStatement_;}
  const CS_ExpressionBasis* getLoopInitStatement() const { return initialStatement_;}

  /** sets the loop's initial expression that sets the genvar */
  void setLoopInitStatement(CS_ExpressionBasis* init) { initialStatement_ = init; }

  /** get the body of the loop */
  CS_ExpressionBasis* getLoopCondition() { return loopCondition_;}
  const CS_ExpressionBasis* getLoopCondition() const { return loopCondition_;}

  /** sets the loop's initial expression that sets the genvar */
  void setLoopCondition(CS_ExpressionBasis* cond) { loopCondition_ = cond; }


  /** get the expression that modifies (increments) the genvar*/
  CS_ExpressionBasis* getLoopIncrement() { return incementExpression_;}
  const CS_ExpressionBasis* getLoopIncrement() const { return incementExpression_;}

  /** sets the increment expression of the genvar */
  void setLoopIncrement(CS_ExpressionBasis* increment) { incementExpression_ = increment; }

  /** returns the genvar of the for loop */
  CS_VariableDeclaration* getGenerateVariable() { return  variableDeclr_; }
  const CS_VariableDeclaration* getGenerateVariable() const { return  variableDeclr_; }


  /** method for the tree traversal */
  virtual void Accept(CS_CompilerVisitorBasis &visitor)
  { visitor.Visit( (CS_AnalogLoopGenerateStatement &)(*this)); }

private:

  /** the body of the loop that also includes
   * in case of FOR as last statement */
  CS_AnalogStatement* loopBody_;

  /** initial statement, used only for FOR loops, for WHILE this is null */
  CS_ExpressionBasis* initialStatement_;

  /** condition of the loop */
  CS_ExpressionBasis* loopCondition_;

  /** the expression which changes (increments) the genvar */
  CS_ExpressionBasis* incementExpression_;

  /** reference to the genvar */
  CS_VariableDeclaration* variableDeclr_;

};

#endif /* CS_ANALOGLOOPGENERATESTATEMENT_HPP_ */
