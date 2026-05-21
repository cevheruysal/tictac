/*
 * CS_AnalogConditionalExpression.hpp
 *
 *  Created on: Dec 12, 2012
 *      Author: benk
 */

#ifndef ANALOGCONDITIONALEXPRESSION_HPP_
#define ANALOGCONDITIONALEXPRESSION_HPP_

#include "CS_ExpressionBasis.hpp"

/** the class to model the (..)?...:... construct as an expression
 *
 * see analog_conditional_expression in LRM */
class CS_AnalogConditionalExpression : public CS_ExpressionBasis
{
public:

   /** Ctor with the required expressions */
   CS_AnalogConditionalExpression(CS_ExpressionBasis *ifExpr, CS_ExpressionBasis *thenExpr, CS_ExpressionBasis *elseExpr,
      const CS_ModuleDeclaration* module, int startLine, int startColumn);

   virtual ~CS_AnalogConditionalExpression() {;}

   /** */
   virtual int getClasses() const { return  (int)(thenExpr_->getClasses() | elseExpr_->getClasses()); }

   /** */
   virtual int getTypes() const { return (int)(ifExpr_->getTypes() | thenExpr_->getTypes() | elseExpr_->getTypes()); }

   virtual double evalConst() const;

   /** Condition of the expression */
   CS_ExpressionBasis* getCondition() { return ifExpr_; }

   /** the then expression in */
   CS_ExpressionBasis* getThenExpr() { return thenExpr_; }

   /** the else expression */
   CS_ExpressionBasis* getElseExpr() { return elseExpr_; }

   /** method for the tree traversal */
   virtual void Accept(CS_CompilerVisitorBasis &visitor) { visitor.Visit( (CS_AnalogConditionalExpression &)(*this)); }

   /** propagate recursively the system dependency informations */
   virtual CS_AKDependency propagateDependencies();

   /** method for recursive differentiation with AD*/
   virtual CS_ExpressionBasis* doDifferentiate(int sysVarIndex, int *flag);

   /** function that creates a deep copy of the expression itself */
   virtual CS_ExpressionBasis* copy() {
     // copy the arguments
     CS_ExpressionBasis *cond = ifExpr_->copy();
     CS_ExpressionBasis *tr = thenExpr_->copy();
     CS_ExpressionBasis *fl = (elseExpr_ != 0) ? elseExpr_->copy() : 0;
     // create new object
     return (new CS_AnalogConditionalExpression(cond,tr, fl, this->getModule(),
         this->getStartLine(), this->getStartColumn()));
   }

private:

    CS_ExpressionBasis *ifExpr_;

    CS_ExpressionBasis *thenExpr_;

    CS_ExpressionBasis *elseExpr_;

};

#endif /* ANALOGCONDITIONALEXPRESSION_HPP_ */
