/*
 * CS_BinaryOperator.cpp
 *
 *  Created on: Dec 12, 2012
 *      Author: benk
 */

#include "CS_BinaryOperator.hpp"
#include "CS_UnaryOperator.hpp"
#include "CS_Number.hpp"
#include "CS_String.hpp"

#include <math.h>
#include <string.h>

CS_BinaryOperator::CS_BinaryOperator(BinarOpType ty, CS_ExpressionBasis* exprL, CS_ExpressionBasis* exprR,
    const CS_ModuleDeclaration* module, int startLine, int startColumn):
    CS_ExpressionBasis(module, startLine, startColumn), opType_(ty),
    hasStringOperands_(false), targetExprL_(exprL), targetExprR_(exprR)
{
  // make basic test
  if (verb() > 2)
   {    VERILOG_ERROR_COND_STOP( (exprL == 0) || (exprR == 0), " CS_BinaryOperator, " << ty <<
      " one expression is NULL " << exprL << " or " << exprR, this); }
  else
   { VERILOG_ERROR_COND_STOP( (exprL == 0) || (exprR == 0), "Internal Error", this); }

  // detect if we have string operators
  if ( (targetExprL_->getTypes() >= STRING) && (targetExprR_->getTypes() >= STRING) )
    hasStringOperands_ = true;

  if (hasStringOperands_)
    {
      // test for string binary operators
      switch (opType_) {
        // logical operations for strings
        case VERILOG_BINAR_GT :   // ">" ;
        case VERILOG_BINAR_LT :   // "<" ;
        case VERILOG_BINAR_GEQ :  // ">=" ;
        case VERILOG_BINAR_LEQ :  // "<=" ;
        case VERILOG_BINAR_LOGEQ : //"==" ;
        case VERILOG_BINAR_LOGNEQ ://"!=" ;
          break;
        // this case should not happen
        default: {
          VERILOG_PRINT_L3( verb(), " UNKNOWN BINARY OPERATION FOR STRINGS ");
          VERILOG_ERROR_STOP("Internal Error", this);
          break;
        }
        }
    }
}

