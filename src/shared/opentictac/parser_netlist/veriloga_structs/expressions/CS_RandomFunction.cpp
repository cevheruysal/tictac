/*
 * CS_RandomFunction.cpp
 *
 *  Created on: Aug 14, 2014
 *      Author: benk
 */

#include "CS_RandomFunction.hpp"
#include "CS_VariableReference.hpp"
#include "CS_ParameterReference.hpp"
#include "CS_String.hpp"

CS_RandomFunction::CS_RandomFunction(RandomFuncCallType ft,
    CS_ExpressionBasis** args, int nrArgs,
    const CS_ModuleDeclaration* module, int startLine, int startColumn) :
    CS_FunctionCall( args, nrArgs, module, startLine, startColumn, false),
    type_(ft), seed_var_(NULL), seed_par_(NULL), seed_expr_(NULL)
{

  seed_expr_ = getArg(0);
  CS_VariableReference* seed = dynamic_cast<CS_VariableReference*>(getArg(0));
  seed_par_ =  dynamic_cast<CS_ParameterReference*>(getArg(0));
  seed_var_ = seed;
  isGlobal_or_Instance_ = VERILOG_RANDOMFUNC_NOT_GIVEN;

  //VERILOG_ERROR_COND_STOP( ((getNrArg() > 0) && (getArg(0)) && (getArg(0)->getTypes() > INTEGER) ),
  //                  "First argument should be of integer type ", this);
  //VERILOG_ERROR_COND_STOP( ((getNrArg() > 0) && (getArg(0)) && (seed == NULL) ),
  //                  "First argument should be an integer variable ", this);

  switch (type_)
  {
    //TODO: check the function's argument for each type
    case VERILOG_RANDOMFUNC_RANDOM: {
      VERILOG_ERROR_COND_STOP( (getNrArg() > 1) ,
                  "$random takes only one seed variable as argument", this);
      break;
    }
    case VERILOG_RANDOMFUNC_ARANDOM   : {
      VERILOG_ERROR_COND_STOP( (getNrArg() > 2) ,
                  "$arandom takes at most two arguments", this);
      break;
    }
    case VERILOG_RANDOMFUNC_RDISTNORMAL   : {
      VERILOG_ERROR_COND_STOP( (getNrArg() < 3) ,
                  "$rdist_normal needs at least three arguments", this);
      VERILOG_ERROR_COND_STOP( (getNrArg() > 4) ,
                  "$rdist_normal takes at most four arguments", this);
      break;
    }
    case VERILOG_RANDOMFUNC_RDISTUNIFORM: {
      VERILOG_ERROR_COND_STOP( (getNrArg() < 3) ,
                   "$rdist_uniform needs at least three arguments", this);
       VERILOG_ERROR_COND_STOP( (getNrArg() > 4) ,
                   "$rdist_uniform takes at most four arguments", this);
       break;
    }
    default: {
      VERILOG_ERROR_STOP("Internal Error: unknown RandomFunction",this);
    }
  }

  // check the last argument if it is string
  if ( (getNrArg() > 0) && (getArg(getNrArg()-1)) ) {
      if (getArg(getNrArg()-1)->getTypes() == STRING)
        {
          const CS_String * s = dynamic_cast<const CS_String *>(getArg(getNrArg()-1));
          VERILOG_ERROR_COND_STOP(s == NULL,
            "Function only accepts \"instance\" or \"global\" given as constant string", this);
          std::string ret = s->getStringValue();
          VERILOG_ERROR_COND_STOP( (ret != "instance") && (ret != "global"),
            "Function only accepts \"instance\" or \"global\" as random type specification", this);
          // store which type of function this is "global" or "instance" or not given
          if (ret == "global")
            isGlobal_or_Instance_ = VERILOG_RANDOMFUNC_GLOBAL;
          if (ret == "instance")
            isGlobal_or_Instance_ = VERILOG_RANDOMFUNC_INSTANCE;
        }
  }
}


bool CS_RandomFunction::hasSameSeed( const CS_RandomFunction* func) const
{
  bool same = false;
  // test if these function calls have the same variable or parameter
  if ( (seed_par_ !=  NULL) &&  (func->getParameterSeed() != NULL) )
    {
      if (seed_par_->getParamObject() == func->getParameterSeed()->getParamObject())
        same = true;
    }
  if ( (seed_var_ !=  NULL) &&  (func->getVariableSeed() != NULL) )
    {
      if (seed_var_->getvalueObj() == func->getVariableSeed()->getvalueObj())
        same = true;
    }
  return same;
}
