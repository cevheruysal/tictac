/*
 * CS_VariableReference.hpp
 *
 *  Created on: Jan 10, 2013
 *      Author: benk
 */

#ifndef CS_VARIABLEREFERENCE_HPP_
#define CS_VARIABLEREFERENCE_HPP_

#include "CS_VariableDeclaration.hpp"
#include "CS_IndexedIdentifier.hpp"

/** class that models the A and A[i] constructs where A is a variable
 *
 * see variable_reference in LRM
 * */

class CS_VariableReference : public CS_IndexedIdentifier
{
public:

  /** empty Ctor */
  CS_VariableReference(CS_VariableDeclaration *varObj,  CS_ExpressionBasis *indexExpr,
      const CS_ModuleDeclaration* module, int startLine, int startColumn, int derivativeIndex = -1);

  /** empty Dtor */
  virtual ~CS_VariableReference() {;}

  /** returns the variable object from the expressions */
  const CS_VariableDeclaration* getVariableObj() const { return varObj_; }
  CS_VariableDeclaration* getVariableObj() { return varObj_; }

  /** constant evaluation, used only for generate loop expansion */
  virtual double evalConst() const;

  /** method for the tree traversal */
  virtual void Accept(CS_CompilerVisitorBasis &visitor) { visitor.Visit( (CS_VariableReference &)(*this)); }

  /** String should not appear in composed expressions */
  virtual CS_AKDependency propagateDependencies();

  /** method for recursive differentiation with AD*/
  virtual CS_ExpressionBasis* doDifferentiate(int sysVarIndex, int *flag);

  /** function that creates a deep copy of the expression itself */
  virtual CS_ExpressionBasis* copy() ;

  /** @returns the derivative index. If this reference is within a derivative
   * expression then we do not need to return the name of the variable but the
   * derivative w.r.t. some other variable */
  int getDerivativeIndex() const { return derivativeIndex_; }

  /** the size of the expression */
  virtual int getExprSize() const { return varObj_->getSize();}

private:

  /** pointer to the variable, this is a local copy of the pointer in the super class */
  CS_VariableDeclaration *varObj_;

  /** this integer indicates, if this expression stays in a derivative expression
   * if it is -1 no derivative if >= 0 then this is part of a derivative expression */
  int derivativeIndex_;

};

#endif /* CS_VARIABLEREFERENCE_HPP_ */
