/*
 * CS_AnalogConditionalStatement.hpp
 *
 *  Created on: Dec 19, 2012
 *      Author: benk
 */

#ifndef CS_ANALOGCONDITIONALSTATEMENT_HPP_
#define CS_ANALOGCONDITIONALSTATEMENT_HPP_

#include "CS_AnalogStatement.hpp"
#include "CS_AnalogSeqBlock.hpp"
#include "CS_ExpressionBasis.hpp"

/** class for IF () ... ELSE ... construct in Verilog-A
 *
 * see analog_conditional_statement in LRM */
class CS_AnalogConditionalStatement : public CS_AnalogStatement
{
public:

  CS_AnalogConditionalStatement(const CS_ModuleDeclaration* module, int startLine, int startColumn,
      CS_ExpressionBasis *condition, CS_AnalogStatement *trueBranch,  CS_AnalogStatement *falseBranch = 0);

  virtual ~CS_AnalogConditionalStatement() {;}

  /** returns the expression in the condition */
  CS_ExpressionBasis* getExpr() { return condition_; }

  /** returns the true branch instructions */
  CS_AnalogStatement*  getTrueBranch() { return trueBranch_; }
  CS_AnalogStatement** getTrueBranchPointer() { return &(trueBranch_); }

  /** returns the false branch instructions (this might be null if there is no else part)*/
  CS_AnalogStatement*  getFalseBranch() { return falseBranch_; }
  CS_AnalogStatement** getFalseBranchPointer() { return &(falseBranch_); }

  /** method for the tree traversal */
  virtual void Accept(CS_CompilerVisitorBasis &visitor) { visitor.Visit( (CS_AnalogConditionalStatement &)(*this)); }

private :

  /** condition expression in the if */
  CS_ExpressionBasis *condition_;

  /** the operation(s) in the true case */
  CS_AnalogStatement *trueBranch_;

  /** the operation(s) in the false case (null if there is no false branch)*/
  CS_AnalogStatement *falseBranch_;

};

#endif /* CS_ANALOGCONDITIONALSTATEMENT_HPP_ */
