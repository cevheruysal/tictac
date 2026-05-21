/*
 * CS_Port.hpp
 *
 *  Created on: Nov 30, 2012
 *      Author: benk
 */

#ifndef CS_PORT_HPP_
#define CS_PORT_HPP_

#include "verilog_comp.hpp"
#include "CS_Identifier.hpp"
#include "CS_ModuleDeclaration.hpp"
#include "CS_Range.hpp"
#include "CS_CompilerTreeNode.hpp"
#include "CS_ErrorInfo.hpp"
#include "CS_DisciplineDeclaration.hpp"


/** class denoting the in/out ports of a Verilog-A module.
 * We distinguish only between inout ports and ground.
 * All ports are by default inout, also when they are defined otherwise
 *  inout_declaration and port_identifier in LRM  this class represents the combination of both */
class CS_Port : public CS_Identifier, public CS_CompilerTreeNode, public CS_ErrorInfo
{
public:

  /** Ctor for a port object
   * @param portIndex [IN] index in the scalar
   * @param portIndex [IN] the range
   * @param ty [IN] type of the type
   * @param isExternal [IN] true if the port is external to the module, false otherwise
   * @param discipline [IN] name of the discipline if this known*/
  CS_Port(const CS_ModuleDeclaration* module, int startLine, int startColumn,
      std::string name,
      int portIndex,
      MY_SMPO<const CS_DisciplineDeclaration> &discipline,
      CS_Range* range = 0, PortType ty = PORT_INOUT_EXTERNAL,
      bool isExternal = true);

  virtual ~CS_Port() {;}

  /** returns the type of the port */
  virtual VarType getType() const { return (range_ == 0) ? REAL : REAL_ARRAY; }

  /** sets the range object */
  void setRange(CS_Range* range);

  /** setting the discipline of the port */
  void setDiscipline(MY_SMPO<const CS_DisciplineDeclaration> &discipline) {
    discipline_ = discipline;
    /*std::cout << " setDiscipline " << getName() << " f=" << hasValidDiscipline() << "\n";*/
  }

  /** returns the discipline of the port */
  const MY_SMPO<const CS_DisciplineDeclaration>& getDiscipline() const { return  discipline_; }
  /** returns true if this port has a valid discipline */
  bool hasValidDiscipline() const { return (discipline_.get() != 0); }

  /** returns the name of the port */
  virtual const std::string& getName() const { return name_; }

  /** returns the string of the actual entity (e.g., the corresponding C variable of a Verilog parameter)*/
  virtual std::string getGenerate([[maybe_unused]] const VarType resType) const {
    return std::string("port."+name_);
  }

  /** returns the class of the value (Port,Variable or Parameter)*/
  virtual VariableClass getClass() const { return PORT; }

  /** the context (simple implementation) is obvious */
  virtual int getNodeContext() const { return (int)PORT; }

  /** returns the type of the port*/
  PortType getPortType() const { return portType_; }

  /** sets the type of the port*/
  void setPortType(PortType t) {   portType_ = t;  }

  /** returns the size of the variable */
  virtual int getSize() const { return size_; }

  /** returns the offset of an array that is specified in the declaration, */
  virtual int getOffset() const { return offset_; };

  /** returns the index of the port in the module.
   * This is the scalar index and this also represents the first unknowns in the
   * genrated AK C code */
  int getPortIndex() const { return portIndex_; }

  /** returns the index range object, if the port object is scalar then this is null */
  CS_Range* getRangeObj() { return range_; }
  const CS_Range* getRangeObj() const { return range_; }

  /** method for the tree traversal */
  virtual void Accept(CS_CompilerVisitorBasis &visitor) { visitor.Visit( (CS_Port &)(*this)); }

  /** returns the flag if a port is external */
  bool isPortExternal() const { return isExternal_; }

  /** true if this is an array */
  virtual bool isArray() const { return isArray_; };

  /** returns the module */
  const CS_ModuleDeclaration* getModule() const { return module_;}

  /** line nr in the Verilog file*/
  int getStartLine() const { return startLine_; }

  /** column number in the Verilog file */
  int getStartColumn() const { return startColumn_; }

  /** this function shows if the pins are numbered in reverse order */
  virtual bool isInReversedOrder() const { return isReverseNumbered_; }

private:

  /** start line of the expression */
  const int startLine_;

  /** start column */
  const int startColumn_;

  /** the module containing the parameter */
  const CS_ModuleDeclaration* module_;

  /** name of the port */
  std::string name_;

  /** the index within the ports */
  int portIndex_;

  /** ports can be arrays */
  CS_Range* range_;

  /** the type of the port */
  PortType portType_;

  /** the size of the port (scalar or array) */
  int size_;

  /** used only for array of ports, and is the offset */
  int offset_;

  /** flag to show if this is numbered inversely */
  bool isReverseNumbered_;

  /** this flag is set if the port is an external one, false if it is internal */
  bool isExternal_;

  /** flag to show if this port has been declared as an array */
  bool isArray_;

  /** pointer to a discipline */
  MY_SMPO<const CS_DisciplineDeclaration> discipline_;

};

#endif /* CS_PORT_HPP_ */
