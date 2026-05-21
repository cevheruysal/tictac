/*
 * CS_AnalogFilterOperationFunction.cpp
 *
 *  Created on: Dec 14, 2012
 *      Author: benk
 */

#include "CS_AnalogFilterOperationFunction.hpp"
#include "CS_Number.hpp"
#include "CS_VariableReference.hpp"
#include "CS_BranchProbeFunction.hpp"
#include "CS_Number.hpp"
#include "CS_BinaryOperator.hpp"

CS_AnalogFilterOperationFunction::CS_AnalogFilterOperationFunction( const AnalogOpFunctionType opType,
    const CS_ModuleDeclaration* module, int startLine, int startColumn,
    CS_ExpressionBasis** args, int nrArgs):
    CS_FunctionCall( args, nrArgs, module, startLine, startColumn, true),
    opType_(opType), ddx_Deriv_expr_(0)
{
  // todo: check arguments

  VERILOG_ERROR_COND_STOP( !this->getArg(0), "This function needs one (non-default) argument", this);

  switch (opType_) {
   case VERILOG_ANOPFUNC_LIMIT: {
      VERILOG_ERROR_COND_STOP( ((getNrArg() < 1) || (getArg(0) == NULL)) ,
          "LIMIT needs at least one valid argument", this );
      VERILOG_WARNING( "Limiting currently not active", this );
      break; }
   case VERILOG_ANOPFUNC_DDX: {
     VERILOG_ERROR_COND_STOP( (getNrArg() != 2), "DDX takes exactly (only) two anguments", this);
     CS_ExpressionBasis* exprRef = getArg(0);
     CS_BranchProbeFunction *sysVar = dynamic_cast<CS_BranchProbeFunction*>(getArg(1));

     // for DDX should be a voltage argument!!!
     VERILOG_ERROR_COND_STOP( sysVar == NULL, "DDX Second argument must be either "
         "a pin voltage or a current variable (and not an arbitrary expression).", this);
     // test the second argument which should be only either a voltage or a current variable
     VERILOG_ERROR_COND_STOP( (
            (sysVar->getBranchExpressionType() != VERILOG_BRANCH_V_ONE)
         && (sysVar->getBranchExpressionType() != VERILOG_BRANCH_I_TWO)),
         "DDX Second argument must be either "
         "V(pin) - pin voltage or I(pin1,pin2) - current variable.", this);
     // expression must be a scalar expression
     VERILOG_ERROR_COND_STOP( exprRef->getTypes() >= INTEGER_ARRAY ,
         "Expression in DDX must be a scalar " << exprRef->getTypes(), this);
     break;
   }
   case VERILOG_ANOPFUNC_TABLE: {
     VERILOG_ERROR_COND_STOP( ((getNrArg() < 3) || (getArg(0) == NULL)
         || (getArg(1) == NULL) || (getArg(2) == NULL)) ,
         "TABLE_MODEL needs at least three valid arguments", this );
     CS_ExpressionBasis* inE = getArg(0);
     CS_ExpressionBasis* tableX = getArg(1);
     CS_ExpressionBasis* tableY = getArg(2);
     // we test here "hard" for 1D, for ND we need to rework this
     VERILOG_ERROR_COND_STOP( (inE->getTypes() >= INTEGER_ARRAY) ,
         "First argument in TABLE_MODEL must be a scalar " << inE->getTypes(), this);
     VERILOG_ERROR_COND_STOP( ((tableX->getExprSize() < 2) || (tableX->getTypes() >= STRING)),
         "Currently only 1D TABLE_MODEL is implemented, "
         "therefore second argument must be an array" << tableX->getTypes(), this);
     VERILOG_ERROR_COND_STOP( ((tableY->getExprSize() < 2) || (tableY->getTypes() >= STRING)),
         "Currently only 1D TABLE_MODEL is implemented, "
         "therefore third argument must be an array" << tableY->getTypes(), this);
     VERILOG_ERROR_COND_STOP( (tableY->getExprSize() != tableX->getExprSize()),
             "Different array sizes in TABLE_MODEL " << tableX->getExprSize() << " .vs "
             << tableY->getExprSize(), this);
     if (getArg(3))
       { // if there is a fourth argument than this must be a string
         VERILOG_ERROR_COND_STOP( ((getArg(3)->getTypes() != STRING) || (getArg(3)->getClasses() != CONSTANT)),
                  "Currently only 1D TABLE_MODEL is implemented, therefore fourth "
                  "argument must be constant string" << getArg(3)->getTypes(), this);
       }
     break;
   }

   /* Check syntax of the Laplace operators.
    *
    * IMPORTANT NOTE: Global syntax checks which are true for all four kinds of
    * Laplace operators have to be set in each individual case statement.
    *
    * Depending of the actual kind, a different processing is required.
    * Due to the Verilog-A LRM, the zero- or pole-type representation supports
    * empty lists, which are no longer allowed in the conventional coefficient
    * representation.*/
   case  VERILOG_ANOPFUNC_LAPLACE_ND :  // both num and denom have to be arrays
     {
       CS_ExpressionBasis* inE = getArg(0);
       CS_ExpressionBasis* tableX = getArg(1);
       CS_ExpressionBasis* tableY = getArg(2);
       VERILOG_ERROR_COND_STOP( ((getNrArg() < 3) || (getArg(0) == NULL)
           || (getArg(1) == NULL) || (getArg(2) == NULL)) ,
           "LAPLACE_ND needs at least three valid arguments", this );
       VERILOG_ERROR_COND_STOP( (inE->getTypes() >= INTEGER_ARRAY) ,
           "First argument in LAPLACE_ND must be a scalar " << inE->getTypes(), this);
       VERILOG_ERROR_COND_STOP( ((tableX->getExprSize() < 1) || (tableX->getTypes() >= STRING)),
           "Second argument of LAPLACE_ND must be an array " << tableX->getTypes(), this);
       VERILOG_ERROR_COND_STOP( ((tableY->getExprSize() < 1) || (tableY->getTypes() >= STRING)),
           "Third argument of LAPLACE_ND must be an array " << tableY->getTypes(), this);

       /*
       // For Laplace at the moment we do not accept tolerance arguments
       if ( getNrArg() > 3 )
         VERILOG_WARNING("Ignoring tolerance argument for LAPLACE", getArg(3));
         */

       break;
     }
   case  VERILOG_ANOPFUNC_LAPLACE_NP :   // the numerator has to be an array
     {
       CS_ExpressionBasis* inE = getArg(0);
       CS_ExpressionBasis* tableX = getArg(1);
       CS_ExpressionBasis* tableY = getArg(2);
       VERILOG_ERROR_COND_STOP( ((getNrArg() < 3) || (getArg(0) == NULL)
           || (getArg(1) == NULL) ) ,
           "LAPLACE_NP needs at least three valid arguments", this );
       VERILOG_ERROR_COND_STOP( (inE->getTypes() >= INTEGER_ARRAY) ,
           "First argument in LAPLACE_NP must be a scalar " << inE->getTypes(), this);
       VERILOG_ERROR_COND_STOP( ((tableX->getExprSize() < 1) || (tableX->getTypes() >= STRING)),
           "Second argument of LAPLACE_NP must be an array " << tableX->getTypes(), this);
       if ( tableY!= NULL )  // These checks are only meaningful in the nontrivial case
         {
           VERILOG_ERROR_COND_STOP( tableY->getTypes() >= STRING,
               "Third argument of LAPLACE_NP is invalid " << tableY->getTypes(), this);
           // denominator must have even number of coefficients
           VERILOG_ERROR_COND_STOP( ( ( tableY->getExprSize() ) % 2 != 0 ),
              "LAPLACE_NP must have even number of coefficients in the denominator ", this);
         }

/*
       // For Laplace at the moment we do not accept tolerance arguments
       if ( getNrArg() > 3 )
         VERILOG_WARNING("Ignoring tolerance argument for LAPLACE", getArg(3));
*/
       break;
     }
   case  VERILOG_ANOPFUNC_LAPLACE_ZD :  // the denominator has to be an array
     {
       CS_ExpressionBasis* inE = getArg(0);
       CS_ExpressionBasis* tableX = getArg(1);
       CS_ExpressionBasis* tableY = getArg(2);
       VERILOG_ERROR_COND_STOP( ((getNrArg() < 3) || (getArg(0) == NULL)
           || (getArg(2) == NULL)) ,
           "LAPLACE_ZD needs at least three valid arguments", this );
       VERILOG_ERROR_COND_STOP( (inE->getTypes() >= INTEGER_ARRAY) ,
           "First argument in LAPLACE_ZD must be a scalar " << inE->getTypes(), this);
       if ( tableX!= NULL )  // This check is only meaningful in the nontrivial case
         {
           VERILOG_ERROR_COND_STOP( tableX->getTypes() >= STRING,
               "Second argument of LAPLACE_ZD is invalid " << tableX->getTypes(), this);
           // numerator must have even number of coefficients
           VERILOG_ERROR_COND_STOP( ( ( tableX->getExprSize() ) % 2 != 0 ),
              "LAPLACE_ZD must have even number of coefficients in the denominator ", this);
         }
       VERILOG_ERROR_COND_STOP( ((tableY->getExprSize() < 1) || (tableY->getTypes() >= STRING)),
           "Third argument of LAPLACE_ZD must be an array of length >= 1 " << tableY->getTypes(), this);

       /*
       // For Laplace at the moment we do not accept tolerance arguments
       if ( getNrArg() > 3 )
         VERILOG_WARNING("Ignoring tolerance argument for LAPLACE", getArg(3));
*/
       break;
     }
   case  VERILOG_ANOPFUNC_LAPLACE_ZP :
     {
       CS_ExpressionBasis* inE = getArg(0);
       CS_ExpressionBasis* tableX = getArg(1);
       CS_ExpressionBasis* tableY = getArg(2);
       VERILOG_ERROR_COND_STOP( ( (getNrArg() < 3) || (getArg(0) == NULL) ) ,
           "LAPLACE_ZP needs at least three valid arguments", this );
       VERILOG_ERROR_COND_STOP( (inE->getTypes() >= INTEGER_ARRAY) ,
           "First argument in LAPLACE_ZP must be a scalar " << inE->getTypes(), this);
       if ( tableX != NULL )
         {
           VERILOG_ERROR_COND_STOP( tableX->getTypes() >= STRING,
               "Second argument of LAPLACE_ZP is invalid " << tableX->getTypes(), this);
           // numerator must have even number of coefficients
           VERILOG_ERROR_COND_STOP( ( ( tableX->getExprSize() ) % 2 != 0 ),
              "LAPLACE_ZP must have even number of coefficients in the denominator ", this);
         }
       if ( tableY != NULL )
         {
           VERILOG_ERROR_COND_STOP( tableY->getTypes() >= STRING,
               "Third argument of LAPLACE_ZP is invalid " << tableY->getTypes(), this);
           // denominator must have even number of coefficients
           VERILOG_ERROR_COND_STOP( ( ( tableY->getExprSize() ) % 2 != 0 ),
              "LAPLACE_ZP must have even number of coefficients in the denominator ", this);
         }

       /*
       // For Laplace at the moment we do not accept tolerance arguments
       if ( getNrArg() > 3 )
         VERILOG_WARNING("Ignoring tolerance argument for LAPLACE", getArg(3));
*/
     break;
     }
   default: {break;}
  }

}


