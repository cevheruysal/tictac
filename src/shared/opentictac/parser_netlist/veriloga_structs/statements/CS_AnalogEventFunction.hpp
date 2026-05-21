/*
 * CS_AnalogEventFunction.hpp
 *
 *  Created on: Dec 20, 2012
 *      Author: benk
 */

#ifndef EVENTFUNCTION_HPP_
#define EVENTFUNCTION_HPP_

#include "CS_ExpressionBasis.hpp"
#include "verilog_comp.hpp"
#include "CS_FunctionCall.hpp"

/** the enum type for event functions */
typedef enum{
   VAMSEVENT_INITIAL_STEP = 0,    /* Global event 'initial_step' */
   VAMSEVENT_FINAL_STEP = 1,      /* Global event 'final_step'   */
   VAMSEVENT_CROSS = 2,           /* Monitored event 'cross'     */
   VAMSEVENT_ABOVE = 3,           /* Monitored event 'above'     */
   VAMSEVENT_TIMER = 4            /* Monitored event 'timer'     */
} EventFuncType;


/** class that models the various types of events (e.g., cross, above ...)
 *
 * see analog_event_functions in LRM */
class CS_AnalogEventFunction : public CS_FunctionCall
{
public:

  /** */
  CS_AnalogEventFunction(EventFuncType ft, CS_ExpressionBasis **args,
      int nrArg, const CS_ModuleDeclaration* module, int startLine, int startColumn);

  /** empty Dtor*/
  virtual ~CS_AnalogEventFunction() {;}

  /** method for the tree traversal */
  virtual void Accept(CS_CompilerVisitorBasis &visitor)
  {
    visitor.Visit( (CS_AnalogEventFunction &)(*this));
  }

  /** returns the type of the event function */
  EventFuncType getFunctionType() const { return functionType_; }

  /** returns the Eval index of the first argument of this function for the AK*/
  int getStartAKEvalInd() const { return startAKEvalInd_; }

  /** sets the start index of the AK eval list*/
  void setStartAKEvalInd(int newind) { startAKEvalInd_ = newind; }

  /** gets the kind string for the AK data generation */
  std::string& getAKKind() { return kindInString_; }
  /** sets the kind string for the AK data structure generation */
  void setAKKind(std::string kind) { kindInString_ = kind;}

  /** returns the rest of the string that is used for the AK data structure*/
  std::string& getAKRestString() { return restAKGenerate_; }
  /** sets the string for the AK data structure generation*/
  void setAKRestString(std::string restString) { restAKGenerate_ = restString;}
  /** adds to the AK data structure string */
  void addAKRestString(std::string restString) { restAKGenerate_ += restString;}

  /** returns its index in the event list*/
  int getAkEventIndex() const { return akEventIndex_; }
  /** sets the index in the event list of the module */
  void setAkEventIndex(int newEventIndex ) { akEventIndex_ = newEventIndex; }

  virtual std::string eval(VarType) const {
    VERILOG_PRINT_L3(verb(),"CS_AnalogEventFunction can not be evaluated ");
    VERILOG_ERROR_STOP( "Internal Error", this );
    return "";
  }

  virtual int getClasses() const{
    VERILOG_PRINT_L3(verb(),"CS_AnalogEventFunction has no class ");
    VERILOG_ERROR_STOP( "Internal Error", this );
    return 0;
  }

  virtual int getTypes() const {
    VERILOG_PRINT_L3(verb(),"CS_AnalogEventFunction has no type ");
    VERILOG_ERROR_STOP( "Internal Error", this );
    return 0;
  }

  /** function that creates a deep copy of the expression itself */
  virtual CS_ExpressionBasis* copy() {
    // copy the arguments
    CS_ExpressionBasis *arg[30];
    for (int i = 0; i < this->getNrArg(); i++)
      arg[i] = this->getArg(i)->copy();

    // here we do not copy the arguments
    return (new CS_AnalogEventFunction(functionType_, arg,
        this->getNrArg(), this->getModule(),
        this->getStartLine(), this->getStartColumn()));
    return 0;
  }

private:

  /** type of the function */
  EventFuncType functionType_;

  // =========== AK specific =======
  /** its index in the event list */
  int akEventIndex_;

  /** */
  int startAKEvalInd_;

  std::string kindInString_;

  std::string restAKGenerate_;

};

#endif /* EVENTFUNCTION_HPP_ */
