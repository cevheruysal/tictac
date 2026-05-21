/*
 * CS_IndirectContributionStatement.hpp
 *
 *  Created on: Apr 9, 2013
 *      Author: benk
 */

#ifndef CS_INDIRECTCONTRIBUTIONSTATEMENT_HPP_
#define CS_INDIRECTCONTRIBUTIONSTATEMENT_HPP_

#include "CS_AnalogStatement.hpp"
#include "CS_BranchProbeFunction.hpp"
#include "CS_ExpressionBasis.hpp"

/** class to represent indirect Contributions e.g., V(n) : V(p) == exp(variable)
 * This statement has three (two) parts
 *
 * see indirect_contribution_statement in LRM */
class CS_IndirectContributionStatement : public CS_AnalogStatement
{
public:

  /** Ctor of the indirect contribution statement */
  CS_IndirectContributionStatement(const CS_ModuleDeclaration* module,
      int startLine, int startColumn,
      CS_BranchProbeFunction *lvalue, CS_ExpressionBasis *indirectExpr,
      CS_ExpressionBasis *rExpr, std::string attribStr);

  /** Empty Dtor */
  virtual ~CS_IndirectContributionStatement() {;}

  virtual bool isElementaryInsruction() const { return true; }

  CS_BranchProbeFunction* getLeftExpr() const { return  lvalue_; }

  CS_ExpressionBasis* getInDirectExpression() const { return indirectExpr_; }

  CS_ExpressionBasis* getRightExpression() const { return rExpr_; }

  /** method for the tree traversal */
  virtual void Accept(CS_CompilerVisitorBasis &visitor)
  { visitor.Visit( (CS_IndirectContributionStatement &)(*this)); }

  /** returns the attribute string */
  const std::string& getAttribString() const { return attribStr_; }

private:

  /** the most left expression of the statement,
   * which must be a branch probe function */
  CS_BranchProbeFunction *lvalue_;

  /** the "middle" expression that is either branch probe, ddt(branch probe)
   * or idt(branch probe)*/
  CS_ExpressionBasis *indirectExpr_;

  /** the right expression that can be a general analog expression*/
  CS_ExpressionBasis *rExpr_;

  /** we store some attributes in this class w.r.t. the "middle" expression*/
  std::string attribStr_;

};

#endif /* CS_INDIRECTCONTRIBUTIONSTATEMENT_HPP_ */