CS_AKDependency CS_AnalogFilterOperationFunction::propagateDependencies()
{
  CS_AKDependency retSet;

  switch (opType_) {
    case VERILOG_ANOPFUNC_DDT :
    case VERILOG_ANOPFUNC_IDT :
    case VERILOG_ANOPFUNC_IDTMOD :
    case VERILOG_ANOPFUNC_ABSDELAY  :
    case VERILOG_ANOPFUNC_TRANSITION  :
    case VERILOG_ANOPFUNC_SLEW:
    case VERILOG_ANOPFUNC_LAST_CROSSING  : //"last_crossing"
    case VERILOG_ANOPFUNC_LAPLACE_ND : //"laplace_nd" ;
    case VERILOG_ANOPFUNC_LAPLACE_NP ://"laplace_np" ;
    case VERILOG_ANOPFUNC_LAPLACE_ZD ://"laplace_zd" ;
    case VERILOG_ANOPFUNC_LAPLACE_ZP :
      { // "ddt,idt,absdelay,transition" ;
        int mySysVar = this->getAkSystemVarIndex();
        // there could be only one argument, the results are not needed
        this->getArg(0)->propagateDependencies();
        retSet.addSystemVarIndex( mySysVar , this->getDependencyType() );
        break;
      }
    case VERILOG_ANOPFUNC_DDX   :
      {
        CS_ExpressionBasis* exprRef = getArg(0);
        const CS_BranchProbeFunction* sysVar = dynamic_cast<const CS_BranchProbeFunction*>(getArg(1));

        VERILOG_ERROR_COND_STOP( sysVar == NULL, "Internal Error DDX.", this);
        // this must be the system variable index
        int sysVarInd = ( (sysVar->getActVarDependency().getNrElements() > 0)
            ? (sysVar->getActVarDependency().getSysVars()[0]) : (-1) );
        // no dependency for ddx, since at the moment we do not have 2nd derivatives
        if ( ddx_Deriv_expr_ == 0 )
          {
            int tmp = 0;
            ddx_Deriv_expr_ = exprRef->doDifferentiate( sysVarInd, &tmp );
          }
        // to the outside "retSet" is zero since to the outside there is no derivative dependancy
        // since we do not have second order derivatives
        break;
      }
    case VERILOG_ANOPFUNC_LIMIT   :
    case VERILOG_ANOPFUNC_LIMEXP   :
    case VERILOG_ANOPFUNC_TABLE: //  for ND we need not just the first argument
      {  //"limexp" ;
        // this function has only
        retSet = this->getArg(0)->propagateDependencies();
        break;
      }
    default: {
      VERILOG_PRINT_L2( verb(), "CS_AnalogFilterOperationFunction::propagateDependencies" <<
          "not implemented for this function type :" << opType_ );
      VERILOG_ERROR_STOP( "Internal Error", this);
      break;
    }
  }

  // store the combined dependency in the actual node
  setActVarDependency(retSet);
  // return the dependencies
  return retSet;
}

