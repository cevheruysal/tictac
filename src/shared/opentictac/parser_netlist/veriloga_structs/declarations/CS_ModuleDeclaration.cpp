/*
 * CS_ModuleDeclaration.cpp
 *
 *  Created on: Nov 27, 2012
 *      Author: benk
 */

#include "CS_ModuleDeclaration.hpp"
#include "CS_Parameter.hpp"
#include "CS_ParameterDeclaration.hpp"
#include "CS_Port.hpp"
#include "CS_VariableDeclaration.hpp"
#include "CS_Range.hpp"
#include "CS_AnalogSeqBlock.hpp"
#include "CS_AnalogStatement.hpp"
#include "CS_BranchDeclaration.hpp"
#include "CS_ModuleInstantiation.hpp"
#include "CS_AnalogFunctionDeclaration.hpp"
#include "CS_AnalogInitialBlock.hpp"
#include "CS_RandomFunction.hpp"
#include "CS_NatureAndDisciplineCollection.hpp"

#include <algorithm>

CS_ModuleDeclaration::CS_ModuleDeclaration(std::string modName, std::string fileName,
    int startL, int endL): UT_VerboseClass("ModuleDeclaration")
    , ports_()
    , nrPorts_(0)
    , nrScalarExternalPorts_(0)
    , parameters_()
    , nrParams_(0)
    , variables_()
    , nrVars_(0)
    , moduleInsts_()
    , nrModInst_(0)
    , branchDecrs_()
    , nrBranchDeclrs_(0)
    , userFunctionDeclars_()
    , nrUserFunctionDeclrs_(0)
    , nrRandomIntFunctionCalls_(0)
    , nrRandomRealFunctionCalls_(0)
    , nrRandSeeds_(0)
{
  // store the strings
  moduleName_ = modName;
  moduleFile_ = fileName;
  // store the line numbers
  startLine_ = startL;
  endLine_ = endL;
  //setVerb(6);

  instructions_ = NULL;
  analogInitInstructions_ = NULL;
}

CS_ModuleDeclaration::~CS_ModuleDeclaration()
{
  if (instructions_ != NULL)
    delete instructions_;
  // nothing to do yet
}