double CS_BinaryOperator::evalConst() const {

  if (hasStringOperands_)
    { // string binary operations
      // both operands must be constant
      VERILOG_ERROR_COND_STOP( targetExprL_->getClasses() != CONSTANT , "Left operand must be constant", targetExprL_);
      VERILOG_ERROR_COND_STOP( targetExprR_->getClasses() != CONSTANT , "Right operand must be constant", targetExprR_);

      const CS_String *leftS = dynamic_cast<CS_String*>(targetExprL_);
      const CS_String *rightS = dynamic_cast<CS_String*>(targetExprR_);

      VERILOG_ERROR_COND_STOP( leftS == NULL, "Left operand must be constant string.", targetExprL_);
      VERILOG_ERROR_COND_STOP( rightS == NULL , "Right operand must be constant string.", targetExprR_);

      std::string leftStr = leftS->getStringValue();
      std::string rightStr = rightS->getStringValue();

      switch (opType_) {
        // string operations
        case VERILOG_BINAR_GT :       { return (double)(strcmp(leftStr.c_str(),
            rightStr.c_str() ) > 0); } // ">" ;
        case VERILOG_BINAR_LT :       { return (double)(strcmp(leftStr.c_str(),
            rightStr.c_str() ) < 0); } // "<" ;
        case VERILOG_BINAR_GEQ :      { return (double)(double)(strcmp(leftStr.c_str(),
            rightStr.c_str() ) >= 0); } // ">=" ;
        case VERILOG_BINAR_LEQ :      { return (double)(strcmp(leftStr.c_str(),
            rightStr.c_str() ) <= 0); } // "<=" ;
        case VERILOG_BINAR_LOGEQ :    { return (double)(strcmp(leftStr.c_str(),
            rightStr.c_str() ) == 0); } //"==" ;
        case VERILOG_BINAR_LOGNEQ :   { return (double)(strcmp(leftStr.c_str(),
            rightStr.c_str() ) != 0); } //"!=" ;
        // this case should not happen
        default: {
          VERILOG_PRINT_L3( verb(), " UNKNOWN BINARY OPERATION FOR STRINGS ");
          VERILOG_ERROR_STOP("Internal Error", this);
          break;
        }
      }
    }
  else
    {
      double valL = targetExprL_->evalConst();
      double valR = targetExprR_->evalConst();
      switch (opType_) {
        // arithmetic operators
        case VERILOG_BINAR_PLUS :     { return (valL + valR); } // "+" ;
        case VERILOG_BINAR_MIN :      { return (valL - valR); } // "-" ;
        case VERILOG_BINAR_MUL :      { return (valL * valR); } // "*" ;
        case VERILOG_BINAR_DIV_SAFE :
        case VERILOG_BINAR_DIV :      { return ( ((fabs(valR) > VERILOG_MY_EPSILON)) ? (valL / valR) :
            (valL/((valR>0)?(VERILOG_MY_EPSILON):(-VERILOG_MY_EPSILON))) ); } // "/" ; (safe division to avoid NaNs)

        case VERILOG_BINAR_MODULUS :  { return (double)( ((int)(valL)) % ((int)(valR))); } // "%" ;
        // logical operations
        case VERILOG_BINAR_GT :       { return (double)(valL > valR); } // ">" ;
        case VERILOG_BINAR_LT :       { return (double)(valL < valR); } // "<" ;
        case VERILOG_BINAR_GEQ :      { return (double)(valL >= valR); } // ">=" ;
        case VERILOG_BINAR_LEQ :      { return (double)(valL <= valR); } // "<=" ;
        case VERILOG_BINAR_LOGEQ :    { return (double)(valL == valR); } //"==" ;
        case VERILOG_BINAR_LOGNEQ :   { return (double)(valL != valR); } //"!=" ;
        case VERILOG_BINAR_LOGAND :   { return (double)(valL && valR); } //"&&" ;
        case VERILOG_BINAR_LOGOR :    { return (double)(valL || valR); } //"||" ;
        // operations only for intergers
        case VERILOG_BINAR_REDAND :   { return (double)((int)(valL) & (int)(valR)); } //"&" ;
        case VERILOG_BINAR_REDOR :    { return (double)((int)(valL) | (int)(valR)); } //"|" ;
        case VERILOG_BINAR_REDXOR :   { return (double)( ((int)(valL)) ^ ((int)(valR))); } //"^" ;
        case VERILOG_BINAR_REDNAND :  { return (double)( ~((int)(valL)) & ((int)(valR))); } //"~&" ;
        case VERILOG_BINAR_REDNOR :   { return (double)( ~((int)(valL)) | ((int)(valR))); } //"~|" ;
        case VERILOG_BINAR_REDXNOR :  { return (double)( ~((int)(valL)) ^ ((int)(valR))); } // "~^" ;
        case VERILOG_BINAR_LSHIFT :   { return (double)( ((int)(valL)) << ((int)(valR))); } // "<<" ;
        case VERILOG_BINAR_RSHIFT :   { return (double)( ((int)(valL)) >> ((int)(valR))); } // ">>" ;
        // this case should not happen
        default: {
          VERILOG_PRINT_L3( verb(), " UNKNOWN BINARY OPERATION " << opType_);
          VERILOG_ERROR_STOP("Internal Error", this);
          break;}
      }
    }
  return 0.0;
}

int CS_BinaryOperator::getClasses() const
{
  // return the combined classes
  return (targetExprL_->getClasses() | targetExprR_->getClasses());
}

