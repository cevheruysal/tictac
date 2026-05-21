/*
 * IP_ExpansionStack.cpp
 *
 *  Created on: Feb 28, 2015
 *      Author: benk
 */

#include "IP_ExpansionStack.hpp"
#include "IP_ExpressionParser.hpp"

// =========================================================
// ======================== ExpansionStack =================
// =========================================================

/// Ctor
ExpansionStack::ExpansionStack() : UT_SIMVerboseClass("ExpansionStack")
{
  // the default names for ground nodes
  listOfGndNames_.insert( std::pair<std::string,MYINT>("0",0));
  listOfGndNames_.insert( std::pair<std::string,MYINT>("gnd",0));
  this->setVerb(0);
}

void ExpansionStack::addGlobalPinn(std::string globalPinName, MYINT ind)
{
  globalPinns_.insert( std::pair<std::string,MYINT>(globalPinName,ind));
}

bool ExpansionStack::foundParameter(std::string &paramName, bool &hasInitValue,
    TICTAC_SPTR<CS_ExpressionBasis> &internalFoundParamValue,
    TICTAC_SPTR<CS_Parameter>       &internalFoundParamRef,
    IP_Param*                             &internalFoundParam,
    bool                                  &internalFoundParamIsLocal,
    bool ignoreActualLevel ) const {
  MYINT startInd = (MYINT)expansionStack_.size() - 1, i;
  hasInitValue = false;
  // consider if the actual level should be ignored
  if (ignoreActualLevel)
    startInd--;
  // we go backwards in the stack (we want to pupport only this variant !!!)
  for (i = startInd; i >= 0; i--)
    {
      SIM_PRINT_L5(verb(), "ExpansionStack::foundParameter stack ind=" << i << " paramName=" << paramName );
      // ============ LOCAL PARAMS ===========
      // look first in the local params then in the X-call parameters
      auto initParamIt = expansionStack_[i].localParams_.find(paramName);
      if (initParamIt != expansionStack_[i].localParams_.end())
        { // local param found
          SIM_PRINT_L5(verb(), "ExpansionStack::foundParameter local param found");
          auto localParamValIt = expansionStack_[i].localParamValues_.find(paramName);
          auto localParamRefIt = expansionStack_[i].localParamsRefs_.find(paramName);
          internalFoundParam = initParamIt->second;
          internalFoundParamRef = localParamRefIt->second;
          internalFoundParamIsLocal = true;
          if (localParamValIt != expansionStack_[i].localParamValues_.end() )
            {
              internalFoundParamValue = localParamValIt->second;
              hasInitValue = true;
              SIM_PRINT_L5(verb(), "ExpansionStack::foundParameter local param value found " << internalFoundParamValue.get());
            }
          else
            SIM_PRINT_L5(verb(), "ExpansionStack::foundParameter local param value NOT found ");
          return true;
        }

      // ============ X-CALL PARAMS ===========
      auto localParamIt = expansionStack_[i].instantiationParams_.find(paramName);
      if (localParamIt != expansionStack_[i].instantiationParams_.end())
        { // instant param found
          SIM_PRINT_L5(verb(), "ExpansionStack::foundParameter instant param found");
          auto localParamValIt = expansionStack_[i].instantiationParamValues_.find(paramName);
          auto localParamRefIt = expansionStack_[i].instantParamsRefs_.find(paramName);
          internalFoundParam = localParamIt->second;
          internalFoundParamRef = localParamRefIt->second;
          internalFoundParamIsLocal = false;
          if (localParamValIt != expansionStack_[i].instantiationParamValues_.end() )
            {
              internalFoundParamValue = IP_ExpressionParser::getExprForConstantValue( localParamValIt->second );
              hasInitValue = true;
            }
          return true;
        }
    }
  return false;
}

bool ExpansionStack::hasParameter(std::string &paramName, bool &hasInitValue, [[maybe_unused]] bool ignoreActualLevel) const
{
  TICTAC_SPTR<CS_ExpressionBasis> internalFoundParamValue;
  TICTAC_SPTR<CS_Parameter>       internalFoundParamRef;
  IP_Param*                             internalFoundParam;
  bool                                  internalFoundParamIsLocal;
  return foundParameter(paramName, hasInitValue,
      internalFoundParamValue, internalFoundParamRef, internalFoundParam, internalFoundParamIsLocal);
}

