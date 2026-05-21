/*
 * CS_IndexedIdentifier.hpp
 *
 *  Created on: Dec 13, 2012
 *      Author: benk
 */

#ifndef INDEXEDIDENTIFIER_HPP_
#define INDEXEDIDENTIFIER_HPP_

#include "CS_ExpressionBasis.hpp"
#include "CS_Identifier.hpp"

/** class that models the A and A[i] constructs at a higher abstract level.
 * This class is inherited by 3 further classes (variable_reference,
 * parameter_reference, analog_port_reference)
 *
 * see the derived  classes of this class for LRM reference:
 * (variable_reference, parameter_reference, analog_port_reference)
 * */
class CS_IndexedIdentifier : public CS_ExpressionBasis
{
public:

  /** empty Ctor */
  CS_IndexedIdentifier(CS_Identifier *valObj,  CS_ExpressionBasis *indexExpr,
      const CS_ModuleDeclaration* module, int startLine, int startColumn);

  /** empty Dtor */
  virtual ~CS_IndexedIdentifier() {;}

   /** returns the classes that can be found in an expression (constant, ports, parameters, variables)*/
   virtual int getClasses() const { return valueObj_->getClass();}

   /** returns the types (actual INTEGER or REAL)*/
   virtual int getTypes() const {
     if (valueObj_->getSize() <= 1)
       return valueObj_->getType();
     else
       return ((indexExpr_!=NULL) ?
           (verilog_getScalarType((VarType)valueObj_->getType(),this)) : (valueObj_->getType()));
   }

   /** returns the type of the indexed identifier (actual INTEGER or REAL)*/
   VarType getType() const {
     if (valueObj_->getSize() <= 1)
       return valueObj_->getType();
     else
       return ((indexExpr_!=NULL) ?
           (verilog_getScalarType((VarType)valueObj_->getType(),this)) : (valueObj_->getType()));
   }

   /** returns the Value object */
   const CS_Identifier* getvalueObj() const { return valueObj_; }
   CS_Identifier* getvalueObj() { return valueObj_; }

   /** returns the expression in the index (this might be null if this is not indexed) */
    CS_ExpressionBasis* getIndexerExpr() const { return indexExpr_; }

   /** method for the tree traversal */
    virtual void Accept([[maybe_unused]] CS_CompilerVisitorBasis &visitor) {
      //visitor.Visit( (CS_IndexedIdentifier &)(*this));
      VERILOG_PRINT_L3(verb()," CS_IndexedIdentifier: this is an abstract class should not be called ");
      VERILOG_ERROR_STOP("Internal Error", this);
    }

    /** in case of constant indexing we compute and store this integer number that is returned by this function
     * if the indexExpr_ is not constant (can not be evaluated by the compuler) than returns -1*/
    int getConstIndexValue() const { return constIndexVal_; }

    /** returns true if this is a constant indexed expression, false otherwise */
    bool isConstIndexingExpression() const { return isConstantIndexingExpression_; }

protected:

   /** the value (port or variable)*/
    CS_Identifier *valueObj_;

   /** this expression represents the index []*/
    CS_ExpressionBasis *indexExpr_;

   /** if the indexer expression is not null and constant
    * then we might compute and store this constant index*/
    int constIndexVal_;

    /** flag indicates if one can use the getConstIndexValue function */
    bool isConstantIndexingExpression_;
};

#endif /* INDEXEDIDENTIFIER_HPP_ */