int CS_BinaryOperator::getTypes() const
{
  // if this is a string operation then the returned type is always an integer
  if (hasStringOperands_)
    return (int)INTEGER;

  switch (opType_) {
    // arithmetic operators
    case VERILOG_BINAR_PLUS :  // "+" ;
    case VERILOG_BINAR_MIN :   // "-" ;
    case VERILOG_BINAR_MUL :   // "*" ;
    case VERILOG_BINAR_DIV_SAFE :   // "/" ;
    case VERILOG_BINAR_DIV :   // "/" ;
      // in these cases, we classes return the combined type
      return (targetExprL_->getTypes() | targetExprR_->getTypes());

    case VERILOG_BINAR_MODULUS :  // "%" ;
    case VERILOG_BINAR_GT :       // ">" ;
    case VERILOG_BINAR_LT :       // "<" ;
    case VERILOG_BINAR_GEQ :      // ">=" ;
    case VERILOG_BINAR_LEQ :      // "<=" ;
    case VERILOG_BINAR_LOGEQ :    //"==" ;
    case VERILOG_BINAR_LOGNEQ :   //"!=" ;
    case VERILOG_BINAR_LOGAND :   //"&&" ;
    case VERILOG_BINAR_LOGOR :    //"||" ;
    case VERILOG_BINAR_REDAND :   //"&" ;
    case VERILOG_BINAR_REDOR :    //"|" ;
    case VERILOG_BINAR_REDXOR :   //"^" ;
    case VERILOG_BINAR_REDNAND :  //"~&" ;
    case VERILOG_BINAR_REDNOR :   //"~|" ;
    case VERILOG_BINAR_REDXNOR :  // "~^" ;
    case VERILOG_BINAR_LSHIFT :   // "<<" ;
    case VERILOG_BINAR_RSHIFT :   // ">>" ;
      // these are always real
      return (int)INTEGER;
    // this case should not happen
    default: {
      VERILOG_PRINT_L3( verb(), " UNKNOWN BINARY OPERATION " << opType_);
      VERILOG_ERROR_STOP("Internal Error", this);
      break;}
  }
  // default, this is usually not called
  return (targetExprL_->getTypes() | targetExprR_->getTypes());
}

/** propagate recursively the system dependency informations */
CS_AKDependency CS_BinaryOperator::propagateDependencies()
{
  CS_AKDependency b1 = targetExprL_->propagateDependencies();
  CS_AKDependency b2 = targetExprR_->propagateDependencies();
  CS_AKDependency tmp_ret;

  // test if this is a string operation
  if (hasStringOperands_)
    {
      // nothing to do, no dependency
    }
  else
    {
      switch (opType_) {
        // arithmetic operators
        case VERILOG_BINAR_PLUS :  // "+" ;
        case VERILOG_BINAR_MIN :   // "-" ;
        case VERILOG_BINAR_MUL :   // "*" ;
        case VERILOG_BINAR_DIV_SAFE :
        case VERILOG_BINAR_DIV :   // "/" ;
          {
            // in these cases, we classes propagate the dependencies
            tmp_ret.uniteDependencies(b1);
            tmp_ret.uniteDependencies(b2);
            break;
          }
        case VERILOG_BINAR_MODULUS :  // "%" ;
        case VERILOG_BINAR_GT :       // ">" ;
        case VERILOG_BINAR_LT :       // "<" ;
        case VERILOG_BINAR_GEQ :      // ">=" ;
        case VERILOG_BINAR_LEQ :      // "<=" ;
        case VERILOG_BINAR_LOGEQ :    //"==" ;
        case VERILOG_BINAR_LOGNEQ :   //"!=" ;
        case VERILOG_BINAR_LOGAND :   //"&&" ;
        case VERILOG_BINAR_LOGOR :    //"||" ;
        case VERILOG_BINAR_REDAND :   //"&" ;
        case VERILOG_BINAR_REDOR :    //"|" ;
        case VERILOG_BINAR_REDXOR :   //"^" ;
        case VERILOG_BINAR_REDNAND :  //"~&" ;
        case VERILOG_BINAR_REDNOR :   //"~|" ;
        case VERILOG_BINAR_REDXNOR :  // "~^" ;
        case VERILOG_BINAR_LSHIFT :   // "<<" ;
        case VERILOG_BINAR_RSHIFT :   // ">>" ;
          {
            // no propagation for these operators, just return empty set
            break;
          }
        default: {
          VERILOG_PRINT_L3( verb(), " UNKNOWN BINARY OPERATION ");
          VERILOG_ERROR_STOP("Internal Error", this);
          break;}
      }
    }
  // store it in the actual node
  setActVarDependency(tmp_ret);
  // return the combined dependency
  return tmp_ret;
}

