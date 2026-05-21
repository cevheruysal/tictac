/*
 * CS_ModuleInstantiation.hpp
 *
 *  Created on: Feb 1, 2013
 *      Author: benk
 */

#ifndef CS_MODULEINSTANTIATION_HPP_
#define CS_MODULEINSTANTIATION_HPP_

#include "CS_CompilerTreeNode.hpp"
#include "CS_ErrorInfo.hpp"

// forward declaration of the expression class
class CS_ExpressionBasis;
class CS_ModuleDeclaration;

/** */
typedef enum {
  VERILOG_SPICE_NO_TYPE = 0,
  VERILOG_SPICE_RES = 1,
  VERILOG_SPICE_CAP = 2,
  VERILOG_SPICE_IND = 3,
  /* todo: later other spice primitives or Verilog module instances*/
  /* type for Verilog indexing */
  VERILOG_MODULE_INSTANCE = 50
} ModuleInstanceType;

/**
 * see module_keyword module_identifier, module_parameter_port_list,
 * and module_parameter_port_list in LRM */
class CS_ModuleInstantiation : public CS_CompilerTreeNode, public CS_ErrorInfo
{
public:

  /** Ctor to initialize all the required fields: instance name, parameter names
   * and their initial values at last the ports where this is connceted */
  CS_ModuleInstantiation(ModuleInstanceType instType, std::string instName,
      std::string moduleName,
      std::vector<std::string>& paramNames,
      std::vector<CS_ExpressionBasis*>& parameValues,
      std::vector<CS_ExpressionBasis*>& portConnects,
      const CS_ModuleDeclaration* module, int startLine, int startColumn );

  /** empty Dtor */
  virtual ~CS_ModuleInstantiation() {;}

  /** returns the instance name*/
  const std::string& getInstName() const { return instName_; }

  /** returns the module name*/
  const std::string& getModuleName() const { return moduleName_; }

  /** returns the type of this instantiation */
  ModuleInstanceType getInstType() const { return instType_; }

  /** number of parameters set*/
  int getNrParameters() const { return (int) paramNames_.size();}

  /** returns the name of the given parameter*/
  std::string& getParamName(int i) { return paramNames_[i]; }
  const std::string& getParamName(int i) const { return paramNames_[i]; }

  /** returns the value of the parameter values as expression */
  CS_ExpressionBasis* getParamValue(int i) { return parameValues_[i]; }
  const CS_ExpressionBasis* getParamValue(int i) const { return parameValues_[i]; }

  /** number of ports */
  int getNrPorts() const { return (int) portConnects_.size();}

  /** returns the connected ports as expression */
  CS_ExpressionBasis* getPort(int i) { return portConnects_[i]; }
  const CS_ExpressionBasis* getPort(int i) const { return portConnects_[i]; }

  /** for the visitor */
  virtual void Accept(CS_CompilerVisitorBasis &visitor) { visitor.Visit((CS_ModuleInstantiation&)(*this)); }

  /** returns the module */
  const CS_ModuleDeclaration* getModule() const { return module_;}

  /** line nr in the Verilog file*/
  int getStartLine() const { return startLine_; }

  /** column number in the Verilog file */
  int getStartColumn() const { return startColumn_; }

private:

  /** type of the instance*/
  ModuleInstanceType instType_;

  /** name of the instance */
  std::string instName_;

  /** module name (for X calls) */
  std::string moduleName_;

  /** the names of the parameters*/
  std::vector<std::string> paramNames_;

  /** the values of the instantiated parameters */
  std::vector<CS_ExpressionBasis*> parameValues_;

  /** ports to which is connected */
  std::vector<CS_ExpressionBasis*> portConnects_;

  /** for error handling */
  const CS_ModuleDeclaration* module_;
  int startLine_;
  int startColumn_;

};

#endif /* CS_MODULEINSTANTIATION_HPP_ */
