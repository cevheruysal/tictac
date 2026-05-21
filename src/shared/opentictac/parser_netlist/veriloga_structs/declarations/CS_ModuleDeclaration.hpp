/*
 * CS_ModuleDeclaration.hpp
 *
 *  Created on: Nov 27, 2012
 *      Author: benk
 */

#ifndef CS_MODULEDECLARATION_HPP_
#define CS_MODULEDECLARATION_HPP_

#include "verilog_comp.hpp"
#include "CS_AKModuleSpecInfo.hpp"

class CS_Parameter;
class CS_ParameterDeclaration;
class CS_Port;
class CS_VariableDeclaration;
class CS_Identifier;
class CS_Range;
class CS_AnalogSeqBlock;
class CS_AnalogStatement;
class CS_BranchDeclaration;
class CS_AKModuleSpecInfo;
class CS_ModuleInstantiation;
class CS_AnalogFunctionDeclaration;
class CS_AnalogInitialBlock;
class CS_RandomFunction;

/** class contains all the informations within one Verilog module.
 * These informations are potentially: ports, parameters, instructions
 * see module_declaration in the LRM */
class CS_ModuleDeclaration : public UT_VerboseClass
{
public:

  /** Ctor creates an empty module */
  CS_ModuleDeclaration(std::string modName, std::string fileName,
      int startL, int endL);

  /** Dtor deletes memory*/
  virtual ~CS_ModuleDeclaration();

  /** returns the strut's name in the C generate that contains all the
   * parameters, ports, variables */
  std::string getCStructName() const { return std::string(moduleName_); }

  /** returns the name of the module */
  const std::string& getName() const { return moduleName_; }

  /** returns the file containing the module */
  const std::string& getModuleFile() const { return moduleFile_; }

  /** adds one port to the module, or if a port exists already with the same name than
   * the size and the type of this port will be updated. The variable index is negative
   * if the port already exists in the module and is >= 0 if it is already existing
   * As a last argument we also pass the discipline name */
  void addPort( const std::string &name, int startLine, int startColumn,
      CS_Range* range, int &index,
      PortType pt = PORT_INOUT_EXTERNAL, bool isExternal = false,
      std::string discipline = "");

  /** returns the port of a given name and it also returns the index  */
  const CS_Port* getPort(const std::string &name, int &index) const;

  /** returns the port of a given index */
  const CS_Port* getPort(int i) const { return ports_[i];}
  CS_Port* getPort(int i) { return ports_[i];}

  /** returns the number of declared ports (arrays count as one)*/
  int getNrPort() const { return nrPorts_; }
  /** returns the number of scalar ports (one port array is transformed into scalar arrays)*/
  int getNrScalarExternalPorts() const { return nrScalarExternalPorts_; }

  /** adds one parameter declaration, but it also checks if this parameter already exists*/
  void addParameter(CS_ParameterDeclaration* paramDeclr);

  /** returns the parameter of a given name and it also returns the index
   * @param name name of the parameter
   * @param index the returned index if this param was found
   * @param local if true only local param was found, false otherwise */
  const CS_Parameter* getParam(const std::string &name, int &index, bool &local) const;

  /** returns the parameter at a given index */
  const CS_Parameter* getParam(int i) const;
  CS_Parameter* getParam(int i);

  /** returns the parameter declaration at the given index */
  CS_ParameterDeclaration* getParamDeclr(int i) { return  parameters_[i]; }
  const CS_ParameterDeclaration* getParamDeclr(int i) const { return  parameters_[i]; }
  /** returns the parameter declaration of a given name and it also returns the index
   * @param name name of the parameter decalaration
   * @param index the returned index if this param was found
   * @param onlyDeviceParam true for CM model generation, when we only
   * want device parameters and no model parameters */
  CS_ParameterDeclaration* getParamDeclr(const std::string &name, int &index, bool onlyDeviceParam=false);

  /** returns the number of declared parameters (arrays and alias count one)*/
  int getNrParam() const { return nrParams_; }