void CS_ModuleDeclaration::addPort( const std::string &name,
    int startLine, int startColumn,
    CS_Range* range, int &index, PortType pt, bool isExternal,
    std::string discipline)
{

  //VERILOG_PRINT_L4( verb() , " looking for port ");
  CS_Port *ret = 0;
  MY_SMPO<const CS_DisciplineDeclaration> disciplineObj;
  if ( discipline != "") {
      VERILOG_ERROR_COND_STOP( !CS_NatureAndDisciplineCollection::getDiscAndNatures()->hasDiscipline(discipline),
          "Discipline " << discipline << " not found!", 0);
      disciplineObj = CS_NatureAndDisciplineCollection::getDiscAndNatures()->getDiscipline(discipline);
      VERILOG_PRINT_L6( verb() , "disciplineObj = " << discipline );
      VERILOG_PRINT_L6( verb() , "disciplineObj = " << disciplineObj->getDisciplineName() );
  }

  index = -1;
  // first look for the port if it is existent
  for (int i = 0; i < nrPorts_ ; i++){
      if (name == ports_[i]->getName() ){
          ret = ports_[i];
          index = i;
      }
  }
  // if port is already in the module, then update the port's informations
  if (ret != 0) {
      VERILOG_PRINT_L4( verb() , "Refound Port in Module:" << moduleName_ << " port:" << name << " pt:" << pt);
      // test if this already had one range
      if ((ret->getRangeObj()) && !(ret->getRangeObj()->isEqual(range)))
        VERILOG_WARNING( "Range definition mismatch, considering the last one", ret);
      ret->setRange(range);
      // only if the port has not been declared previously as ground, then it should stay ground
      if (ret->getPortType() != PORT_GROUND && ret->getPortType() != PORT_INOUT_EXTERNAL) {
          VERILOG_PRINT_L4( verb() , "Set port type pt:" << pt);
          ret->setPortType(pt);
      }
      // disciplines can be reassigned, if there was none
      if (!ret->hasValidDiscipline())
        ret->setDiscipline(disciplineObj);
      else
        VERILOG_ERROR_COND_STOP( discipline != "",
            "Discipline " << discipline << " can not be reassigned to port: " << ret->getName(), ret);
  }
  else {

      // if not found then add a new port
      VERILOG_PRINT_L4( verb() , "Add Port to Module:" << moduleName_ << " port:" << name << " pt:" << pt);
      ret = new CS_Port(this, startLine, startColumn, name, nrPorts_, disciplineObj, range, pt, isExternal);

      // test if there is a match lower/upper, if YES then throw an error
      std::string nameLower = name;
      std::transform( nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
      for (int i = 0; i < nrPorts_ ; i++){
          std::string lName = ports_[i]->getName() ;
          std::transform( lName.begin(), lName.end(), lName.begin(), ::tolower);
          // if we have a lowecase match
          if ( lName == nameLower)
            VERILOG_ERROR_STOP("new port \"" << ports_[i]->getName()
                << "\" differs only in upper case with other port name \""
                << name << "\" (This is will lead to error in SPICE netlist)", ret );
      }

      ports_.push_back(ret);
      nrPorts_++;
      // the ground port does not count as port for external
      if ((pt != PORT_GROUND) && isExternal)
        nrScalarExternalPorts_ = nrScalarExternalPorts_ + ret->getSize();
  }
}

const CS_Port* CS_ModuleDeclaration::getPort(const std::string &name, int &index) const {
  // look for a port with the specified name
  for (int i = 0; i < nrPorts_ ; i++){
      if (name == ports_[i]->getName() ){
          index = i;
          return ports_[i];
      }
  }
  index = -1;
  return 0;
}


void CS_ModuleDeclaration::addParameter( CS_ParameterDeclaration* paramDeclr ){
  const std::string &name = paramDeclr->getName();
  CS_ParameterDeclaration *ret = 0;
  // look if this parameter exists already
  for (int i = 0; i < nrParams_ ; i++){
      // test if the names are equal
      if (name == parameters_[i]->getName()){
          ret = parameters_[i];
      }
  }
  // if port is already in the module, then update the port's informations
  if (ret != 0 && !UT_GeneralConfiguration::isCMGenerate() ){
      VERILOG_WARNING( "Second declaration of parameter '" << name << "' in module '"
          << moduleName_ << "'", paramDeclr->getParam());
      //VERILOG_PRINT_L4( verb() , "Refound Paramter in Module:" << moduleName_ << " port:" << name );
      // todo: delete the pointer, since it will not be used ?
  }
  else{
      // test if there is a match lower/upper, if YES then throw an error
      std::string nameLower = name;
      std::transform( nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
      // do this only for non-local parameters
      for (int i = 0; (i < nrParams_) && (!paramDeclr->getParam()->isLocalParam()); i++){
          std::string lName = parameters_[i]->getName();
          std::transform( lName.begin(), lName.end(), lName.begin(), ::tolower);
          // if we have a lowecase match
          if ( lName == nameLower && !UT_GeneralConfiguration::isCMGenerate())
            VERILOG_ERROR_STOP("new parameter \"" << parameters_[i]->getName()
                << "\" differs only in upper case with other parameter name \""
                << name << "\" (This is will lead to error in SPICE netlist)", paramDeclr->getParam() );
      }

      // add the parameter to the list
      parameters_.push_back( paramDeclr );
      nrParams_++;
  }
}


const CS_Parameter* CS_ModuleDeclaration::getParam(
    const std::string &name, int &index, bool &local) const
{
  // look for the Parameter with the specified name
  for (int i = 0; i < nrParams_ ; i++){
      if ( name == parameters_[i]->getName() ) {
          index = i;
          local = parameters_[i]->getParam()->isLocalParam();
          return parameters_[i]->getParam();
      }
  }
  index = -1;
  return 0;
}

CS_ParameterDeclaration* CS_ModuleDeclaration::getParamDeclr(
    const std::string &name, int &index, bool onlyDeviceParam)
{
  // look for the Parameter with the specified name
  for (int i = 0; i < nrParams_ ; i++){
      if ( name == parameters_[i]->getName()
          // in CM case there could be multiple "M" parameters ...
          // and we want only the device param
          && (!onlyDeviceParam || parameters_[i]->getParam()->isDeviceParameter() )
          ) {
          index = i;
          return parameters_[i];
      }
  }
  index = -1;
  return 0;
}


void CS_ModuleDeclaration::addVariable(CS_VariableDeclaration* var)
{
  const std::string &name = var->getName();
  int index_L = -1;
  CS_VariableDeclaration *ret = 0;
  VERILOG_PRINT_L4( verb() , " Add Variable " << var->getName() <<" to module "
      << this->moduleName_ << " type:" << var->getType() << " nr. Var=" << nrVars_ );
  // look if this variable exists already
  for (int i = 0; i < nrVars_ ; i++){
      // test if the names are equal
      if (name == variables_[i]->getName()){
          ret = variables_[i];
          index_L = i;
      }
  }
  // if variable is already in the module, then update the informations
  if (ret != 0){
      VERILOG_PRINT_L4( verb() , "Refound Variable in Module:" << moduleName_ << " variable:" << name );
      variables_[index_L] = ret;
  }
  else{
      // add the variable
      variables_.push_back(var);
      nrVars_++;
  }
}

const CS_Parameter* CS_ModuleDeclaration::getParam(int i) const
{
  return parameters_[i]->getParam();
}

CS_Parameter* CS_ModuleDeclaration::getParam(int i)
{
  return parameters_[i]->getParam();
}


const CS_VariableDeclaration* CS_ModuleDeclaration::getVar( const std::string &name, int &index) const
{
  // look for the Variable with the specified name
  for (int i = 0; i < nrVars_ ; i++){
      if (name == variables_[i]->getName() ){
          index = i;
          return variables_[i];
      }
  }
  index = -1;
  return 0;
}

/* adds one branch Declaration, if it does not exists */
void CS_ModuleDeclaration::addBranchDeclar(CS_BranchDeclaration* branchDeclr, int &index)
{
  CS_BranchDeclaration* branchDeclrTmp = 0;
  index = -1;
  for (int i = 0; i < nrBranchDeclrs_ ; i++){
      if (branchDeclr->getName() == branchDecrs_[i]->getName() ){
        branchDeclrTmp = branchDecrs_[i];
        index = i;
      }
  }
  // if this branch has been already declared in this module
  if (branchDeclrTmp != 0) {
      VERILOG_WARNING(" Branch already declared '" << branchDeclrTmp->getName()
          << "' ignoring new declaration ", branchDeclr);
  }
  else {
      // add the branch declaration
      branchDecrs_.push_back( branchDeclr );
      nrBranchDeclrs_++;
  }
}

/* returns the branch declaration of a given name, if it does not exists then null  */
const CS_BranchDeclaration* CS_ModuleDeclaration::getBranchDeclr(const std::string &name,
    int &index) const
{
  CS_BranchDeclaration* branchDeclrTmp = 0;
  index = -1;
  // look for a branch declaration with the given name
  for (int i = 0; i < nrBranchDeclrs_ ; i++){
      if (name == branchDecrs_[i]->getName() ){
        branchDeclrTmp = branchDecrs_[i];
        index = i;
      }
  }
  // return the found (or not found = NULL) declaration
  return branchDeclrTmp;
}

CS_Identifier* CS_ModuleDeclaration::getValueObj( std::string &name )
{
  // search within parameters
  //VERILOG_PRINT_L3( verb(), "VerilogModule::getValueObj P:" << nrParams_ );
  for (int i = 0; i < nrParams_ ; i++){
      if (name == parameters_[i]->getName() )
          return parameters_[i]->getParam();
  }
  // search within variables
  for (int i = 0; i < nrVars_ ; i++){
      if (name == variables_[i]->getName() )
          return variables_[i];
  }
  // search within ports
  for (int i = 0; i < nrPorts_ ; i++){
      if (name == ports_[i]->getName() )
          return ports_[i];
  }
  return 0;
}


void CS_ModuleDeclaration::addInstr(CS_AnalogStatement *instruction)
{
  if (instructions_ == NULL)
    instructions_ = new CS_AnalogSeqBlock(this, startLine_, 1);
  // add one instruction to the instruction sequence
  instructions_->addInstruction(instruction);
}


/* adds a new Instance to this module */
void CS_ModuleDeclaration::addInstantiation( CS_ModuleInstantiation* newInst )
{
  // first test if this instance name already exists
  for (int i = 0 ; i < this->nrModInst_; i++)
    {
      VERILOG_ERROR_COND_STOP( moduleInsts_[i]->getInstName() == newInst->getInstName() ,
          "Instance with name '" << newInst->getInstName() << "' already exists in this module", newInst);
    }
  // add the instance to the array of instances
  moduleInsts_.push_back(newInst);
  nrModInst_++;
}


int CS_ModuleDeclaration::hasFunction(std::string funcName)
{
  int ret = -1;
  for (int i = 0; i < nrUserFunctionDeclrs_; i++)
    {
      if (userFunctionDeclars_[i]->getFunctionName() == funcName)
        ret = i;
    }
  return ret;
}


int CS_ModuleDeclaration::getNrInitInstrSequence() const
{
  if (analogInitInstructions_ != NULL)
    return analogInitInstructions_->getNrInitInstrSequence();
  else
    return 0;
}


void CS_ModuleDeclaration::addInitInstr(CS_AnalogStatement *instruction)
{
  if (analogInitInstructions_ == NULL)
    analogInitInstructions_ = new CS_AnalogInitialBlock(this,
        instruction->getStartLine(), instruction->getStartColumn() );
  // add one instruction to the initial analog part
  analogInitInstructions_->addInitInstr(instruction);
}


void CS_ModuleDeclaration::addIntRandFunction(CS_RandomFunction *randFunc) {
  randomFunctionCalls_.push_back(randFunc);
  nrRandomIntFunctionCalls_++;
}


void CS_ModuleDeclaration::addRealRandFunction(CS_RandomFunction *randFunc) {
  randomFunctionCalls_.push_back(randFunc);
  nrRandomRealFunctionCalls_++;
}