int CS_AnalogFilterOperationFunction::getClasses() const{
  // this expression denpends on internal variables, since the function calls are variables
  switch (opType_)
  {
    case VERILOG_ANOPFUNC_DDT : {return INTERNALVAR; break;}         // "ddt" ;
    case VERILOG_ANOPFUNC_TRANSITION : {return INTERNALVAR; break;}  //"transition" ;
    case VERILOG_ANOPFUNC_LAST_CROSSING : {return INTERNALVAR; break;}  //"last_crossing" ;
    case VERILOG_ANOPFUNC_LIMEXP   : {return this->getArg(0)->getClasses(); break;}     //"limexp" ;
    case VERILOG_ANOPFUNC_LIMIT: { return this->getArg(0)->getClasses(); break; } // "limit"
    case VERILOG_ANOPFUNC_DDX: { return this->getArg(0)->getClasses(); break; } // "ddx"
    case VERILOG_ANOPFUNC_TABLE: { return this->getArg(0)->getClasses(); break;} //"table_model for ND do here more
    default: { return INTERNALVAR; break; }
  }
  return INTERNALVAR; //
}

int CS_AnalogFilterOperationFunction::getTypes() const {
  // the types of the returned variables are REALS
  return REAL; //
}


CS_ExpressionBasis* CS_AnalogFilterOperationFunction::doDifferentiate(int sysVarIndex, int *flag){

  // test if this dependent on the chosen system variable
  if (this->getActVarDependency().isSysVarIndexPresent(sysVarIndex) < 0)
    return 0;

  // based on the type to different things
  switch (opType_)
  {
    case VERILOG_ANOPFUNC_IDT :     // "idt" ;
    case VERILOG_ANOPFUNC_IDTMOD :  // "idtmod" ;
    case VERILOG_ANOPFUNC_DDT :     // "ddt" ;
    case VERILOG_ANOPFUNC_TRANSITION : //"transition" ;
    case VERILOG_ANOPFUNC_ABSDELAY  : // "absdelay" ;
    case VERILOG_ANOPFUNC_SLEW : // slew
    case VERILOG_ANOPFUNC_LAST_CROSSING : //"last_crossing" ;
    case VERILOG_ANOPFUNC_LAPLACE_ND : //"laplace_nd" ;
    case VERILOG_ANOPFUNC_LAPLACE_NP ://"laplace_np" ;
    case VERILOG_ANOPFUNC_LAPLACE_ZD ://"laplace_zd" ;
    case VERILOG_ANOPFUNC_LAPLACE_ZP :
      {
        // the derivative with respect to this system variable is simple to compute, since these
        // function calls represent a function variable in itself
        if ( this->getAkSystemVarIndex() == sysVarIndex)
          return (new CS_Number(1.0, getModule(), getStartLine(), getStartColumn()));
        else
          return (new CS_Number(0.0, getModule(), getStartLine(), getStartColumn()));
      }
    case VERILOG_ANOPFUNC_LIMIT    :
      {
        // for the limit just pass the differentitation of the first argument
        return this->getArg(0)->doDifferentiate(sysVarIndex,flag);
      }
    case VERILOG_ANOPFUNC_TABLE:
      {
        // IMPORTANT: for table_model we need "coeff*(dV/ds), but the multiplication
        // with a coeff will be added in the code generation,
        // Therefore at the point "this->getAkName()" MUST BE SET CORRECTLY!!
        CS_ExpressionBasis *coef = new CS_Number( this->getAkName(), getModule(), getStartLine(), getStartColumn(), true);
        return (new CS_BinaryOperator( VERILOG_BINAR_MUL, coef, getArg(0)->doDifferentiate(sysVarIndex,flag) ,
            getModule(), getStartLine(), getStartColumn()));
      }
    case VERILOG_ANOPFUNC_LIMEXP   :
      {
        CS_AnalogFilterOperationFunction *retExp = 0;
        CS_ExpressionBasis* args[1] = { this->getArg(0) };
        retExp = new CS_AnalogFilterOperationFunction( opType_ ,
            getModule(), getStartLine(), getStartColumn(), args, 1 );
        // set the generate name to the derivative temporary variable
        retExp->setAkGenerateName( "_d" + this->getAkGenerateName() + "_" +
            verilog_integerToString(sysVarIndex) );
        return retExp;
      }
    case VERILOG_ANOPFUNC_DDX : // ddx
      {
        // DDX has no (official) derivatives
        return 0;
      }
    default: {
      return 0;
    }
  }
  return 0;
}
