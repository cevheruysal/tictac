/*
 * CS_AnalogFilterOperationFunction.hpp
 *
 *  Created on: Dec 14, 2012
 *      Author: benk
 */

#ifndef CS_ANALOGFILTEROPERATIONFUNCTION_HPP_
#define CS_ANALOGFILTEROPERATIONFUNCTION_HPP_

#include "CS_FunctionCall.hpp"

/** various types of the analog operation function */
typedef enum {
    VERILOG_ANOPFUNC_DDT           = 0,  // "ddt" ;
    VERILOG_ANOPFUNC_IDT           = 1,  //"idt" ;
    VERILOG_ANOPFUNC_IDTMOD        = 2,  //"idtmod" ;
    VERILOG_ANOPFUNC_DDX           = 3,  //"ddx" ;
    VERILOG_ANOPFUNC_LIMEXP        = 4,  //"limexp" ;
    VERILOG_ANOPFUNC_ABSDELAY      = 5,  //"absdelay" ;
    VERILOG_ANOPFUNC_LAPLACE_ND    = 6,  //"laplace_nd" ;
    VERILOG_ANOPFUNC_LAPLACE_NP    = 7,  //"laplace_np" ;
    VERILOG_ANOPFUNC_LAPLACE_ZD    = 8,  //"laplace_zd" ;
    VERILOG_ANOPFUNC_LAPLACE_ZP    = 9,  //"laplace_zp" ;
    VERILOG_ANOPFUNC_TRANSITION    = 10,  //"transition" ;
    VERILOG_ANOPFUNC_SLEW          = 11,  //"slew" ;
    VERILOG_ANOPFUNC_ZI_ND         = 12,  //"zi_nd" ;
    VERILOG_ANOPFUNC_ZI_NP         = 13,  //"zi_np" ;
    VERILOG_ANOPFUNC_ZI_ZD         = 14, //"zi_zd" ;
    VERILOG_ANOPFUNC_ZI_ZP         = 15, //"zi_zp" ;
    VERILOG_ANOPFUNC_LAST_CROSSING = 16,  //"last_crossing" ;
    VERILOG_ANOPFUNC_LIMIT            = 17, // "limit"
    VERILOG_ANOPFUNC_TABLE            = 18, // "table_mode"
} AnalogOpFunctionType;

/** class to model the analog operation functions
 *
 * see analog_filter_function_call in LRM */
class CS_AnalogFilterOperationFunction : public CS_FunctionCall
{
public:

  /** Ctor with the type of the function and with the argument list */
  CS_AnalogFilterOperationFunction( const AnalogOpFunctionType opType,
      const CS_ModuleDeclaration* module, int startLine, int startColumn,
      CS_ExpressionBasis** args, int nrArgs);

  virtual ~CS_AnalogFilterOperationFunction() { ; }

  virtual int getClasses() const;

  virtual int getTypes() const;

  /** method for the tree traversal */
  virtual void Accept(CS_CompilerVisitorBasis &visitor) { visitor.Visit( (CS_AnalogFilterOperationFunction &)(*this)); }

  /** returns the type of this function */
  AnalogOpFunctionType getFunctionType() const { return opType_; }

  /** propagate recursively the system dependency informations */
  virtual CS_AKDependency propagateDependencies();

  /** method for recursive differentiation with AD*/
  virtual CS_ExpressionBasis* doDifferentiate(int sysVarIndex, int *flag);

  /** function that creates a deep copy of the expression itself */
  virtual CS_ExpressionBasis* copy() {
    // copy the arguments
    CS_ExpressionBasis *arg[10];
    for (int i = 0; i < this->getNrArg(); i++)
      if (getArg(i))
        arg[i] = this->getArg(i)->copy();
      else
        arg[i] = 0;
    // create new object
    return (new CS_AnalogFilterOperationFunction( opType_,  this->getModule(),
        this->getStartLine(), this->getStartColumn(), arg, this->getNrArg()));
  }

  /** @returns in case of DDX returns the derivative expression,
   * otherwise it is null (alse when the DDX operation is zero)*/
  CS_ExpressionBasis* getDDXExpr() { return ddx_Deriv_expr_; }
  const CS_ExpressionBasis* getDDXExpr() const { return ddx_Deriv_expr_; }

private:

  /** type of the function */
  const AnalogOpFunctionType opType_;

  /** this is only used to store the derivative expression within
   * the DDX opeator */
  CS_ExpressionBasis* ddx_Deriv_expr_;

};

#endif /* CS_ANALOGFILTEROPERATIONFUNCTION_HPP_ */
