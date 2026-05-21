/*
 * CS_AnalogBuiltInMathFunction.cpp
 *
 *  Created on: Dec 14, 2012
 *      Author: benk
 */

#include "CS_AnalogBuiltInMathFunction.hpp"

#include "CS_BinaryOperator.hpp"
#include "CS_Number.hpp"
#include "CS_AnalogConditionalExpression.hpp"
#include "CS_UnaryOperator.hpp"

#define IS_OPTIMIZED_EXPRESSION UT_GeneralConfiguration::doExprOptim()

#include <math.h>

CS_AnalogBuiltInMathFunction::CS_AnalogBuiltInMathFunction(
    MathFunctionType fT, CS_ExpressionBasis** args, int nrFunc,
    const CS_ModuleDeclaration* module, int startLine, int startColumn):
    CS_FunctionCall(args, nrFunc, module, startLine, startColumn, false), functionType_(fT)
{
  for (int i = 0 ; i < this->getNrArg() ; i++)
    VERILOG_ERROR_COND_STOP( !this->getArg(i), "This function can not have default arguments", this);

  // check the return type of the function
  int tmp_nrArg = -1;
  VarType tmp_functionType = NO_TYPE;

  switch (functionType_){
    case VERILOG_FUNCMATH_ABS: { tmp_nrArg = 1; break;} //"abs" ;
    case VERILOG_FUNCMATH_CEIL: { tmp_nrArg = 1; tmp_functionType = REAL; break;} //"ceil" ;
    case VERILOG_FUNCMATH_EXP_FUNC : { tmp_nrArg = 1; tmp_functionType = REAL; break;} //"exp" ;
    case VERILOG_FUNCMATH_FLOOR : { tmp_nrArg = 1; tmp_functionType = REAL; break;} //"floor" ;
    case VERILOG_FUNCMATH_HYPOT : { tmp_nrArg = 2; tmp_functionType = REAL; break;} //"hypot" ;
    case VERILOG_FUNCMATH_LN : { tmp_nrArg = 1; tmp_functionType = REAL; break;} //"ln" ;
    case VERILOG_FUNCMATH_LOG : { tmp_nrArg = 1; tmp_functionType = REAL; break;} //"log" ;
    case VERILOG_FUNCMATH_MAX : { tmp_nrArg = 2; break;}  //"max" ;
    case VERILOG_FUNCMATH_MINIMUM : { tmp_nrArg = 2; break;}   //"min" ;
    case VERILOG_FUNCMATH_POW  : { tmp_nrArg = 2; tmp_functionType = REAL; break;}   //"pow" ;
    case VERILOG_FUNCMATH_SQRT : { tmp_nrArg = 1; tmp_functionType = REAL; break;}   //"sqrt" ;
    case VERILOG_FUNCMATH_LIMIT_TO_EPSILON : { tmp_nrArg = 1; tmp_functionType = REAL; break;}   //"my_limit" ;
    case VERILOG_FUNCMATH_ACOS : { tmp_nrArg = 1; tmp_functionType = REAL; break;}   //"acos" ;
    case VERILOG_FUNCMATH_ACOSH : { tmp_nrArg = 1; tmp_functionType = REAL; break;}   //"acosh" ;
    case VERILOG_FUNCMATH_ASIN  : { tmp_nrArg = 1; tmp_functionType = REAL; break;}   //"asin" ;
    case VERILOG_FUNCMATH_ASINH : { tmp_nrArg = 1; tmp_functionType = REAL; break;}   //"asinh" ;
    case VERILOG_FUNCMATH_ATAN  : { tmp_nrArg = 1; tmp_functionType = REAL; break;}   //"atan" ;
    case VERILOG_FUNCMATH_ATANH : { tmp_nrArg = 1; tmp_functionType = REAL; break;}   //"atanh" ;
    case VERILOG_FUNCMATH_ATAN2 : { tmp_nrArg = 2; tmp_functionType = REAL; break;}   //"atan2" ;
    case VERILOG_FUNCMATH_COS : { tmp_nrArg = 1; tmp_functionType = REAL; break;}   //"cos" ;
    case VERILOG_FUNCMATH_COSH : { tmp_nrArg = 1; tmp_functionType = REAL; break;}   //"cosh" ;
    case VERILOG_FUNCMATH_SIN : { tmp_nrArg = 1; tmp_functionType = REAL; break;}   //"sin" ;
    case VERILOG_FUNCMATH_SINH : { tmp_nrArg = 1; tmp_functionType = REAL; break;}   //"sinh" ;
    case VERILOG_FUNCMATH_TAN : { tmp_nrArg = 1; tmp_functionType = REAL; break;}   //"tan" ;
    case VERILOG_FUNCMATH_TANH : { tmp_nrArg = 1; tmp_functionType = REAL; break;}   //"tanh" ;
    case VERILOG_FUNCMATH_CLOG2 : { tmp_nrArg = 1; break;}  // clog2
    default: { /** this should not occur */ break;}
  }
  // test the number of arguments
  VERILOG_ERROR_COND_STOP( tmp_nrArg != getNrArg() , " Function should take " <<
      tmp_nrArg << " arguments but it has " << getNrArg(), this);
  // store the ret type of the function
  this->functionRetType_ = tmp_functionType;
}