TICTAC_SPTR<CS_Parameter> ExpansionStack::getParameterReference(std::string &paramName, [[maybe_unused]] bool ignoreActualLevel) const
{
  TICTAC_SPTR<CS_ExpressionBasis> internalFoundParamValue;
  TICTAC_SPTR<CS_Parameter>       internalFoundParamRef;
  IP_Param*                             internalFoundParam;
  bool                                  internalFoundParamIsLocal;
  bool hasInitValue;
  bool hasParam = foundParameter(paramName, hasInitValue,
      internalFoundParamValue, internalFoundParamRef, internalFoundParam, internalFoundParamIsLocal);

  SIM_ERROR_COND_STOP( !hasParam, "Parameter not found " << paramName);
  return internalFoundParamRef;
}

TICTAC_SPTR<CS_ExpressionBasis> ExpansionStack::getParameterInitialValue(std::string &paramName, [[maybe_unused]] bool ignoreActualLevel) const
{
  TICTAC_SPTR<CS_ExpressionBasis> internalFoundParamValue;
  TICTAC_SPTR<CS_Parameter>       internalFoundParamRef;
  IP_Param*                             internalFoundParam;
  bool                                  internalFoundParamIsLocal;
  bool hasInitValue;
  bool hasParam = foundParameter(paramName, hasInitValue,
      internalFoundParamValue, internalFoundParamRef, internalFoundParam, internalFoundParamIsLocal);
  SIM_PRINT_L5(verb(), "ExpansionStack::getParameterInitialValue " << hasParam << " " << hasInitValue << " p=" << internalFoundParamValue.get() );
  /* int *p = 0 ; p[3]=4; */
  SIM_ERROR_COND_STOP( !hasParam || !hasInitValue || (internalFoundParamValue.get() == NULL),
      "Parameter's initial value not found " << paramName << " ptr=" << internalFoundParamValue.get());
  return internalFoundParamValue;
}

bool ExpansionStack::isGnd(std::string pinName) const
{
  auto subIt = listOfGndNames_.find(pinName);
  if (subIt != listOfGndNames_.end())
    return true;
  else
    return false;
}
MYINT ExpansionStack::isGlobalPinn(std::string pinName) const {
  auto subIt = globalPinns_.find(pinName);
  if (subIt != globalPinns_.end())
    return subIt->second;
  else
    return -1;
}
void ExpansionStack::setGlobalPinnID(std::string pinName, MYINT id)
{
  auto subIt = globalPinns_.find(pinName);
  // if this is found then set the new value for this global pin
  if (subIt != globalPinns_.end())
    globalPinns_[pinName] = id;
}

void ExpansionStack::addNewLevel(IP_SubcirDef *actSubCir, const std::string &xcallName )
{
  // add a new level
  ExpansionLevelInformation newElem(actSubCir, xcallName);
  expansionStack_.push_back(newElem);
}

void ExpansionStack::removeLastLevel()
{
  // just remove the last element
  expansionStack_.pop_back();
}

IP_SubcirDef* ExpansionStack::getSubcircuit(const std::string &subcktName, MYINT nrPinns)
{
  IP_SubcirDef* ret = NULL;
  for (MYINT i = (MYINT)expansionStack_.size()-1; i >= 0; i-- ) {
      ret = expansionStack_[i].getSubcircuit( subcktName, nrPinns);
      if (ret != NULL)
        return ret;
  }
  // return null
  return ret;
}

IP_Model *
    ExpansionStack::getModel(std::string &modelName, std::string &modelType)
{
  IP_Model* ret = NULL;
  for (MYINT i = (MYINT)expansionStack_.size()-1; i >= 0; i-- ) {
      ret = expansionStack_[i].getModel( modelName, modelType);
      if (ret != NULL)
        return ret;
  }
  // return null
  return ret;
}

IP_Model* ExpansionStack::getModel(std::string &modelName)
{
  IP_Model* ret = NULL;
  for (MYINT i = (MYINT)expansionStack_.size()-1; i >= 0; i-- ) {
      ret = expansionStack_[i].getModel( modelName);
      if (ret != NULL)
        return ret;
  }
  // return null
  return ret;
}

