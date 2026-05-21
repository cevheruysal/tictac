/*
 * CS_AnalogPortReference.hpp
 *
 *  Created on: Jan 10, 2013
 *      Author: benk
 */

#ifndef CS_ANALOGPORTREFERENCE_HPP_
#define CS_ANALOGPORTREFERENCE_HPP_

#include "CS_Port.hpp"
#include "CS_IndexedIdentifier.hpp"

/** class that models the A and A[i] constructs where A is a port (external or internal)
 *
 * see analog_port_reference (branch_reference) in LRM
 * (branch_reference will be transformed to analog_port_reference during parsing)
 * */
class CS_AnalogPortReference : public CS_IndexedIdentifier
{
public:

  /** empty Ctor */
  CS_AnalogPortReference(CS_Port *portObj,  CS_ExpressionBasis *indexExpr,
      const CS_ModuleDeclaration* module, int startLine, int startColumn);

  /** empty Dtor */
  virtual ~CS_AnalogPortReference() {;}

  /** returns the Port object of this class*/
  CS_Port* getPortObject() { return port_; }
  const CS_Port* getPortObject() const { return port_; }

  /** method for the tree traversal */
  virtual void Accept(CS_CompilerVisitorBasis &visitor) { visitor.Visit( (CS_AnalogPortReference &)(*this)); }

  /** function that creates a deep copy of the expression itself */
  virtual CS_ExpressionBasis* copy() {
    CS_ExpressionBasis* indexer = (this->getIndexerExpr())?this->getIndexerExpr()->copy():0;
    // here we do not copy the arguments
    return (new CS_AnalogPortReference(
        port_, indexer, this->getModule(),
        this->getStartLine(), this->getStartColumn()));
  }

  /** propagate recursively the system dependency informations */
  virtual CS_AKDependency propagateDependencies();

  virtual CS_ExpressionBasis* doDifferentiate([[maybe_unused]] int sysVarIndex, [[maybe_unused]] int *flag) {
    VERILOG_PRINT_L2( verb(), " doDifferentiate function not implemented for CS_AnalogPortReference" );
    VERILOG_ERROR_STOP("Port can not be used directly in expressions", this);
    return 0;
  }

private:

  /** the port object, this is a copy from the object in the super class */
  CS_Port *port_;

};

#endif /* CS_ANALOGPORTREFERENCE_HPP_ */