int CS_AnalogBuiltInMathFunction::getClasses() const{
  if (nrArgs_ > 1)
    return (args_[0]->getClasses() | args_[1]->getClasses());
  else
    return args_[0]->getClasses();
}

int CS_AnalogBuiltInMathFunction::getTypes() const {
  if (nrArgs_ > 1)
    return ( functionRetType_ | args_[0]->getTypes() | args_[1]->getTypes() );
  else
    return ( functionRetType_ | args_[0]->getTypes() );
}

double CS_AnalogBuiltInMathFunction::evalConst() const {
  switch (functionType_){
    case VERILOG_FUNCMATH_ABS: { return fabs(args_[0]->evalConst()); break;} //"abs" ;
    case VERILOG_FUNCMATH_CEIL: { return ceil(args_[0]->evalConst()); break;} //"ceil" ;
    case VERILOG_FUNCMATH_EXP_FUNC : { return exp(args_[0]->evalConst()); break;} //"exp" ;
    case VERILOG_FUNCMATH_FLOOR : { return floor(args_[0]->evalConst()); break;} //"floor" ;
    case VERILOG_FUNCMATH_HYPOT : { return hypot(args_[0]->evalConst(), args_[1]->evalConst()); break;} //"hypot" ;
    case VERILOG_FUNCMATH_LN : { return log(args_[0]->evalConst()); break;} //"ln" ;
    case VERILOG_FUNCMATH_LOG : { return log10(args_[0]->evalConst()); break;} //"log" ;
    case VERILOG_FUNCMATH_MAX : { //"max" ; // fmax does not exist on solaris
      double d1 = args_[0]->evalConst();
      double d2 = args_[1]->evalConst();
      return (d1>d2)?d1:d2;
    }
    case VERILOG_FUNCMATH_MINIMUM : { //"min" ; // fmin does not exists on solaris
      double d1 = args_[0]->evalConst();
      double d2 = args_[1]->evalConst();
      return (d1>d2)?d2:d1;
    }
    case VERILOG_FUNCMATH_POW  : {
      double t1 = args_[0]->evalConst();
      double t2 = args_[1]->evalConst();
      return (pow(t1, t2)); break;
    }   //"pow" ;
    case VERILOG_FUNCMATH_SQRT : {
      double t1 = args_[0]->evalConst();
      return ((t1 > VERILOG_MY_EPSILON) ? (sqrt(t1)) : (sqrt(VERILOG_MY_EPSILON))); break;
    }   //"sqrt" ;
    case VERILOG_FUNCMATH_LIMIT_TO_EPSILON: {
      double t = args_[0]->evalConst();
      return ((t > VERILOG_MY_EPSILON) ? t : VERILOG_MY_EPSILON); }
    case VERILOG_FUNCMATH_ACOS : { return acos(args_[0]->evalConst()); break;}   //"acos" ;
    case VERILOG_FUNCMATH_ACOSH : { return acosh(args_[0]->evalConst()); break;}   //"acosh" ;
    case VERILOG_FUNCMATH_ASIN  : { return asin(args_[0]->evalConst()); break;}   //"asin" ;
    case VERILOG_FUNCMATH_ASINH : { return asinh(args_[0]->evalConst()); break;}   //"asinh" ;
    case VERILOG_FUNCMATH_ATAN  : { return atan(args_[0]->evalConst()); break;}   //"atan" ;
    case VERILOG_FUNCMATH_ATANH : { return atanh(args_[0]->evalConst()); break;}   //"atanh" ;
    case VERILOG_FUNCMATH_ATAN2 : { return atan2(args_[0]->evalConst(), args_[1]->evalConst()); break;}   //"atan2" ;
    case VERILOG_FUNCMATH_COS : { return cos(args_[0]->evalConst()); break;}   //"cos" ;
    case VERILOG_FUNCMATH_COSH : { return cosh(args_[0]->evalConst()); break;}   //"cosh" ;
    case VERILOG_FUNCMATH_SIN : { return sin(args_[0]->evalConst()); break;}   //"sin" ;
    case VERILOG_FUNCMATH_SINH : { return sinh(args_[0]->evalConst()); break;}   //"sinh" ;
    case VERILOG_FUNCMATH_TAN : { return tan(args_[0]->evalConst()); break;}   //"tan" ;
    case VERILOG_FUNCMATH_TANH : { return tanh(args_[0]->evalConst()); break;}   //"tanh" ;
    case VERILOG_FUNCMATH_CLOG2 :    { return ceil(log( args_[0]->evalConst())*1.44269504088896); break; } // clog2
    default: { /** this should not occur */ return 0.0;}
  }
  return 0.0;
}


