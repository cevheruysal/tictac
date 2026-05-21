/*
 * CS_SystemFunction.hpp
 *
 *  Created on: Dec 28, 2012
 *      Author: benk
 */

#ifndef CS_SYSTEMFUNCTION_HPP_
#define CS_SYSTEMFUNCTION_HPP_

#include "CS_FunctionCall.hpp"

/** types of the functions, that we call as system functions.
 * These functions simply require or provide an information to/from the Verilog code */
typedef enum {
  VERILOG_SYSFUNC_DISPLAY          = 0,  // display
  VERILOG_SYSFUNC_STROBE           = 1,  // strobe
  VERILOG_SYSFUNC_MONITOR          = 2,  // monitor
  VERILOG_SYSFUNC_DEBUG            = 3,  // debug
  VERILOG_SYSFUNC_WRITE            = 4,  // write

  // Simulation control tasks
  VERILOG_SYSFUNC_FINISH           = 5,  // finish
  VERILOG_SYSFUNC_STOP             = 6,  // stop
  VERILOG_SYSFUNC_WARNING          = 7,  // warning
  VERILOG_SYSFUNC_ERROR            = 8,  // error
  VERILOG_SYSFUNC_INFO             = 9,  // info
  VERILOG_SYSFUNC_FATAL            = 10, // fatal
  VERILOG_SYSFUNC_DISCONTINUITY    = 15, // discontinuity
  VERILOG_SYSFUNC_BOUNDSTEP        = 16, // bound step

  // File operator functions and further string functions
  VERILOG_SYSFUNC_FCLOSE        = 21,
  VERILOG_SYSFUNC_FDISPLAY      = 22,
  VERILOG_SYSFUNC_FDEBUG        = 23,
  VERILOG_SYSFUNC_FMONITOR      = 24, // not supported yet ... ?
  VERILOG_SYSFUNC_FSCANF        = 25,
  VERILOG_SYSFUNC_FREAD         = 26,
  VERILOG_SYSFUNC_FSEEK         = 27,
  VERILOG_SYSFUNC_FOPEN         = 28,
  VERILOG_SYSFUNC_FSTROBE       = 29,
  VERILOG_SYSFUNC_FERROR        = 30,
  VERILOG_SYSFUNC_REWIND        = 31,
  VERILOG_SYSFUNC_FWRITE        = 32,
  VERILOG_SYSFUNC_FTELL         = 33,
  VERILOG_SYSFUNC_FGETS         = 34,
  VERILOG_SYSFUNC_FFLUSH        = 35,
  VERILOG_SYSFUNC_FEOF          = 36,
  VERILOG_SYSFUNC_SSCANF        = 37,
  VERILOG_SYSFUNC_SWRITE        = 38,
  VERILOG_SYSFUNC_SFORMAT       = 39,
  VERILOG_SYSFUNC_NOFUNCTION       = 100
} SystemFuncCallType;


/** class to model system function calls (that do not require special VAMS function call from the AK)
 *
 * In this class we put many of the syntactical entities that we thought should not be
 * put into a different file. Later however this class might be split into many others
 * see display_tasks_in_analog_block and "Simulation control tasks" in LRM
 * Furthermore we added the file specific functions here as well see chapter 9.5 in LRM */
class CS_SystemFunction : public CS_FunctionCall
{

public:

  /** Ctor for the system function no arguments */
  CS_SystemFunction(SystemFuncCallType ty,
      CS_ExpressionBasis** args, int nrArgs,
      const CS_ModuleDeclaration* module, int startLine, int startColumn);

  virtual ~CS_SystemFunction(){;}

   /** returns the classes that can be found in an expression (constant, ports, parameters, variables)*/
   virtual int getClasses() const;

   /** returns the types (actual INTEGER or REAL)*/
   virtual int getTypes() const;

   /** method for the tree traversal */
   virtual void Accept(CS_CompilerVisitorBasis &visitor) { visitor.Visit( (CS_SystemFunction &)(*this)); }

   /** returns the type of this system function */
   SystemFuncCallType getFunctionType() const { return type_; }

   /** propagateDependencies() call of CS_SystemFunction should not appear in composed expressions */
   virtual CS_AKDependency propagateDependencies()
     {
       VERILOG_PRINT_L3(verb(), "propagateDependencies() within a SystemFunctionCall should not be called ");
       //VERILOG_ERROR_STOP(" propagateDependencies() within a SystemFunctionCall should not be called  ", this);
       return CS_AKDependency();
     }

   /** function that creates a deep copy of the expression itself */
   virtual CS_ExpressionBasis* copy() {
     // copy the arguments
     CS_ExpressionBasis *arg[30];
     for (int i = 0; i < this->getNrArg(); i++)
       arg[i] = this->getArg(i)->copy();

     // here we do not copy the arguments
     return (new CS_SystemFunction(type_, arg,
         this->getNrArg(), this->getModule(),
         this->getStartLine(), this->getStartColumn()));
   }

   /** in the generate we use temporary buffers, this is its size*/
   static int getBufferSize() { return 2050; }

   /** the function above but in string format  */
   static std::string getBufferSizeStr() { return verilog_integerToString(getBufferSize()); }

private:

  /** type of the system function */
  SystemFuncCallType type_;

};

#endif /* CS_SYSTEMFUNCTION_HPP_ */
