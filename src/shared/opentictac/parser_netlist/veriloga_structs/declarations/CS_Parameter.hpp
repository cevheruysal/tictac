/*
 * CS_Parameter.hpp
 *
 *  Created on: Nov 29, 2012
 *      Author: benk
 */

#ifndef CS_PARAMETER_HPP_
#define CS_PARAMETER_HPP_

#include "verilog_comp.hpp"
#include "CS_Identifier.hpp"
#include "CS_ErrorInfo.hpp"
#include "CS_Range.hpp"

class CS_ModuleDeclaration;

/** the class models one parameter including its type, but without constraints or default values
 * See parameter_identifier and parameter_type in LRM */
class CS_Parameter : public CS_Identifier, public CS_ErrorInfo
{
public:

  /** Ctor for the parameter class. Inputs are the name of the
   * parameter (as it is in the Verilog file), and the statically assigned
   * parameter index within the module */
  CS_Parameter(std::string name, const CS_ModuleDeclaration* module,
      int startLine,  int startColumn,
      int paramIndex, VarType type = REAL,
      bool isLocal = false, CS_Range* range = NULL,
      std::string aliasName = "");

  /** empty Dtor */
  virtual ~CS_Parameter() {;}

  /** returns the name of the parameter */
  virtual const std::string& getName() const { return name_; }

  /** returns the index of the parameter within a module */
  int getIndex() const { return index_; }

  /** return the type of the parameter (INT or REAL)*/
  virtual VarType getType() const { return type_; }

  /** the context (simple implementation) is obvious */
  virtual int getNodeContext() const { return (int)PARAMETER; }

  /** return the C name of the parameter */
  virtual std::string getGenerate([[maybe_unused]] const VarType resType) const {
    return std::string( "params." + name_);
  }

  /** the dimension of the value*/
  virtual int getSize() const { return size_; }

  /** returns the offset of an array that is specified in the declaration, */
  virtual int getOffset() const { return offset_; };

  /** this function shows if (in the case of arrays) the elements are numbered
   * in reverse order */
  virtual bool isInReversedOrder() const { return isReverse_; }

  /** we have one class */
  virtual VariableClass getClass() const { return PARAMETER; }

  /** returns the module */
  const CS_ModuleDeclaration* getModule() const { return module_;}

  /** line nr in the Verilog file*/
  int getStartLine() const { return startLine_; }

  /** column number in the Verilog file */
  int getStartColumn() const { return startColumn_; }

  /** true if this is an array */
  virtual bool isArray() const { return (range_ != NULL); };

  /** returns true if parameter local, false otherwise */
  bool isLocalParam() const { return isLocalParameter_; }

  /** returns true if this is an alias parameter*/
  bool isAliasParam() const { return (aliasName_.size() > 0);}

  /** if this is an alias parameter then this return the alias name, if this is
   * a "normal" parameter than this is empty */
  const std::string& getAliasName() const { return aliasName_; }

  /** only for CM returns true if this is a device parameter and no model */
  bool isDeviceParameter() const { return isDevicePrameter_; }
  /** sets the flag to signal if (only for CM) this is a device parameter and no
   * MODEL parameter*/
  void setIsDeviceParameter(bool a) { isDevicePrameter_ = a;}

  /** when we have variable array length then this MUST be defined through
   * a localparam */
  CS_Parameter* getLocalParameterForVariableArraySize() { return localParamForVariableArraySize_;}
  const CS_Parameter* getLocalParameterForVariableArraySize() const { return localParamForVariableArraySize_;}

  /** returns true when this is a variable length parameter array */
  bool isVariableArrayLength() const {
    return (!isLocalParameter_ &&  localParamForVariableArraySize_ != 0);
  }

private:

  /** the Verilog module containing */
  const CS_ModuleDeclaration* module_;

  /** start line of the declaration */
  const int startLine_;

  /** start column */
  const int startColumn_;

  /** the name of the parameter*/
  const std::string name_;

  /** the index within parameters */
  const int index_;

  /** the type of the parameter (INTEGER OR REAL), local parameters can be strings*/
  VarType type_;

  /** flag to show if an instance is a local parameter or not */
  bool isLocalParameter_;

  /** if it is an array than this is not null*/
  CS_Range* range_;

  /** local parameters can be arrays */
  int size_;

  /** offset in case of array local parameters */
  int offset_;

  /** for local arrays if they are given in reverse order then this is true */
  bool isReverse_;

  /** if this is an alias param than this string will not be empty */
  std::string aliasName_;

  /** only for CM true if this is a device model parameter and no model parameter*/
  bool isDevicePrameter_;

  /** in case of array external parameters which can have variable sizes
   * we need a localparam which directly defines the length of the array and this
   * local parameter will be directly set externally */
  CS_Parameter *localParamForVariableArraySize_;
};

#endif /* CS_PARAMETER_HPP_ */
