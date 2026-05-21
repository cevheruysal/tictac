/*
 * CS_ParameterDeclaration.hpp
 *
 *  Created on: Nov 28, 2012
 *      Author: benk
 */

#ifndef CS_PARAMETERDECLARATION_HPP_
#define CS_PARAMETERDECLARATION_HPP_

#include "verilog_comp.hpp"
#include "CS_ExpressionBasis.hpp"
#include "CS_ValueRange.hpp"
#include "CS_CompilerTreeNode.hpp"

class CS_Parameter;

/** Class to model the declaration of a parameter.
 * See parameter_declaration in LRM */
class CS_ParameterDeclaration : public CS_CompilerTreeNode
{
public:

  /** Ctor */
  CS_ParameterDeclaration(CS_Parameter* param, CS_ExpressionBasis* defValue = 0);

  /** empty Dtor */
  virtual ~CS_ParameterDeclaration() {;}

  /** adds a new constraint to the parameter declaration */
  void addConstraint(CS_ValueRange* newConstr) {
    constraints_.push_back( newConstr );
  }

  /** returns the name of the Parameter */
  const std::string getName() const;

  /** returns the parameter index */
  int getIndex() const;

  /** returns the parameter object */
  const CS_Parameter* getParam() const { return param_; }
  CS_Parameter* getParam() { return param_; }

  /** returns the number of constraints */
  int getNrConstr() const { return (int)constraints_.size(); }

  /** return the constraint at the given index */
  CS_ValueRange* getConstraint(int i) { return constraints_[i]; }
  const CS_ValueRange* getConstraint(int i) const { return constraints_[i]; }

  /** return the default value's expression */
  CS_ExpressionBasis* getDefaultExpr() { return defValue_; }
  const CS_ExpressionBasis* getDefaultExpr() const { return defValue_; }

  /** sets the deafult expression (memory cleanup is not the task of this function)*/
  void setDefaultExpression(CS_ExpressionBasis *defaultExpr) { defValue_ = defaultExpr; }

  /** method for the tree traversal */
  virtual void Accept(CS_CompilerVisitorBasis &visitor) { visitor.Visit( (CS_ParameterDeclaration &)(*this)); }

private:

  /** the parameter object */
   CS_Parameter* param_;

  /** the value assigned to the parameter by default */
   CS_ExpressionBasis* defValue_;

  /** the vector that contains all the constrains of a parameter  */
  std::vector<CS_ValueRange*> constraints_;

};

#endif /* CS_PARAMETERDECLARATION_HPP_ */
