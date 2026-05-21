/*
 * CS_SystemFunction.cpp
 *
 *  Created on: Dec 28, 2012
 *      Author: benk
 */

#include "CS_SystemFunction.hpp"
#include "CS_String.hpp"

CS_SystemFunction::CS_SystemFunction(SystemFuncCallType ty,
    CS_ExpressionBasis** args, int nrArgs,
    const CS_ModuleDeclaration* module, int startLine, int startColumn) :
    CS_FunctionCall( args, nrArgs, module, startLine, startColumn, false), type_(ty)
{
  for (int i = 0 ; i < this->getNrArg() ; i++)
    VERILOG_ERROR_COND_STOP( !this->getArg(i), "This function can not have default arguments", this);

  VERILOG_PRINT_L4( verb(), " CS_SystemFunction::CS_SystemFunction type= " << this->getArg(0)->getTypes()  );

  switch (type_)
  {
    //TODO: check the function's argument for each type
    case VERILOG_SYSFUNC_DISPLAY: {break;}
    case VERILOG_SYSFUNC_STROBE: {break;}
    case VERILOG_SYSFUNC_MONITOR: {break;}
    case VERILOG_SYSFUNC_DEBUG: {break;}
    case VERILOG_SYSFUNC_WRITE: {break;}
    case VERILOG_SYSFUNC_FINISH: {break;}
    case VERILOG_SYSFUNC_STOP: {break;}
    case VERILOG_SYSFUNC_WARNING: {break;}
    case VERILOG_SYSFUNC_ERROR:  {break;}
    case VERILOG_SYSFUNC_INFO:  {break;}
    case VERILOG_SYSFUNC_FATAL: {break;}

    case VERILOG_SYSFUNC_DISCONTINUITY: {
      VERILOG_ERROR_COND_STOP( this->getNrArg() > 1 , "DISCONTINUITY takes at most one argument", this );
      if (this->getNrArg() > 0) {
          VERILOG_ERROR_COND_STOP( (this->getArg(0)->getTypes() != INTEGER) &&
                                   (this->getArg(0)->getTypes() != NO_TYPE) ,
              "DISCONTINUITY needs one integer argument " << this->getArg(0)->getTypes(), this );
      }
      break;}

    case VERILOG_SYSFUNC_BOUNDSTEP: {
      VERILOG_ERROR_COND_STOP( this->getNrArg() != 1 , "BOUND_STEP takes one argument", this );
      if (this->getNrArg() > 0) {
          VERILOG_ERROR_COND_STOP( (this->getArg(0)->getTypes() > (REAL | INTEGER)),
              "BOUND_STEP needs one scalar argument", this );
      }
      break;}

    case VERILOG_SYSFUNC_FGETS: {      // first argument must be an integer
      VERILOG_ERROR_COND_STOP( this->getNrArg() < 2 , "FGETS needs at least two arguments", this );
      VERILOG_ERROR_COND_STOP( ( (this->getArg(0)->getTypes() != (int)STRING) ||
          (this->getArg(0)->getClasses() != (int)INTERNALVAR) ),
          "First argument of FGETS must be a string variable" , this);
      VERILOG_ERROR_COND_STOP( ( (this->getArg(1)->getTypes() != (int)INTEGER) ),
          "Second argument of FGETS must be an integer" , this);
      break;}

    case VERILOG_SYSFUNC_FCLOSE: {
      VERILOG_ERROR_COND_STOP( this->getNrArg() < 1 , "FCLOSE needs at least one argument", this );
      VERILOG_ERROR_COND_STOP( this->getArg(0)->getTypes() != (int)INTEGER,
          "First argument of FCLOSE must be an integer" , this);
      break;}

    case VERILOG_SYSFUNC_FDISPLAY:
    case VERILOG_SYSFUNC_FDEBUG:
    case VERILOG_SYSFUNC_FSCANF:
    case VERILOG_SYSFUNC_FSTROBE:
    case VERILOG_SYSFUNC_FWRITE:{
      VERILOG_ERROR_COND_STOP( this->getNrArg() < 2 , "This FILE function takes at least two arguments", this );
      // first argument must be an integer
      VERILOG_ERROR_COND_STOP( this->getArg(0)->getTypes() != (int)INTEGER,
          "First argument of this FILE function must be an integer, function type " << type_ , this);
      break;
    }

    case VERILOG_SYSFUNC_FOPEN: {
      // if there is only one argument, we should add by default "r+"
      if (this->getNrArg() == 1)
        {
          std::string defVal ="w";
          CS_ExpressionBasis* argDefault = new CS_String(defVal, this->getModule(),
                                                 this->getStartLine(), this->getStartColumn() );
          this->addNewArgument( argDefault );
        }
      VERILOG_ERROR_COND_STOP( this->getNrArg() < 2 , "FOPEN takes at least two arguments", this );
      VERILOG_ERROR_COND_STOP( (this->getArg(0)->getTypes() != (int)STRING) ,
          "First argument of FOPEN must be a constant string" , this);
      VERILOG_ERROR_COND_STOP( ( (this->getArg(1)->getTypes() != (int)STRING) ||
          (this->getArg(1)->getClasses() != (int)CONSTANT) ),
          "Second argument of FOPEN must be a constant string" , this);
      break;
    }

    case VERILOG_SYSFUNC_REWIND:
    case VERILOG_SYSFUNC_FTELL:
    case VERILOG_SYSFUNC_FFLUSH  :
    case VERILOG_SYSFUNC_FEOF    : {
      VERILOG_ERROR_COND_STOP( this->getNrArg() != 1 , "This FILE function takes only one argument", this );
      // first argument must be an integer
      VERILOG_ERROR_COND_STOP( this->getArg(0)->getTypes() != (int)INTEGER,
          "First argument of this FILE function must be an integer, function type " << type_ , this);
      break;
    }

    case VERILOG_SYSFUNC_FSEEK   : {
      VERILOG_ERROR_COND_STOP( this->getNrArg() != 3 , "FSEEK takes only three arguments", this );
      // first argument must be an integer
      VERILOG_ERROR_COND_STOP( this->getArg(0)->getTypes() != (int)INTEGER, "First argument of FSEEK must be an integer " , this);
      VERILOG_ERROR_COND_STOP( this->getArg(1)->getTypes() != (int)INTEGER, "Second argument of FSEEK must be an integer " , this);
      VERILOG_ERROR_COND_STOP( this->getArg(2)->getTypes() != (int)INTEGER, "Third argument of FSEEK must be an integer " , this);
      break;
    }

    case VERILOG_SYSFUNC_SWRITE:
    case VERILOG_SYSFUNC_SFORMAT:
    case VERILOG_SYSFUNC_SSCANF:  {
      // first argument must be a string
      VERILOG_ERROR_COND_STOP( ( (this->getArg(0)->getTypes() != (int)STRING) ||
          (this->getArg(0)->getClasses() != (int)INTERNALVAR) ),
          "First argument of this STRING function must be a string variable, function type " << type_ , this);
      VERILOG_ERROR_COND_STOP( ( (this->getArg(1)->getTypes() != (int)STRING) ||
          (this->getArg(1)->getClasses() != (int)CONSTANT) ),
          "Second argument of his STRING function must be a constant string, function type " << type_ , this);
      // TODO: only string Variables can be written, test from the 2nd argument
      // if it a string then it must be a variable
      break;
    }

    case VERILOG_SYSFUNC_FMONITOR:
    case VERILOG_SYSFUNC_FERROR:
    case VERILOG_SYSFUNC_FREAD:
    default:
      { // some functions are not supported
        VERILOG_ERROR_STOP("SystemFunction of this type (" << type_
          << ") not supported at the moment", this);
        break;
      }
  }
  //this->setVerb(6);
}

