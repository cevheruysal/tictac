/*
 * CS_AnalogBuiltInMathFunction.hpp
 *
 *  Created on: Dec 14, 2012
 *      Author: benk
 */

#ifndef CS_ANALOGBUILDINTMATHFUNCTION_HPP_
#define CS_ANALOGBUILDINTMATHFUNCTION_HPP_

#include "CS_FunctionCall.hpp"

typedef enum {
    VERILOG_FUNCMATH_ABS = 0, //"abs" ;
    VERILOG_FUNCMATH_CEIL = 1, //"ceil" ;
    VERILOG_FUNCMATH_EXP_FUNC = 2,  //"exp" ;
    VERILOG_FUNCMATH_FLOOR = 3, //"floor" ;
    VERILOG_FUNCMATH_HYPOT = 4, //"hypot" ;
    VERILOG_FUNCMATH_LN = 5,  //"ln" ;
    VERILOG_FUNCMATH_LOG = 6,  //"log" ;
    VERILOG_FUNCMATH_MAX = 7,  //"max" ;
    VERILOG_FUNCMATH_MINIMUM = 8,  //"min" ;
    VERILOG_FUNCMATH_POW  = 9,  //"pow" ;
    VERILOG_FUNCMATH_SQRT = 10,  //"sqrt" ;
    VERILOG_FUNCMATH_LIMIT_TO_EPSILON = 11, //function with 'x < eps' are shielded (eps);
                                            //(needed for safe derivatives)
    VERILOG_FUNCMATH_ACOS = 12,  //"acos" ;
    VERILOG_FUNCMATH_ACOSH = 13,  //"acosh" ;
    VERILOG_FUNCMATH_ASIN  = 14,  //"asin" ;
    VERILOG_FUNCMATH_ASINH = 15,  //"asinh" ;
    VERILOG_FUNCMATH_ATAN  = 16,  //"atan" ;
    VERILOG_FUNCMATH_ATANH = 17,  //"atanh" ;
    VERILOG_FUNCMATH_ATAN2 = 18,  //"atan2" ;
    VERILOG_FUNCMATH_COS = 19,  //"cos" ;
    VERILOG_FUNCMATH_COSH = 20,  //"cosh" ;
    VERILOG_FUNCMATH_SIN = 22,  //"sin" ;
    VERILOG_FUNCMATH_SINH = 23,  //"sinh" ;
    VERILOG_FUNCMATH_TAN = 24,  //"tan" ;
    VERILOG_FUNCMATH_TANH = 25,  //"tanh" ;
    VERILOG_FUNCMATH_CLOG2 = 26  //"log" ;
} MathFunctionType;

/** class to model the mathematical function call
 *
 * see analog_built_in_function_call in LRM */
class CS_AnalogBuiltInMathFunction : public CS_FunctionCall
{
public:

  /** Ctor for math functions with one/two arguments
   * for functions which should have only one argument the second argument pointer is null */
  CS_AnalogBuiltInMathFunction(MathFunctionType fT, CS_ExpressionBasis** args, int nrFunc,
      const CS_ModuleDeclaration* module, int startLine, int startColumn);

  /** empty Dtor */
  virtual ~CS_AnalogBuiltInMathFunction() {;}

  virtual int getClasses() const;

  virtual int getTypes() const;

  virtual double evalConst() const;

  /** method for the tree traversal */
  virtual void Accept(CS_CompilerVisitorBasis &visitor) { visitor.Visit( (CS_AnalogBuiltInMathFunction &)(*this)); }

  /** propagate recursively the system dependency informations */
  virtual CS_AKDependency propagateDependencies();

  /** method for recursive differentiation with AD*/
  virtual CS_ExpressionBasis* doDifferentiate(int sysVarIndex, int *flag);

  /** returns the returned type of the function */
  VarType getFunctionRetType() const { return functionRetType_; }

  /** returns the type of the mathematical function */
  MathFunctionType getFunctionType() const { return functionType_; }

  /** function that creates a deep copy of the expression itself */
  virtual CS_ExpressionBasis* copy() {
    // copy the arguments
    CS_ExpressionBasis *arg[10];
    for (int i = 0; i < this->getNrArg(); i++)
      if (this->getArg(i))
        arg[i] = this->getArg(i)->copy();
      else
        arg[i] = 0;
    // create new object
    return (new CS_AnalogBuiltInMathFunction(functionType_, arg,
        this->getNrArg(), this->getModule(),
        this->getStartLine(), this->getStartColumn()));
  }

private :

  MathFunctionType functionType_;

  VarType functionRetType_;
};

#endif /* CS_ANALOGBUILDINTMATHFUNCTION_HPP_ */
