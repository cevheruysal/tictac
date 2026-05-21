/*
 * CS_AnalogDispOrSysFunc.hpp
 *
 *  Created on: Mar 4, 2013
 *      Author: benk
 */

#ifndef CS_ANALOGDISPORSYSFUNC_HPP_
#define CS_ANALOGDISPORSYSFUNC_HPP_

#include "verilog_comp.hpp"
#include "CS_AnalogStatement.hpp"

#include "CS_SystemFunction.hpp"

/** this statement represents a simple statement, with a simple function call
 * that can be either for display or error purposes
 *
 * see in LRM assert_severity_task and display_tasks_in_analog_block */
class CS_AnalogDispOrSysFunc : public CS_AnalogStatement
{
public:

  /** Ctor with the given parameters */
  CS_AnalogDispOrSysFunc(CS_SystemFunction *funcCalled,
      const CS_ModuleDeclaration* module, int startLine, int startColumn);

  /** empty Dtor */
  virtual ~CS_AnalogDispOrSysFunc() {;}

  /** method for the tree traversal */
  virtual void Accept(CS_CompilerVisitorBasis &visitor) { visitor.Visit( (CS_AnalogDispOrSysFunc &)(*this) ); }

  /** returns the system function that is called */
  CS_SystemFunction* getFunctionCalled() { return funcCalled_; }
  const CS_SystemFunction* getFunctionCalled() const { return funcCalled_; }

private:

  /** the function that is called */
  CS_SystemFunction* funcCalled_;
};

#endif /* CS_ANALOGDISPORSYSFUNC_HPP_ */