int CS_SystemFunction::getClasses() const
{
  // if there is a returned function it is a constant (no derivatives needed)
  return (int)CONSTANT;
}


int CS_SystemFunction::getTypes() const
{
  switch (this->type_) {
  case VERILOG_SYSFUNC_DISPLAY: { return INTEGER; }
  case VERILOG_SYSFUNC_STROBE:  { return INTEGER; }
  case VERILOG_SYSFUNC_MONITOR: { return INTEGER; }
  case VERILOG_SYSFUNC_DEBUG:   { return INTEGER; }
  case VERILOG_SYSFUNC_WRITE:   { return INTEGER; }

  // Simulation control tasks
  case VERILOG_SYSFUNC_FINISH:  { return INTEGER; }
  case VERILOG_SYSFUNC_STOP:    { return INTEGER; }
  case VERILOG_SYSFUNC_WARNING: { return INTEGER; }
  case VERILOG_SYSFUNC_ERROR:   { return INTEGER; }
  case VERILOG_SYSFUNC_INFO:    { return INTEGER; }
  case VERILOG_SYSFUNC_FATAL:   { return INTEGER; }

  // File operator functions and further string functions
  case VERILOG_SYSFUNC_FCLOSE:  { return INTEGER; }
  case VERILOG_SYSFUNC_FDISPLAY:{ return INTEGER; }
  case VERILOG_SYSFUNC_FMONITOR:{ return INTEGER; }
  case VERILOG_SYSFUNC_SWRITE:  { return INTEGER; }
  case VERILOG_SYSFUNC_SFORMAT: { return INTEGER; }
  case VERILOG_SYSFUNC_FSCANF:  { return INTEGER; }
  case VERILOG_SYSFUNC_FREAD:   { return INTEGER; }
  case VERILOG_SYSFUNC_FSEEK:   { return INTEGER; }
  case VERILOG_SYSFUNC_FOPEN:   { return INTEGER; }
  case VERILOG_SYSFUNC_FSTROBE: { return INTEGER; }
  case VERILOG_SYSFUNC_FERROR:  { return INTEGER; }
  case VERILOG_SYSFUNC_REWIND:  { return INTEGER; }
  case VERILOG_SYSFUNC_FWRITE:  { return INTEGER; }
  case VERILOG_SYSFUNC_SSCANF:  { return INTEGER; }
  case VERILOG_SYSFUNC_FTELL:   { return INTEGER; }
  case VERILOG_SYSFUNC_FGETS:   { return INTEGER; }
  default : { return INTEGER; }
  }
  // if there is a returned value then it is real
  return (int)INTEGER;
}