CS_AKDependency CS_AnalogBuiltInMathFunction::propagateDependencies()
{
  // propagate the dependencies
  CS_AKDependency ret_set;

  // for some functions we cancel the dependencies
  switch (functionType_){
    case VERILOG_FUNCMATH_FLOOR :
    case VERILOG_FUNCMATH_CEIL:
    case VERILOG_FUNCMATH_CLOG2 :  {
      return ret_set;
    }
    default: { /* nothing to do */ break;}
  }

  if (this->getNrArg() == 1)
    { // one argument
      ret_set = this->getArg(0)->propagateDependencies();
    }
  else
    { // two arguments
      CS_AKDependency tmpSet1 = this->getArg(0)->propagateDependencies();
      ret_set = this->getArg(1)->propagateDependencies();
      ret_set.uniteDependencies( tmpSet1 );
    }
  // store the combined dependency in the actual node
  setActVarDependency(ret_set);
  // return the combined dependency list
  return ret_set;
}


CS_ExpressionBasis* CS_AnalogBuiltInMathFunction::doDifferentiate(int sysVarIndex, int *flag)
{
  // test if this dependent on the chosen system variable
  if (this->getActVarDependency().isSysVarIndexPresent(sysVarIndex) < 0)
    return 0;

  // if we reach the switch statement than we have dependency w.r.t. to this system variable
  CS_ExpressionBasis* retExp = 0;

  CS_ExpressionBasis *arg1 = this->getArg(0);
  CS_ExpressionBasis *arg2 = this->getArg(1);
  CS_ExpressionBasis *arg1Deriv = 0;
  CS_ExpressionBasis *arg2Deriv = 0;
  // compute the derivatives of the arguments
  if (arg1->getActVarDependency().isSysVarIndexPresent(sysVarIndex) >= 0)
    arg1Deriv = arg1->doDifferentiate(sysVarIndex, flag);
  if (arg2)
    if (arg2->getActVarDependency().isSysVarIndexPresent(sysVarIndex) >= 0)
      arg2Deriv = arg2->doDifferentiate(sysVarIndex, flag);

  VERILOG_PRINT_L3( verb(), " DIFF ANAL " << functionType_ );

  //  ------- ALL CASES ARE COMPLETE ------------------
  switch (functionType_){

    case VERILOG_FUNCMATH_ABS: { //"abs" ;
      // abs(x)' = (x>0)?x':-x';
      CS_ExpressionBasis *zero = new CS_Number(0.0, getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis *ifExpr = new CS_BinaryOperator( VERILOG_BINAR_GT, arg1, zero,
          getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis *elseExpr =
          new CS_UnaryOperator( VERILOG_UNAR_MIN, arg1Deriv, getModule(), getStartLine(), getStartColumn());
      // the returned expression is created
      retExp = new CS_AnalogConditionalExpression(ifExpr, arg1Deriv, elseExpr,
          getModule(), getStartLine(), getStartColumn());
      arg1->setTwinSelf("abs_arg");  // for efficient derivative computation
      return retExp;
    }

    case VERILOG_FUNCMATH_EXP_FUNC : { //"exp" ;
      // exp(x)' = exp(x)*x'
      // the returned expression is created
      retExp = new CS_BinaryOperator( VERILOG_BINAR_MUL, this, arg1Deriv,
          getModule(), getStartLine(), getStartColumn());
      this->setTwinSelf("exp_func"); // for efficient derivative computation
      return retExp;
    }

    case VERILOG_FUNCMATH_LN : { //"ln" ;
      // ln(x)' = x'/x
      // TODO: 1/x should also be computed only once!
      // the returned expression is created
      retExp = new CS_BinaryOperator( VERILOG_BINAR_DIV, arg1Deriv, arg1,
          getModule(), getStartLine(), getStartColumn());
      arg1->setTwinSelf("ln_arg"); // for efficient derivative computation
      return retExp;
    }

    case VERILOG_FUNCMATH_LOG : { //"log" ;
      // log_10(x)' = x' / (ln(10)*x)
      // TODO: 1/(ln(10)*x) should also be computed only once!
      double ln10 = log(10.0);
      CS_ExpressionBasis *factor = new CS_Number(ln10, getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis *denominator = new CS_BinaryOperator( VERILOG_BINAR_MUL, factor, arg1,
          getModule(), getStartLine(), getStartColumn());
      // the returned expression is created
      retExp = new CS_BinaryOperator( VERILOG_BINAR_DIV, arg1Deriv, denominator,
          getModule(), getStartLine(), getStartColumn());
      arg1->setTwinSelf("log_arg"); // for efficient derivative computation
      return retExp;
    }

    case VERILOG_FUNCMATH_MAX : { //"max" ;
      // max(x1,x2)' = (x1>x2)?x1':x2';
      CS_ExpressionBasis *condition = new CS_BinaryOperator( VERILOG_BINAR_GT, arg1, arg2,
          getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis *zero = new CS_Number(0.0, getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis *argm1 = (arg1Deriv != 0) ? arg1Deriv : zero;
      CS_ExpressionBasis *argm2 = (arg2Deriv != 0) ? arg2Deriv : zero;
      // the returned expression is created
      retExp = new CS_AnalogConditionalExpression( condition, argm1, argm2, getModule(),
          getStartLine(), getStartColumn());
      arg1->setTwinSelf("max_1_arg"); // for efficient derivative computation
      arg2->setTwinSelf("max_1_arg"); // for efficient derivative computation
      return retExp;
    }

    case VERILOG_FUNCMATH_MINIMUM : { //"min" ;
      // min(x1,x2)' = (x1<x2)?x1':x2';
      CS_ExpressionBasis *condition = new CS_BinaryOperator( VERILOG_BINAR_LT, arg1, arg2,
          getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis *zero = new CS_Number(0.0, getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis *argm1 = (arg1Deriv != 0) ? arg1Deriv : zero;
      CS_ExpressionBasis *argm2 = (arg2Deriv != 0) ? arg2Deriv : zero;
      // the returned expression is created
      retExp = new CS_AnalogConditionalExpression( condition, argm1, argm2, getModule(),
          getStartLine(), getStartColumn());
      arg1->setTwinSelf("min_1_arg"); // for efficient derivative computation
      arg2->setTwinSelf("min_2_arg"); // for efficient derivative computation
      return retExp;
    }

    case VERILOG_FUNCMATH_POW  : { //"pow" ;
      // (x^y)' = pow(x,y)' = y*x^{y-1}*x' + (x^y)*ln(x)*y'
      // TODO: ln(x) could be computed only once!
      CS_ExpressionBasis *a1 = 0;
      CS_ExpressionBasis *a2 = 0;
      if ( arg2Deriv != 0)
        { // x is dependent
          // (x^y)' = pow(x,y)' = (x^y)*ln(eps_limit(x))*y'
          CS_ExpressionBasis* args[1] = { arg1 };
          CS_ExpressionBasis *factarg = new CS_AnalogBuiltInMathFunction( VERILOG_FUNCMATH_LIMIT_TO_EPSILON,
              args , 1, getModule(), getStartLine(), getStartColumn());
          CS_ExpressionBasis* args1[1] = { factarg };
          CS_ExpressionBasis *fact1 = new CS_AnalogBuiltInMathFunction( VERILOG_FUNCMATH_LN, args1 , 1,
                        getModule(), getStartLine(), getStartColumn());
          CS_ExpressionBasis *fact2 = new CS_BinaryOperator( VERILOG_BINAR_MUL, this, fact1,
              getModule(), getStartLine(), getStartColumn());
          a2 = new CS_BinaryOperator( VERILOG_BINAR_MUL, fact2 , arg2Deriv,
              getModule(), getStartLine(), getStartColumn());
        }
      if ( arg1Deriv != 0)
        { // only y is dependent
          // (x^y)' = pow(x,y)' = y*x^{y-1}*x'
          CS_ExpressionBasis *fact1 = NULL;
          if (!IS_OPTIMIZED_EXPRESSION)
            {
              CS_ExpressionBasis *one = new CS_Number(1.0, getModule(), getStartLine(), getStartColumn());
              CS_ExpressionBasis *arg2m1 = new CS_BinaryOperator( VERILOG_BINAR_MIN, arg2 , one,
                  getModule(), getStartLine(), getStartColumn());
              CS_ExpressionBasis* args[2] = { arg1, arg2m1 };
              fact1 = new CS_AnalogBuiltInMathFunction( VERILOG_FUNCMATH_POW, args , 2,
                        getModule(), getStartLine(), getStartColumn());
            }
          else
            { // optimized version
              // further optimize by x^{y-1} = (x^y)/x , but when x==0 then this MUST be 0
              fact1 = new CS_BinaryOperator( VERILOG_BINAR_DIV_SAFE, this , arg1,
                  getModule(), getStartLine(), getStartColumn());
            }

          CS_ExpressionBasis *fact2 = new CS_BinaryOperator( VERILOG_BINAR_MUL, arg2, fact1,
              getModule(), getStartLine(), getStartColumn());
          a1 = new CS_BinaryOperator( VERILOG_BINAR_MUL, fact2 , arg1Deriv,
              getModule(), getStartLine(), getStartColumn());
        }

      arg1->setTwinSelf("pow_1_arg"); // for efficient derivative computation
      arg2->setTwinSelf("pow_2_arg"); // for efficient derivative computation
      // decide
      if ( arg2Deriv != 0)
        {
          if (arg1Deriv != 0)
            { // both are dependent
              // (x^y)' = pow(x,y)' = y*x^{y-1}*y' + (x^y)*ln(x)*x'
              retExp = new CS_BinaryOperator( VERILOG_BINAR_PLUS, a1, a2,
                  getModule(), getStartLine(), getStartColumn());
              this->setTwinSelf("pow_func");// for efficient derivative computation
            }
          else
            { // only y is dependent
              // (x^y)' = pow(x,y)' = y*x^{y-1}*x'
              retExp = a2;
              if (IS_OPTIMIZED_EXPRESSION)
                // further optimize by x^{y-1} = (x^y)/x
                this->setTwinSelf("pow_func");// for efficient derivative computation
            }
        }
      else
        { // is only x dependent
          // (x^y)' = pow(x,y)' = (x^y)*ln(x)*y'
          retExp = a1;
          this->setTwinSelf("pow_func"); // for efficient derivative computation
        }
      return retExp;
    }

    case VERILOG_FUNCMATH_LIMIT_TO_EPSILON: { // the limit to epsilon function
      CS_ExpressionBasis* args[1] = { arg1Deriv };
      retExp = new CS_AnalogBuiltInMathFunction( VERILOG_FUNCMATH_LIMIT_TO_EPSILON, args , 1,
               getModule(), getStartLine(), getStartColumn());
      return retExp;
    }

    case VERILOG_FUNCMATH_SQRT : { //"sqrt" ;
      // non-optimized:sqrt(x)' = (0.5/sqrt(mylim(x))) * x'
      // optimized:sqrt(x)' = (0.5/sqrt(x)) * x' // because the division should avoid division by zero
      // TODO: 0.5/sqrt(x) should also be computed only once!, division could be saved!
      CS_ExpressionBasis* args[1] = { this->getArg(0) };
      CS_ExpressionBasis *nominator = new CS_Number(0.5, getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis *factor = NULL;
      if (!IS_OPTIMIZED_EXPRESSION)
        {
          CS_ExpressionBasis *denominator1 = new CS_AnalogBuiltInMathFunction( VERILOG_FUNCMATH_LIMIT_TO_EPSILON,
              args , 1, getModule(), getStartLine(), getStartColumn());
          CS_ExpressionBasis* args1[1] = { denominator1 };
          CS_ExpressionBasis *denominator = new CS_AnalogBuiltInMathFunction( VERILOG_FUNCMATH_SQRT, args1 , 1,
              getModule(), getStartLine(), getStartColumn());
          factor = new CS_BinaryOperator( VERILOG_BINAR_DIV, nominator, denominator ,
              getModule(), getStartLine(), getStartColumn());
        }
      else
        factor = new CS_BinaryOperator( VERILOG_BINAR_DIV, nominator, this ,
            getModule(), getStartLine(), getStartColumn());
      // the returned expression is created
      retExp = new CS_BinaryOperator( VERILOG_BINAR_MUL, factor, arg1Deriv,
          getModule(), getStartLine(), getStartColumn());
      this->setTwinSelf("sqrt_func"); // for efficient derivative computation
      //denominator->setTwin(this);
      //denominator->setTwinPrefix(this->getTwinPrefix());
      return retExp;
    }

    case VERILOG_FUNCMATH_COS :  { //"cos" ;
      // (cos(x))' = -sin(x)*x'
      // TODO: -sin(x) could be computed only once!
      CS_ExpressionBasis* args[1] = { arg1 };
      CS_ExpressionBasis *sinus = new CS_AnalogBuiltInMathFunction( VERILOG_FUNCMATH_SIN, args , 1,
          getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis *msin =
          new CS_UnaryOperator( VERILOG_UNAR_MIN, sinus , getModule(), getStartLine(), getStartColumn());
      retExp = new CS_BinaryOperator( VERILOG_BINAR_MUL, msin, arg1Deriv,
          getModule(), getStartLine(), getStartColumn());
      return retExp;
    }
    case VERILOG_FUNCMATH_SIN :  {  //"sin" ;
      // (sin(x))' = cos(x)*x'
      // TODO: cos(x) could be computed only once!
      CS_ExpressionBasis* args[1] = { arg1 };
      CS_ExpressionBasis *cosinus = new CS_AnalogBuiltInMathFunction( VERILOG_FUNCMATH_COS, args , 1,
          getModule(), getStartLine(), getStartColumn());
      retExp = new CS_BinaryOperator( VERILOG_BINAR_MUL, cosinus, arg1Deriv,
          getModule(), getStartLine(), getStartColumn());
      arg1->setTwinSelf("sin_1_arg");
      return retExp;
    }
    case VERILOG_FUNCMATH_TAN :  { //"tan" ;
      // (tan(x))' = (x'/cos(x)^2)
      // TODO: 1/cos(x)^2 could be computed only once!
      CS_ExpressionBasis* args[1] = { arg1 };
      CS_ExpressionBasis *cos = new CS_AnalogBuiltInMathFunction( VERILOG_FUNCMATH_COS, args , 1,
          getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis *cos2 = new CS_BinaryOperator( VERILOG_BINAR_MUL, cos, cos,
          getModule(), getStartLine(), getStartColumn());
      retExp = new CS_BinaryOperator( VERILOG_BINAR_DIV, arg1Deriv, cos2 ,
          getModule(), getStartLine(), getStartColumn());
      cos->setTwinSelf("cos_func"); // for efficient derivative computation
      arg1->setTwinSelf("tan_1_arg");
      return retExp;
    }

    case VERILOG_FUNCMATH_ACOS :  { //"acos" ;
      /* Differentiation rule for acos : acos(x) --> x' * 1 / -sqrt(1-x**2) */
      // TODO: optimize
      CS_ExpressionBasis *x2 = new CS_BinaryOperator( VERILOG_BINAR_MUL, arg1, arg1,
          getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis *one = new CS_Number(1.0, getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis *one_m_x2 = new CS_BinaryOperator( VERILOG_BINAR_MIN, one, x2,
          getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis* args[1] = { one_m_x2 };
      CS_ExpressionBasis *sqrt_one_m_x2 = new CS_AnalogBuiltInMathFunction( VERILOG_FUNCMATH_SQRT, args , 1,
          getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis *mXderiv =
          new CS_UnaryOperator( VERILOG_UNAR_MIN, arg1Deriv , getModule(), getStartLine(), getStartColumn());
      retExp = new CS_BinaryOperator( VERILOG_BINAR_DIV, mXderiv, sqrt_one_m_x2 ,
          getModule(), getStartLine(), getStartColumn());
      return retExp;
    }
    case VERILOG_FUNCMATH_ASIN  : { //"asin" ;
      /* Differentiation rule for asin : asin(x) --> x' * 1 / sqrt(1-x**2) */
      // TODO: optimize
      CS_ExpressionBasis *x2 = new CS_BinaryOperator( VERILOG_BINAR_MUL, arg1, arg1,
          getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis *one = new CS_Number(1.0, getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis *one_m_x2 = new CS_BinaryOperator( VERILOG_BINAR_MIN, one, x2,
          getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis* args[1] = { one_m_x2 };
      CS_ExpressionBasis *sqrt_one_m_x2 = new CS_AnalogBuiltInMathFunction( VERILOG_FUNCMATH_SQRT, args , 1,
          getModule(), getStartLine(), getStartColumn());
      retExp = new CS_BinaryOperator( VERILOG_BINAR_DIV, arg1Deriv, sqrt_one_m_x2 ,
          getModule(), getStartLine(), getStartColumn());
      return retExp;
    }
    case VERILOG_FUNCMATH_ATAN  : { //"atan" ;
      /* Differentiation rule for atan : atan(x) --> x' * 1 / (1+x**2) */
      // TODO: optimize
      CS_ExpressionBasis *x2 = new CS_BinaryOperator( VERILOG_BINAR_MUL, arg1, arg1,
          getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis *one = new CS_Number(1.0, getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis *one_p_x2 = new CS_BinaryOperator( VERILOG_BINAR_PLUS, one, x2,
          getModule(), getStartLine(), getStartColumn());
      retExp = new CS_BinaryOperator( VERILOG_BINAR_DIV, arg1Deriv, one_p_x2 ,
          getModule(), getStartLine(), getStartColumn());
      return retExp;
    }

    case VERILOG_FUNCMATH_COSH : { //"cosh" ;
      // TODO: optimize
      /* Differentiation rule for cosh : cosh(x) --> x' * sinh(x) */
      CS_ExpressionBasis* args[1] = { arg1 };
      CS_ExpressionBasis *cosinus = new CS_AnalogBuiltInMathFunction( VERILOG_FUNCMATH_SINH, args , 1,
          getModule(), getStartLine(), getStartColumn());
      retExp = new CS_BinaryOperator( VERILOG_BINAR_MUL, cosinus, arg1Deriv,
          getModule(), getStartLine(), getStartColumn());
      return retExp;
    }
    case VERILOG_FUNCMATH_SINH : { //"sinh" ;
      /* Differentiation rule for sinh : sinh(x) --> x' * cosh(x) */
      // TODO: optimize
      CS_ExpressionBasis* args[1] = { arg1 };
      CS_ExpressionBasis *cosinus = new CS_AnalogBuiltInMathFunction( VERILOG_FUNCMATH_COSH, args , 1,
          getModule(), getStartLine(), getStartColumn());
      retExp = new CS_BinaryOperator( VERILOG_BINAR_MUL, cosinus, arg1Deriv,
          getModule(), getStartLine(), getStartColumn());
      return retExp;
    }
    case VERILOG_FUNCMATH_TANH : { //"tanh" ;
      // TODO: optimize
      /* Differentiation rule for tanh : tanh(x) --> x' * (1 - tanh(x) ** 2)
           ("1-tanh()" might result in cancellation but the equivalent
           derivative tanh(x) --> x' * 1/(cosh(x) ** 2) overflows for large
           arguments. */
        /* tanh(x) --> x' * (1 - tanh(x) ** 2) */
        /* this is one alternative-> ... tanh(x) --> x' * 1/(cosh(x) ** 2) */
      CS_ExpressionBasis *tanh2 = new CS_BinaryOperator( VERILOG_BINAR_MUL, this, this,
                getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis *one = new CS_Number(1.0, getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis *one_m_tanh2= new CS_BinaryOperator( VERILOG_BINAR_MIN, one, tanh2,
          getModule(), getStartLine(), getStartColumn());
      retExp = new CS_BinaryOperator( VERILOG_BINAR_MUL, one_m_tanh2, arg1Deriv,
          getModule(), getStartLine(), getStartColumn());
      this->setTwinSelf("tanh_func");
      return retExp;
    }

    case VERILOG_FUNCMATH_ACOSH : { //"acosh" ;
      // arccosh x    ->  x' / sqrt [x 2 - 1 ]
      CS_ExpressionBasis *x2 = new CS_BinaryOperator( VERILOG_BINAR_MUL, arg1, arg1,
          getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis *one = new CS_Number(1.0, getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis *x2_m_one = new CS_BinaryOperator( VERILOG_BINAR_MIN, x2, one,
          getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis* args[1] = { x2_m_one };
      CS_ExpressionBasis *sqrt_one_m_x2 = new CS_AnalogBuiltInMathFunction( VERILOG_FUNCMATH_SQRT, args ,
          1, getModule(), getStartLine(), getStartColumn());
      retExp = new CS_BinaryOperator( VERILOG_BINAR_DIV, arg1Deriv, sqrt_one_m_x2 ,
          getModule(), getStartLine(), getStartColumn());
      return retExp;
    }
    case VERILOG_FUNCMATH_ASINH : { //"asinh" ;
      // arcsinh x   ->   x' / sqrt [x^2 + 1 ]
      CS_ExpressionBasis *x2 = new CS_BinaryOperator( VERILOG_BINAR_MUL, arg1, arg1,
          getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis *one = new CS_Number(1.0, getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis *x2_p_one = new CS_BinaryOperator( VERILOG_BINAR_PLUS, x2, one,
          getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis* args[1] = { x2_p_one };
      CS_ExpressionBasis *sqrt_one_m_x2 = new CS_AnalogBuiltInMathFunction( VERILOG_FUNCMATH_SQRT, args ,
          1, getModule(), getStartLine(), getStartColumn());
      retExp = new CS_BinaryOperator( VERILOG_BINAR_DIV, arg1Deriv, sqrt_one_m_x2 ,
          getModule(), getStartLine(), getStartColumn());
      return retExp;
    }
    case VERILOG_FUNCMATH_ATANH : { //"atanh" ;
      // arctanh x     ->     x' / [ 1 - x^2 ]
      CS_ExpressionBasis *x2 = new CS_BinaryOperator( VERILOG_BINAR_MUL, arg1, arg1,
          getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis *one = new CS_Number(1.0, getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis *one_m_x2 = new CS_BinaryOperator( VERILOG_BINAR_MIN, one, x2,
          getModule(), getStartLine(), getStartColumn());
      retExp = new CS_BinaryOperator( VERILOG_BINAR_DIV, arg1Deriv, one_m_x2 ,
          getModule(), getStartLine(), getStartColumn());
      return retExp;
    }

    case VERILOG_FUNCMATH_HYPOT : { //"hypot" ;
      // hypot(x,y) -> 0.5*x'/(x^2+y^2) + 0.5*y'/(x^2+y^2)
      CS_ExpressionBasis *y2 = new CS_BinaryOperator( VERILOG_BINAR_MUL, arg1 , arg1,
          getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis *x2 = new CS_BinaryOperator( VERILOG_BINAR_MUL, arg2 , arg2,
          getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis *x2_p_y2 = new CS_BinaryOperator( VERILOG_BINAR_PLUS, y2 , x2,
          getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis* args[1] = { x2_p_y2 };
      CS_ExpressionBasis *sqrt_x2_p_p2 = new CS_AnalogBuiltInMathFunction( VERILOG_FUNCMATH_SQRT, args ,
          1, getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis *half = new CS_Number(0.5, getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis *half_over_sqrt_x2_p_p2 = new CS_BinaryOperator( VERILOG_BINAR_DIV, half ,
          sqrt_x2_p_p2, getModule(), getStartLine(), getStartColumn());
      // two terms
      CS_ExpressionBasis *t1 = 0;
      CS_ExpressionBasis *t2 = 0;
      if (arg1Deriv != 0 ){
          // y dependent -> 0.5*x'/(x^2+y^2)
          t1 = new CS_BinaryOperator( VERILOG_BINAR_MUL, arg1Deriv , half_over_sqrt_x2_p_p2,
                        getModule(), getStartLine(), getStartColumn());
      }
      if (arg2Deriv != 0 ){
          // x dependent -> - 0.5*y'/(x^2+y^2)
          t2 = new CS_BinaryOperator( VERILOG_BINAR_MUL, arg2Deriv , half_over_sqrt_x2_p_p2,
                        getModule(), getStartLine(), getStartColumn());
      }
      // set the derivative depending on the dependencies
      if (arg1Deriv != 0 ){
          if (arg2Deriv != 0 ){
              retExp = new CS_BinaryOperator( VERILOG_BINAR_PLUS, t1 , t2,
                  getModule(), getStartLine(), getStartColumn());
          }
          else {
              retExp = t1;
          }
      }
      else {
          retExp = t2;
      }
      return retExp;
    }
    case VERILOG_FUNCMATH_ATAN2 : { //"atan2" ;
      // atan2(y,x)' = y' * x/(x^2+y^2) - x' * y/(x^2+y^2)
      //x^2+y^2
      CS_ExpressionBasis *y2 = new CS_BinaryOperator( VERILOG_BINAR_MUL, arg1 , arg1,
          getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis *x2 = new CS_BinaryOperator( VERILOG_BINAR_MUL, arg2 , arg2,
          getModule(), getStartLine(), getStartColumn());
      CS_ExpressionBasis *y2_p_x2 = new CS_BinaryOperator( VERILOG_BINAR_PLUS, y2 , x2,
          getModule(), getStartLine(), getStartColumn());

      CS_ExpressionBasis *t1 = 0;
      CS_ExpressionBasis *t2 = 0;
      if (arg1Deriv != 0 ){
          // y dependent -> y' * x/(x^2+y^2)
          CS_ExpressionBasis *tmp1 = new CS_BinaryOperator( VERILOG_BINAR_MUL, arg1Deriv , arg2,
              getModule(), getStartLine(), getStartColumn());
          t1 = new CS_BinaryOperator( VERILOG_BINAR_DIV, tmp1 , y2_p_x2,
                        getModule(), getStartLine(), getStartColumn());
      }
      if (arg2Deriv != 0 ){
          // x dependent -> - x' * y/(x^2+y^2)
          CS_ExpressionBasis *tmp1 = new CS_BinaryOperator( VERILOG_BINAR_MUL, arg2Deriv , arg2,
              getModule(), getStartLine(), getStartColumn());
          CS_ExpressionBasis *tmp2 = new CS_UnaryOperator( VERILOG_UNAR_MIN, tmp1 ,
              getModule(), getStartLine(), getStartColumn());
          t2 = new CS_BinaryOperator( VERILOG_BINAR_DIV, tmp2 , y2_p_x2,
                        getModule(), getStartLine(), getStartColumn());
      }
      // set the derivative depending on the dependencies
      if (arg1Deriv != 0 ){
          if (arg2Deriv != 0 ){
              retExp = new CS_BinaryOperator( VERILOG_BINAR_PLUS, t1 , t2,
                  getModule(), getStartLine(), getStartColumn());
          }
          else {
              retExp = t1;
          }
      }
      else {
          retExp = t2;
      }
      return retExp;
    }

    // these cases should not occur
    case VERILOG_FUNCMATH_FLOOR : { return 0; } //"floor" ;
    case VERILOG_FUNCMATH_CEIL:   { return 0; } //"ceil"
    case VERILOG_FUNCMATH_CLOG2 : { return 0; } // clog2
    default: { /** this should not occur */ return 0;}
  }
}

