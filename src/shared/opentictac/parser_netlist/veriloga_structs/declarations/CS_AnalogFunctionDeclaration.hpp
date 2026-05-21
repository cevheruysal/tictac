/*
 * CS_AnalogFunctionDeclaration.hpp
 *
 *  Created on: Dec 14, 2012
 *      Author: benk
 */

#ifndef CS_ANALOGFUNCTIONDECLARATION_HPP_
#define CS_ANALOGFUNCTIONDECLARATION_HPP_

#include "CS_CompilerTreeNode.hpp"
#include "CS_VariableDeclaration.hpp"
#include "CS_AnalogStatement.hpp"
#include "CS_AnalogSeqBlock.hpp"
#include "CS_ModuleDeclaration.hpp"

#include "verilog_comp.hpp"


/** class to model the functions defined by the user,
 * such declaration according to LRM can be made only within one module
 *
 * The idea to use functions is the following:
 *   - the input variables will the replaced by the expressions that are
 *   in the call instance of the function
 *   - each local variable in the function will be added
 *   as a local variable in the module, where it is called (to trace the derivatives)
 *   -
 *
 * see analog_function_declaration in LRM */
class CS_AnalogFunctionDeclaration : public CS_CompilerTreeNode, public CS_ErrorInfo
{
public:

  /** Ctor */
  CS_AnalogFunctionDeclaration(
      std::string functionName,
      VarType funcType,
      CS_VariableDeclaration *functionVariable,
      const CS_ModuleDeclaration* module,
      int startLine, int startColumn);

  /** empty Dtor */
  virtual ~CS_AnalogFunctionDeclaration() {;}

  /** returns the module */
  virtual const CS_ModuleDeclaration* getModule() const { return module_; }

  /** line nr in the Verilog file*/
  virtual int getStartLine() const { return startLine_;}

  /** column number in the Verilog file */
  virtual int getStartColumn() const { return startColumn_;}

  /** method for the tree traversal */
  virtual void Accept(CS_CompilerVisitorBasis &visitor) { visitor.Visit( (CS_AnalogFunctionDeclaration &)(*this)); }

  /** number of arguments */
  int getNrAgrs() const { return nrInputArgs_; }

  /** adds one argument to the function  */
  void addArgumentVariable(CS_VariableDeclaration* arg,
      FunctionArgumentVariableType argType, VarType declType)
  {
    argVariables_.push_back(arg);
    argVariableTypes_.push_back(argType);
    argVariableDeclTypes_.push_back(declType);
    nrInputArgs_++;
  }

  /** returns the argument declaration of one argument */
  CS_VariableDeclaration* getArgumentDeclaration(int i) { return argVariables_[i]; }

  /** returns the type (IN:1,OUT-1,INOUT:0)*/
  FunctionArgumentVariableType getArgumentType(int i) const { return argVariableTypes_[i]; }

  /** return INTEGER or REAL */
  VarType getArgumentDeclrType(int i) const { return argVariableDeclTypes_[i]; }

  /** looks for a local variable within this function */
  int getArgument(std::string &name)
  {
    int ret = -1;
    for (int i = 0 ; i < nrInputArgs_; i++)
      if (name == argVariables_[i]->getName()) ret = i;
    return ret;
  }


  /** number of arguments */
  int gerNrLocalVars() const { return nrLocalVars_; }

  /** adds one argument to the function  */
  void addLocalVariable(CS_VariableDeclaration* arg)
  {
    localVarDeclrs_.push_back(arg);
    nrLocalVars_++;
  }

  /** looks for a local variable within this function */
  int getLocalVar(std::string &name)
  {
    int ret = -1;
    for (int i = 0 ; i < nrLocalVars_; i++)
      if (name == localVarDeclrs_[i]->getName()) ret = i;
    return ret;
  }

  /** returns the declaration of one local variable */
  CS_VariableDeclaration* getLocalDeclr(int i) { return localVarDeclrs_[i];}

  /** returns the type of the function (it is either INTEGER or REAL)*/
  VarType getFunctionType() const { return funcType_; }

  /** sets the type of the function  (it is either INTEGER or REAL)*/
  void setFunctionType(VarType v) { funcType_ = v; }

  /** sets the statements of the function */
  void addStatement(CS_AnalogStatement *statement)
  {
    instructions_->addInstruction(statement);
  }

  /** returns the instruction sequence of the module */
  const CS_AnalogStatement* getInstrSequence() const { return (const CS_AnalogStatement*)instructions_; }
  CS_AnalogStatement* getInstrSequence() { return (CS_AnalogStatement*)instructions_; }

  /** adds one instantiation of an argument */
  void addArgumentInstance(CS_FunctionArgument *argInst) {
    arguments_.push_back(argInst);
    nrArgsInstance_++;
  }

  /** returns the number of instantiations */
  int getNrArgInstance() const { return nrArgsInstance_; }

  /** returns the vector of instantiations */
  std::vector<CS_FunctionArgument*>& getArgInstantiations() { return arguments_; }
  const std::vector<CS_FunctionArgument*>& getArgInstantiations() const { return arguments_; }

  /** returns the declared name of the function */
  const std::string& getFunctionName() const { return functionName_; }

  /** add the prefix to the local variables, which are also stored in the module as local variables */
  static std::string addPrefixForLocalVars(CS_AnalogFunctionDeclaration* func, std::string name);

  /** returns the variable that represents the scalar value of the function*/
  CS_VariableDeclaration* getFunctionVar() { return functionVariable_; }
  const CS_VariableDeclaration* getFunctionVar() const { return functionVariable_; }

  /** sets the Variable Function */
  void setFunctionVar(CS_VariableDeclaration* newVar) { functionVariable_ = newVar; }

private :

  /** the module where this declaration is located */
  const CS_ModuleDeclaration* module_;

  /** start line and column of the declaration */
  int startLine_;
  int startColumn_;

  /** the name of the function (case sensitive) */
  std::string functionName_;

  /** the declarations of the input REAL */
  std::vector<CS_VariableDeclaration*> localVarDeclrs_;

  /** number of local variables */
  int nrLocalVars_;

  /** the declarations of the input REAL */
  std::vector<CS_VariableDeclaration*> argVariables_;

  /** types of the arguments IN, INOUT */
  std::vector<FunctionArgumentVariableType> argVariableTypes_;
  /** types of the arguments IN, INOUT */
  std::vector<VarType> argVariableDeclTypes_;

  /** number of arguments */
  int nrInputArgs_;

  /** the vector of all argument instantiations within the body of the function */
  std::vector<CS_FunctionArgument*> arguments_;

  /** number of input arguments */
  int nrArgsInstance_;

  /** type of the function INTEGER, or REAL*/
  VarType funcType_;

  /** the statements in the function */
  CS_AnalogSeqBlock *instructions_;

  /** the variable that contains the function's value*/
  CS_VariableDeclaration *functionVariable_;
};

#endif /* CS_ANALOGFUNCTIONDECLARATION_HPP_ */
