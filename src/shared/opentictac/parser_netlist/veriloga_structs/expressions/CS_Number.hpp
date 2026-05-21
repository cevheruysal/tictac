/*
 * CS_Number.hpp
 *
 *  Created on: Nov 30, 2012
 *      Author: benk
 */

#ifndef CS_NUMBER_HPP_
#define CS_NUMBER_HPP_

#include "CS_ExpressionBasis.hpp"

/** This class models all the constant numbers in the expressions
 *
 * see number in LRM */
class CS_Number : public CS_ExpressionBasis
{
public:

  /** Ctor for constant values */
  CS_Number(std::string value, const CS_ModuleDeclaration* module,
      int startLine, int startColumn, bool isOnlyString = false);

  /** Ctor with integer */
  CS_Number(int value, const CS_ModuleDeclaration* module,
      int startLine, int startColumn);

  /** Ctor with double */
  CS_Number(double value, const CS_ModuleDeclaration* module,
      int startLine, int startColumn);

  virtual ~CS_Number() {;}

  /** returns the class parameters */
  virtual int getClasses() const { return (int)CONSTANT; }

  /** return the type of the parameter */
  virtual int getTypes() const
  {
    // depending on the input format we have
    if (typeConst_ == REAL)
      return (int)REAL;
    else
      return (int)NO_TYPE;
  }

  virtual double evalConst() const
   {
    VERILOG_ERROR_COND_STOP( (typeConst_ == NO_TYPE) , "Internal error", this);
     if (typeConst_ == REAL)
       return valD_;
     else
      return (double)valI_;
   }

  /** method for the tree traversal */
  virtual void Accept(CS_CompilerVisitorBasis &visitor)
  {
    visitor.Visit( (CS_Number &)(*this));
  }

  /** Infinity should not appear in expressions */
  virtual CS_AKDependency propagateDependencies()
    {
      // store and return empty dependencies
      CS_AKDependency ret_tmp;
      setActVarDependency(ret_tmp);
      return ret_tmp;
    }

  /** function that creates a deep copy of the expression itself */
  virtual CS_ExpressionBasis* copy()
    {
      if (typeConst_ == INTEGER)
        // create new object
        return (new CS_Number(valI_, this->getModule(),
          this->getStartLine(), this->getStartColumn()));
      else
        // create new object
        return (new CS_Number(valD_, this->getModule(),
          this->getStartLine(), this->getStartColumn()));
    }

  /** @returns the internal string representation */
  const std::string& getInternatStringRep() const { return valueS_; }

  virtual int getExprSize() const { return 1;}

private:

  /** the integer value */
  int valI_;

  /** the double value */
  double valD_;

  /** the type of the constant */
  VarType typeConst_;

  /** the value in string format */
  std::string valueS_;

};

#endif /* CS_NUMBER_HPP_ */