  /** adds one variable to the existing list, if it does not exists */
  void addVariable(CS_VariableDeclaration* var);

  /** returns the port of a given name and it also returns the index  */
  const CS_VariableDeclaration* getVar(const std::string &name, int &index) const;

  /** returns the variable at a given index */
  const CS_VariableDeclaration* getVar(int i) const { return variables_[i]; }

  /** returns the variable at a given index */
  CS_VariableDeclaration* getVar(int i) { return variables_[i]; }

  /** returns the number of declared parameters (arrays count as one)*/
  int getNrVars() const { return nrVars_; }

  /** returns the number of spice primitive (or module) instantiation */
  int getNrInsts() const { return nrModInst_; }

  /** returns the instantiation at the index i*/
  CS_ModuleInstantiation* getInstance(int i) { return moduleInsts_[i]; }
  const CS_ModuleInstantiation* getInstance(int i) const { return moduleInsts_[i]; }

  /** adds a new Instance to this module */
  void addInstantiation( CS_ModuleInstantiation* newInst );

  /** adds one branch Declaration, if it does not exists
   * the index, shows if the declaration has been refound in the actual module */
  void addBranchDeclar(CS_BranchDeclaration* branchDeclr, int &index);

  /** returns the branch declaration of a given name, if it does not exists then null  */
  const CS_BranchDeclaration* getBranchDeclr(const std::string &name, int &index) const;

  /** returns the variable at a given index */
  const CS_BranchDeclaration* getBranchDeclr(int i) const { return branchDecrs_[i]; }
  CS_BranchDeclaration* getBranchDeclr(int i) { return branchDecrs_[i]; }

  /** returns the number of declared parameters (arrays count as one)*/
  int getBranchDeclrs() const { return nrBranchDeclrs_; }


  /** returns one "Value" object that can be a parameter, variable, and port
   * that has the specified name. If this does not exist then it returns null.
   * This method is used potentially during parsing */
  CS_Identifier* getValueObj( std::string &name );

  /** returns the instruction sequence of the module */
  const CS_AnalogStatement* getInstrSequence() const { return (const CS_AnalogStatement*)instructions_; }
  CS_AnalogStatement* getInstrSequence() { return (CS_AnalogStatement*)instructions_; }

  /** @returns the analog initial instruction sequence of the module */
  const CS_AnalogStatement* getInitInstrSequence() const
      { return (const CS_AnalogStatement*)analogInitInstructions_; }
  CS_AnalogStatement* getInitInstrSequence() { return (CS_AnalogStatement*)analogInitInstructions_; }
  /** @returns the number of analog initial statement */
  int getNrInitInstrSequence() const;

  /** adds one instruction to the analog instruction sequence of this module
   * @param instruction the new instruction to be added */
  void addInstr(CS_AnalogStatement *instruction);

  /** adds one instruction to the analog initial instruction sequence
   * of this module
   * @param instruction the new instruction to be added */
  void addInitInstr(CS_AnalogStatement *instruction);

  /** returns the function declaration */
  const CS_AnalogFunctionDeclaration* getFunctionDeclr(int i) const { return userFunctionDeclars_[i]; }
  CS_AnalogFunctionDeclaration* getFunctionDeclr(int i) { return userFunctionDeclars_[i]; }

  /** returns the number of function declarations within this modul */
  int getNrFunctionDeclars() const { return nrUserFunctionDeclrs_; }

  /** adds one function declarations */
  void addFunctionDeclaration(CS_AnalogFunctionDeclaration* newFunc)
  {
    userFunctionDeclars_.push_back(newFunc);
    nrUserFunctionDeclrs_++;
  }

  /** adds one integer random function call to this list (this is anyhow already
   * present in the instruction list)*/
  void addIntRandFunction(CS_RandomFunction *randFunc);

  /** adds one real random function call to this list (this is anyhow already
   * present in the instruction list)*/
  void addRealRandFunction(CS_RandomFunction *randFunc);

  /** @returns the number integer random function calls in this module */
  int getNrIntRandFunction() const { return this->nrRandomIntFunctionCalls_; }

