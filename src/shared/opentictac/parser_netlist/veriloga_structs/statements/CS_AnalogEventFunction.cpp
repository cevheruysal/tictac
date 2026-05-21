/*
 * CS_AnalogEventFunction.cpp
 *
 *  Created on: Dec 20, 2012
 *      Author: benk
 */

#include "CS_AnalogEventFunction.hpp"
#include "CS_String.hpp"

CS_AnalogEventFunction::CS_AnalogEventFunction(EventFuncType ft, CS_ExpressionBasis **args,
    int nrArg, const CS_ModuleDeclaration* module, int startLine, int startColumn):
    CS_FunctionCall(args, nrArg,  module, startLine, startColumn, true), functionType_(ft)
{
  startAKEvalInd_ = -1;
  akEventIndex_ = -1;
  kindInString_ = "";
  restAKGenerate_ = "";

  // do some checks for the arguments
  if (    (functionType_ == VAMSEVENT_INITIAL_STEP)
       || (functionType_ == VAMSEVENT_FINAL_STEP) )
    {
      for (int i = 0; i < nrArg ; i++)
        {
          // arguments might be null
          if (!getArg(i)) continue;

          // visit the arguments only if they are not strings
          if (getArg(i)->getTypes() < STRING)
            {
              VERILOG_ERROR_STOP("INITIAL_STEP and FINAL_STEP function take only string as arguments ", this);
            }

          // for such case this must be string argument (and no other)
          CS_String *str = dynamic_cast<CS_String*>(getArg(i));
          if (str == NULL)
            VERILOG_ERROR_STOP("INITIAL_STEP and FINAL_STEP take only: ac, dc, tran, "
                "and noise as string argument ", this);

          std::string arg = str->getStringValue();
          // in LRM "static" or others are not present, but will be defined as "unknown"
          if ( (arg != "ac") && (arg != "dc") && (arg != "tran") && (arg != "noise") )
            {
              str->setStringValue( "none" );
              VERILOG_WARNING("INITIAL_STEP and FINAL_STEP take only: "
                  "ac, dc, tran and noise as argument, other values will be ignored.", this);
            }
        }
    }

  // test the number of arguments
  switch (functionType_)
  {
  case VAMSEVENT_INITIAL_STEP :
  case VAMSEVENT_FINAL_STEP : {
    VERILOG_ERROR_COND_STOP( this->nrArgs_ > 6 , "INITIAL_STEP and FINAL_STEP take at most six arguments ", this);
    break; }
  case VAMSEVENT_CROSS : {
    VERILOG_ERROR_COND_STOP( this->nrArgs_ > 5 , "CROSS takes at most five arguments ", this);
    break; }
  case VAMSEVENT_ABOVE : {
    VERILOG_ERROR_COND_STOP( this->nrArgs_ > 4 , "ABOVE takes at most four arguments ", this);
    break; }
  case VAMSEVENT_TIMER : {
    VERILOG_ERROR_COND_STOP( this->nrArgs_ > 4 , "TIMER takes at most four arguments ", this);
    break; }
  }
}