IP_AnaysisCard* ExpansionStack::getAnalysisCard(AnalysisCardType analysis_type)
{
  return expansionStack_[0].getAnalysisCard(analysis_type); // Analyses must be on toplevel!
}

bool ExpansionStack::is_analysis_used(AnalysisCardType analysis_type)
{
  return expansionStack_[0].is_analysis_used(analysis_type); // Analyses must be on toplevel!
}

/** returns the element of a given name, if found on the current level*/
IP_Element* ExpansionStack::getElement(std::string &modelName)
{
  return expansionStack_[expansionStack_.size() - 1].getElement(modelName);
}

IP_XCall *
    ExpansionStack::getXCall(std::string &xcallname)
{
  return expansionStack_[expansionStack_.size() - 1].getXCall(xcallname);
}

std::vector<IP_XCall *>
    ExpansionStack::getXCalls()
{
  return expansionStack_[expansionStack_.size() - 1].getXCalls();
}

std::vector<IP_Element *>
    ExpansionStack::getElements()
{
  return expansionStack_[expansionStack_.size() - 1].getElements();
}

std::string ExpansionStack::getHierarchicalName(std::string posStr)
{
  std::string ret = "";
  // IMPORTANT, we start from "1" since "TOPLEVEL" should not be there
  for (MYINT i = 1; i < (MYINT)expansionStack_.size(); ++i ) {
      ret += expansionStack_[i].xcallName_ + IP_HIERACHY_DELIMITER_STRING;
  }
  if (posStr.size() > 0)
    ret += posStr;
  else
    ret.pop_back();
  // return the modified string
  return ret;
}

// =========================================================
// ======================== ExpansionLevelInformation=======
// =========================================================

ExpansionLevelInformation::ExpansionLevelInformation(IP_SubcirDef *actSubCir, std::string xcallName)
{
  actSubCir_           = actSubCir;
  xcallName_           = xcallName;
  indexInTheInsertion_ = 0;
}

IP_SubcirDef *
    ExpansionLevelInformation::getSubcircuit(const std::string &subcktName, [[maybe_unused]] MYINT nrPinns)
{
  // TODO: the number of pinns are not yet considered
  auto subIt = subcircuitDefs_.find(subcktName);
  if (subIt != subcircuitDefs_.end())
    return subIt->second;
  else
    return NULL;
}

MYINT
    ExpansionLevelInformation::hasInstantiationPin(const std::string &pinName)
{
  auto subIt = instantiationPinns_.find(pinName);
  if (subIt == instantiationPinns_.end())
    return -2;
  else
    return subIt->second;
}

MYINT
    ExpansionLevelInformation::hasInternalPin(const std::string &pinName)
{
  auto subIt = internalPinns_.find(pinName);
  if (subIt == internalPinns_.end())
    return -2;
  else
    return subIt->second;
}

IP_Model *
    ExpansionLevelInformation::getModel(std::string const &modelName)
{
  // TODO: the type of the model is not considered yet
  auto subIt = localModels_.find(modelName);
  if (subIt != localModels_.end())
    return subIt->second;
  else
    return NULL;
}
IP_Model *
    ExpansionLevelInformation::getModel(std::string &modelName, std::string &modelType)
{
  auto subIt           = localModels_.find(modelName);
  indexInTheInsertion_ = 0;
  while (subIt != localModels_.end())
    {
      IP_Model *model = subIt->second;
      // std::cout << " modelName=" << modelName << " modelType=" << modelType << "\n";
      // std::cout << " " << model->getName() << " " << model->getType() << "\n";
      if (model->getType() == modelType) return model;
      // get the next with this name
      ++subIt;
      indexInTheInsertion_++;
    }
  return NULL;
}

void
    ExpansionLevelInformation::addModel(IP_Model *model)
{
  // std::cout << " addModel " << actSubCir_->getName() << " model=" << model->getName() << "\n";
  localModels_.insert(std::pair<std::string, IP_Model *>(model->getName(), model));
}
void
    ExpansionLevelInformation::addModelParamValues(const IP_Model *                   model,
                                                   std::vector<ConstExpressionValue> &values)
{
  modelParamValues_.insert(
      std::pair<std::string, std::vector<ConstExpressionValue>>(model->getName(), values));
}

