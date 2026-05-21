/*
 * CS_ContributionStatement.hpp
 *
 *  Created on: Dec 19, 2012
 *      Author: benk
 */

#ifndef CONTRIBUTIONSTATEMENT_HPP_
#define CONTRIBUTIONSTATEMENT_HPP_

#include "CS_AnalogStatement.hpp"
#include "CS_BranchProbeFunction.hpp"
#include "CS_ExpressionBasis.hpp"

/** class to represent the instruction e.g., I(p,m) <+ ...
 * instructions that adds a contribution to the pin of a modeled device
 *
 * see contribution_statement in LRM */
class CS_ContributionStatement : public CS_AnalogStatement
{
public:

  /** */
  CS_ContributionStatement(const CS_ModuleDeclaration* module, int startLine, int startColumn,
      CS_BranchProbeFunction *targetE, CS_ExpressionBasis *evalExp );

  /** empty Dtor */
  virtual ~CS_ContributionStatement() {;}

  /** returns true since this is elementary instruction*/
  virtual bool isElementaryInsruction() const { return true; }

  /** returns the left part of the instruction e.g. I(p,n) */
  CS_BranchProbeFunction* getTargetExpr() const { return  targetE_; }

  /** returns the right part of the instruction */
  CS_ExpressionBasis* getExpression() const { return evalExp_; }

  /** sets the expression of the contribution */
  void setExpression(CS_ExpressionBasis* e) { evalExp_ = e; }

  /** method for the tree traversal */
  virtual void Accept(CS_CompilerVisitorBasis &visitor) { visitor.Visit( (CS_ContributionStatement &)(*this)); }

private :

  /** expression contains the branch */
  CS_BranchProbeFunction *targetE_;

  /** expression that defines the contribution */
  CS_ExpressionBasis *evalExp_;
};

#endif /* CONTRIBUTIONSTATEMENT_HPP_ */