  /** @returns the number real random function calls in this module */
  int getNrRealRandFunction() const { return this->nrRandomRealFunctionCalls_; }

  /**
   * @param i index of the different seeds (random generators)
   * @returns true if the given seed by the index needs an internal variable, false otherwise */
  bool needsSeedInternalVar(int i) const { return needsSeedInternalSeedvar_[i]; }

  /** signals that we need one additional seed
   * @param needsInernalVar flag to show if this needs internal variable or not */
  void addNewSeed(bool needsInernalVar) { nrRandSeeds_++; needsSeedInternalSeedvar_.push_back(needsInernalVar);}

  /** @returns the number of different required seeds in this module */
  int getNrRandomSeeds() const { return this->nrRandSeeds_; }

  /** returnt the pointer to the function call */
  const CS_RandomFunction* getRandomFunctionCall(int i) const { return randomFunctionCalls_[i]; }
  CS_RandomFunction* getRandomFunctionCall(int i) { return randomFunctionCalls_[i]; }

  /** returns the index with the name if this exist, otherwise return -1*/
  int hasFunction(std::string funcName);

  /** method returns the object containing AK specific informations */
  CS_AKModuleSpecInfo& getAkInfo() { return akInfo_; }
  const CS_AKModuleSpecInfo& getAkInfo() const { return akInfo_; }

  /** returns the list of the temporary variables which are required for
   * expression optimization*/
  const std::map< std::string, int >& getOptimVarList() const {
    return tmpVariableList_;
  }
  /* sets the list of the temporary variables which are required
   * for expression optimization */
  void setOptimVarList(const std::map< std::string, int > &vars ) {
    tmpVariableList_ = vars;
  }

private:

  /** name of the module */
  std::string moduleName_;

  /** the file of the module where it was parsed */
  std::string moduleFile_;

  /** the starting line in the file where the module was declared */
  int startLine_;
  int endLine_;

  /** the ports of the module */
  std::vector<CS_Port*> ports_;
  int nrPorts_;

  /** number of scalar external ports (no ground and internal ports)*/
  int nrScalarExternalPorts_;

  /** the parameters of the module (local and non-local)*/
  std::vector<CS_ParameterDeclaration*> parameters_;
  int nrParams_;

  /** all the variables within the module */
  std::vector<CS_VariableDeclaration*> variables_;
  int nrVars_;

  /** vector containing all the instantiation within this module of
   * spice primitives or other Verilog modules (later actually is not allowed ) */
  std::vector<CS_ModuleInstantiation*> moduleInsts_;
  int nrModInst_;

  /** object containing the analog instruction of this module in the
   * analog block*/
  CS_AnalogSeqBlock *instructions_;

  /** object containing the analog instruction of this module in the
   * analog initial block */
  CS_AnalogInitialBlock *analogInitInstructions_;

  /** a module might contain branch declarations that are later referred to. */
  std::vector<CS_BranchDeclaration*> branchDecrs_;
  int nrBranchDeclrs_;

  /** a module might contain several function declarations */
  std::vector<CS_AnalogFunctionDeclaration*> userFunctionDeclars_;
  int nrUserFunctionDeclrs_;

  /** vector containint all the calls of random functions */
  std::vector<CS_RandomFunction*> randomFunctionCalls_;
  /** the total number of integer random function calls */
  int nrRandomIntFunctionCalls_;
  /** the total number of real random function calls */
  int nrRandomRealFunctionCalls_;
  /** in this variable we try to measure the number of different seeds in the module */
  int nrRandSeeds_;
  /** shows if the random engine needs an internal variable or not */
  std::vector<bool> needsSeedInternalSeedvar_;

  /** if expression optimization is turned on then this map contains the
   * list of temporary variables which are required for the evaluation routine */
  std::map< std::string, int > tmpVariableList_;

  /** titan specific information within a module */
  CS_AKModuleSpecInfo akInfo_;

};

#endif /* CS_MODULEDECLARATION_HPP_ */