CS_ExpressionBasis* CS_BinaryOperator::doDifferentiate(int sysVarIndex, int *flag)
{
  // test if this dependent on the chosen system variable,
  // and if this is a string expression
  if (   (this->getActVarDependency().isSysVarIndexPresent(sysVarIndex) < 0)
      &&  hasStringOperands_ )
    return 0;
  // when this is not the required operator return null
  if (!(  (opType_ == VERILOG_BINAR_PLUS) || (opType_ == VERILOG_BINAR_MIN) ||
         (opType_ == VERILOG_BINAR_MUL) || (opType_ == VERILOG_BINAR_DIV) ||
         (opType_ == VERILOG_BINAR_DIV_SAFE)) )
    return 0;

  // if we reach the switch statement than we have dependency w.r.t. to this system variable
  CS_ExpressionBasis* retExp = 0;
  CS_ExpressionBasis *arg1 = targetExprL_;
  CS_ExpressionBasis *arg2 = targetExprR_;
  CS_ExpressionBasis *arg1Deriv = 0;
  CS_ExpressionBasis *arg2Deriv = 0;
  // compute the derivatives only if it is necessary
  if ( (opType_ == VERILOG_BINAR_PLUS) || (opType_ == VERILOG_BINAR_MIN) ||
       (opType_ == VERILOG_BINAR_MUL) || (opType_ == VERILOG_BINAR_DIV)  ||
       (opType_ == VERILOG_BINAR_DIV_SAFE) )
    {
      // compute the derivatives of the arguments
      if (arg1->getActVarDependency().isSysVarIndexPresent(sysVarIndex) >= 0) {
          //VERILOG_PRINT_L3( verb(), "B D1 \n");
          arg1Deriv = arg1->doDifferentiate(sysVarIndex, flag);
      }
      if (arg2->getActVarDependency().isSysVarIndexPresent(sysVarIndex) >= 0) {
          //VERILOG_PRINT_L3( verb(),"B D2 \n");
          arg2Deriv = arg2->doDifferentiate(sysVarIndex, flag);
      }
    }
  if ( arg1Deriv == 0 && arg2Deriv == 0) {
      //VarType resType = REAL;
      VERILOG_PRINT_L3( verb()," D(" << sysVarIndex << ") = " // << arg1->eval(resType)
          << " " << arg1->getActVarDependency().toString() );
      VERILOG_PRINT_L3( verb()," D(" << sysVarIndex << ") = " //<< arg2->eval(resType)
          << " " << arg2->getActVarDependency().toString() );
  }
  //VERILOG_PRINT_L3( verb()," DIFF BINAR " << opType_ << "\n");
  // based on the type of the operator do:
  switch (opType_) {
      // arithmetic operators
      case VERILOG_BINAR_PLUS :  // "+" ;
        {
          if (arg1Deriv != 0) {
              if (arg2Deriv != 0){ // both are dependent
                  // return a new binary expression with the derivatives
                  retExp = new CS_BinaryOperator( VERILOG_BINAR_PLUS, arg1Deriv, arg2Deriv,
                      getModule(), getStartLine(), getStartColumn());
              }
              else{ // only arg 1 is dependent
                  retExp = arg1Deriv;
              }
          }
          else{ // only arg 2 is dependent
              retExp = arg2Deriv;
          }
          break;
        }
      case VERILOG_BINAR_MIN :   // "-" ;
        {
          if (arg1Deriv != 0) {
              if (arg2Deriv != 0){ // both are dependent
                  // return a new binary expression with the derivatives
                  retExp = new CS_BinaryOperator( VERILOG_BINAR_MIN, arg1Deriv, arg2Deriv,
                      getModule(), getStartLine(), getStartColumn());
              }
              else{ // only arg 1 is dependent
                  retExp = arg1Deriv;
              }
          }
          else{ // only arg 2 is dependent, but with negative sign : -y'
              retExp = new CS_UnaryOperator( VERILOG_UNAR_MIN,
                  arg2Deriv, getModule(), getStartLine(), getStartColumn());
          }
          break;
        }
      case VERILOG_BINAR_MUL :   // "*" ;
        { // (x*y)' = x'*y + x*y'
          if (arg1Deriv != 0) {
              if (arg2Deriv != 0){ // both are dependent
                  CS_ExpressionBasis *t1 = new CS_BinaryOperator( VERILOG_BINAR_MUL, arg1Deriv, arg2,
                      getModule(), getStartLine(), getStartColumn());
                  CS_ExpressionBasis *t2 = new CS_BinaryOperator( VERILOG_BINAR_MUL, arg1, arg2Deriv ,
                      getModule(), getStartLine(), getStartColumn());
                  // return a new binary expression with the derivatives
                  retExp = new CS_BinaryOperator( VERILOG_BINAR_PLUS, t1, t2,
                      getModule(), getStartLine(), getStartColumn());
                  arg1->setTwinSelf("mul_1_op");
                  arg2->setTwinSelf("mul_2_op");
              }
              else{ // only arg 1 is dependent (x*y)' = x'*y
                  retExp = new CS_BinaryOperator( VERILOG_BINAR_MUL, arg1Deriv, arg2 ,
                      getModule(), getStartLine(), getStartColumn());
                  arg2->setTwinSelf("mul_2_op");
              }
          }
          else{ // only arg 2 is dependent // (x*y)' = x*y'
              retExp = new CS_BinaryOperator( VERILOG_BINAR_MUL, arg1, arg2Deriv ,
                  getModule(), getStartLine(), getStartColumn());
              arg1->setTwinSelf("mul_1_op");
          }
          break;
        }
      case VERILOG_BINAR_DIV_SAFE :
      case VERILOG_BINAR_DIV :   // "/" ;
        { // (x/y)' = ( x'*y - x*y') / y^2
          if (arg1Deriv != 0) {
              if (arg2Deriv != 0){ // both are dependent
                  // TODO: 1 / y^2 could be computed only once!
                  CS_ExpressionBasis *t1 = new CS_BinaryOperator( VERILOG_BINAR_MUL, arg1Deriv, arg2,
                      getModule(), getStartLine(), getStartColumn());
                  CS_ExpressionBasis *t2 = new CS_BinaryOperator( VERILOG_BINAR_MUL, arg1, arg2Deriv ,
                      getModule(), getStartLine(), getStartColumn());
                  // return a new binary expression with the derivatives
                  CS_ExpressionBasis *nominat = new CS_BinaryOperator( VERILOG_BINAR_MIN, t1, t2,
                      getModule(), getStartLine(), getStartColumn());
                  CS_ExpressionBasis *denominat = new CS_BinaryOperator( VERILOG_BINAR_MUL, arg2, arg2,
                                        getModule(), getStartLine(), getStartColumn());
                  // return the fraction
                  retExp = new CS_BinaryOperator( VERILOG_BINAR_DIV, nominat, denominat,
                      getModule(), getStartLine(), getStartColumn());
                  arg1->setTwinSelf("div_1_op");
                  arg2->setTwinSelf("div_2_op");
              }
              else{ // only arg 1 is dependent
                  // (x/y)' = x'/y
                  // TODO: 1/y could be computed only once ?
                  retExp = new CS_BinaryOperator( VERILOG_BINAR_DIV, arg1Deriv, arg2,
                      getModule(), getStartLine(), getStartColumn() );
                  arg2->setTwinSelf("div_2_op");
              }
          }
          else{ // only arg 2 is dependent, but with negative sign
              // (x/y)' = (-x/y^2)*y'
              // TODO: 1/y could be computed only once ?
              CS_ExpressionBasis *t1tmp = new CS_BinaryOperator( VERILOG_BINAR_MUL, arg2, arg2,
                  getModule(), getStartLine(), getStartColumn());
              CS_ExpressionBasis *t0 = new CS_UnaryOperator( VERILOG_UNAR_MIN, arg1,
                  getModule(), getStartLine(), getStartColumn());
              CS_ExpressionBasis *t1 = new CS_BinaryOperator( VERILOG_BINAR_DIV, t0, t1tmp,
                  getModule(), getStartLine(), getStartColumn());
              retExp  = new CS_BinaryOperator( VERILOG_BINAR_MUL, t1, arg2Deriv,
                  getModule(), getStartLine(), getStartColumn());
              arg1->setTwinSelf("div_1_op");
              arg2->setTwinSelf("div_2_op");
          }
          break;
        }
      default: {/* return null pointer*/ break;}
  }
  // return the ready expression
  return retExp;
}


/* function that creates a deep copy of the expression itself */
CS_ExpressionBasis* CS_BinaryOperator::copy() {
  // copy the arguments
  CS_ExpressionBasis *left = targetExprL_->copy();
  CS_ExpressionBasis *right = targetExprR_->copy();
  VERILOG_PRINT_L2( verb() , " Copy Binary" );
  // create new object
  return (new CS_BinaryOperator(opType_, left,
      right, this->getModule(),
      this->getStartLine(), this->getStartColumn()));
}
