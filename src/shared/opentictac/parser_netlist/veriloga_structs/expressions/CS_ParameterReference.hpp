/*
 * CS_ParameterReference.hpp
 *
 *  Created on: Jan 10, 2013
 *      Author: benk
 */

#ifndef CS_PARAMETERREFERENCE_HPP_
#define CS_PARAMETERREFERENCE_HPP_

#include "CS_Parameter.hpp"
#include "CS_IndexedIdentifier.hpp"

/** class that models the A and A[i] constructs where A is a parameter.
 *
 *  IMPORTANT NOTE: A[i] is not allowed in analog, for this cased there will be an error thrown
 *
 * see parameter_reference in LRM
 * */
class CS_ParameterReference: public CS_IndexedIdentifier
{
public:

  /** empty Ctor */
  CS_ParameterReference(CS_Parameter *parObj,  CS_ExpressionBasis *indexExpr,
      const CS_ModuleDeclaration* module, int startLine, int startColumn);

  /** empty Dtor */
  virtual  ~CS_ParameterReference() {;}

  /** returns the parameter object */
  CS_Parameter* getParamObject() { return parObj_; }
  const CS_Parameter* getParamObject() const { return parObj_; }

  /** method for the tree traversal */
  virtual void Accept(CS_CompilerVisitorBasis &visitor) { visitor.Visit( (CS_ParameterReference &)(*this)); }

  /** Infinity should not appear in expressions */
  virtual CS_AKDependency propagateDependencies()
    {
      // store and return empty dependencies
      CS_AKDependency ret_tmp;
      setActVarDependency(ret_tmp);
      return ret_tmp;
    }

  /** function that creates a deep copy of the expression itself */
  virtual CS_ExpressionBasis* copy() {
    // create new object
    return (new CS_ParameterReference( parObj_,
        (this->getIndexerExpr())?this->getIndexerExpr()->copy():0 ,this->getModule(),
        this->getStartLine(), this->getStartColumn() ));
  }

  /** the size of the expression */
  virtual int getExprSize() const { return parObj_->getSize();}

private:

  /** pointer to the parameter object, this is a local copy of the object in the super class */
  CS_Parameter *parObj_;
};

#endif /* CS_PARAMETERREFERENCE_HPP_ */