std::vector<ConstExpressionValue> &
    ExpansionLevelInformation::getModelParams(std::string &modelName, bool &error)
{
  // TODO: the type of the model is not considered yet
  auto subIt                                  = modelParamValues_.find(modelName);
  error                                       = false;
  if (subIt == modelParamValues_.end()) error = true;
  // return always this
  return subIt->second;
}
/** returns the value of the parameters for the model (the model must be existent within the
 * subcircuit )*/
std::vector<ConstExpressionValue> &
    ExpansionLevelInformation::getModelParams(std::string &modelName,
                                              std::string &modelType,
                                              bool &       error)
{
  IP_Model *model = getModel(modelName, modelType);
  auto      subIt = modelParamValues_.find(modelName);
  error           = false;
  if (model != NULL)
    {  // ASUMING that they were added in the same order
      for (MYINT i = 0; i < indexInTheInsertion_; ++i) ++subIt;
    }
  else
    error = true;
  return subIt->second;
}


void
    ExpansionLevelInformation::addLocalParam(IP_Param *param)
{
  localParams_.insert(std::pair<std::string, IP_Param *>(param->getName(), param));
  localParamsRefs_.insert(std::pair<std::string, TICTAC_SPTR<CS_Parameter>>(
      param->getName(),
      TICTAC_SPTR<CS_Parameter>(
          new CS_Parameter(param->getName(),
                           (const CS_ModuleDeclaration *)param->getToken(1).getFileNameDummyMod(),
                           param->getToken(1).getColumnIndex(),
                           param->getToken(1).getRowIndex(),
                           localParamsRefs_.size()))));
}

void
    ExpansionLevelInformation::addLocalParamValue(std::string &                    param,
                                                  TICTAC_SPTR<CS_ExpressionBasis> &epr)
{
  // std::cout << "ExpansionLevelInformation::addLocalParamValue name=" << param << "\n";
  localParamValues_.insert(std::pair<std::string, TICTAC_SPTR<CS_ExpressionBasis>>(param, epr));
}

void
    ExpansionLevelInformation::addInstantParam(IP_Param *param)
{
  instantiationParams_.insert(std::pair<std::string, IP_Param *>(param->getName(), param));
  instantParamsRefs_.insert(std::pair<std::string, TICTAC_SPTR<CS_Parameter>>(
      param->getName(),
      TICTAC_SPTR<CS_Parameter>(
          new CS_Parameter(param->getName(),
                           (const CS_ModuleDeclaration *)param->getToken(1).getFileNameDummyMod(),
                           param->getToken(1).getColumnIndex(),
                           param->getToken(1).getRowIndex(),
                           localParamsRefs_.size()))));
}
void
    ExpansionLevelInformation::addInstantParamValue(std::string &param, ConstExpressionValue &epr)
{
  instantiationParamValues_.insert(std::pair<std::string, ConstExpressionValue>(param, epr));
}

void
    ExpansionLevelInformation::addInstantPinn(const std::string &pinn, MYINT id)
{
  instantiationPinns_.insert(std::pair<std::string, MYINT>(pinn, id));
}
void
    ExpansionLevelInformation::addInternalPinn(const std::string &pinn, MYINT id)
{
  internalPinns_.insert(std::pair<std::string, MYINT>(pinn, id));
}

IP_AnaysisCard* ExpansionLevelInformation::getAnalysisCard(AnalysisCardType analysis_type)
{
  for (auto& analysis : analysiscardToExpand_)
    if (analysis->getAnalysisType() == analysis_type) return analysis;
  return nullptr;
}

bool ExpansionLevelInformation::is_analysis_used(AnalysisCardType analysis_type)
{
  for (auto& analysis : analysiscardToExpand_)
    if (analysis->getAnalysisType() == analysis_type) return true;
  return false;
}


IP_Element *
    ExpansionLevelInformation::getElement(std::string name)
{
  for (int i = 0; i < (int)elementsToExpand_.size(); ++i)
    if (elementsToExpand_[i]->getInstName() == name) return elementsToExpand_[i];
  return 0;
}


IP_XCall *
    ExpansionLevelInformation::getXCall(std::string name)
{
  for (auto el : xCallsToExpand_)
    {
        if (el->getInstantionationXname() == name) return el;
    }
  return 0;
}
