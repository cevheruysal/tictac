/*
 * IP_NetlistExpander.cpp
 *
 *  Created on: Feb 23, 2015
 *      Author: benk
 */

#include "IP_NetlistExpander.hpp"

#include "IP_NetlistVisitBase.hpp"

#include "IP_SubcirDef.hpp"
#include "IP_NetlistStructBase.hpp"
#include "IP_ControlBase.hpp"

#include "IP_SubcirDef.hpp"
#include "IP_Element.hpp"
#include "IP_Function.hpp"
#include "IP_Model.hpp"
#include "IP_Param.hpp"
#include "IP_SubcirSettings.hpp"
#include "IP_XCall.hpp"
#include "IP_ConditionedNetlist.hpp"
#include "IP_GlobalSettings.hpp"
#include "IP_Option.hpp"
#include "IP_Output.hpp"
#include "IP_Measurement.hpp"
#include "IP_Assertion.hpp"
#include "IP_AnaysisCard.hpp"
#include "IP_NetlistStructSequence.hpp"

#include "IP_ExpressionParser.hpp"
#include "IP_ExprConstEvaluator.hpp"

#include "AN_TranAnalysis.hpp"
#include "AN_DCAnalysis.hpp"

#include "OUT_MatlabPlotter.hpp"
#include "OUT_GnuplotPlotter.hpp"

IP_NetlistExpander::IP_NetlistExpander()
{
  hadExprError_ = withinSubCirDef_ = false;
  hadError_ = false;
  saveFile_ = "tictac";
  // this->setVerb(9);
}

IP_NetlistExpander::~IP_NetlistExpander()
{
  // at the moment nothing to do
}

void IP_NetlistExpander::doNetlistExpansion(
    TICTAC_SPTR<IP_SubcirDef> toplevel)
{
  //this->setVerb(6);
  // initialize and clear everuthing
  exprProcessError_ = "";
  allErrors_ = "";
  saveFile_ = "tictac";
  hadError_ = hadExprError_ = false;
  toplevel_ = toplevel;
  withinSubCirDef_ = false;
  globalSettings_.clear();
  analysis_.clear();
  globalOptions_.clear();
  globalICs_.clear();
  outputs_.clear();
  allPins_.clear();
  allDevices_.clear();
  allXCalls_.clear();

  // create netlist and make some netlist initializations
  netlist_ = TICTAC_SPTR<Netlist>(new Netlist("toplevel") );
  // "0" as ground must always be added and the first pinn must be ground !!!
  std::string gnd = "0";
  netlist_->addNewExternalPin(gnd);

  // - start expansion and add a new expansion level
  expansionStack_.addNewLevel(toplevel_.get() ,  toplevel_->getName() );
  treatActualLevel(toplevel_.get());

  // expansion done, now process global options
  processTopLevelStuff_And_CreateTictac();

  // remove the last level
  expansionStack_.removeLastLevel();
}

void IP_NetlistExpander::treatActualLevel(
    IP_SubcirDef* subcircuitToExpand)
{
  // - first iteration (do only first iteration)
  // (later we might have a second call for conditioned netlists)
  // gather all X-calls and elements
  visitSubcircuit(subcircuitToExpand);

  // evaluate all local parameters, and create expressions for each of them
  for( auto it = expansionStack_.getExpansionLastLevel().getLocalParamMap().begin();
      it != expansionStack_.getExpansionLastLevel().getLocalParamMap().end(); ++it)
    {
      const IP_Param* param = it->second;
      const IP_Token& paramValue = param->getInitExprToken();
      std::string name = param->getName();
      SIM_PRINT_L4(verb(), " setVALUE to local param =" << param->getName() );
      // for each parameter we insert an expression
      TICTAC_SPTR<CS_ExpressionBasis> expr = parseExpression(&paramValue, &expansionStack_);
      // test parsing error
      if (hadExprError_) {
          reportError( this->hadExprError_, "Error by evaluating local parameter:" + param->getName() );
          return;
      }
      expansionStack_.getExpansionLastLevel().addLocalParamValue(name, expr);
    }

  // for each found model get the model values
  for( auto it = expansionStack_.getExpansionLastLevel().getLocalModels().begin();
      it != expansionStack_.getExpansionLastLevel().getLocalModels().end(); ++it)
    {
      const IP_Model* model =  it->second;
      MYINT nrMParams = model->getNrModelParams();
      SIM_PRINT_L4(verb(), " setValues local model =" << model->getName()
          << " t=" << model->getType()
          << " nrparams=" << nrMParams);
      std::vector< ConstExpressionValue > paramValues(0);
      // for each model parameter parse and evaluate the parameters
      for (MYINT i = 0; i < nrMParams; i++) {
          paramValues.push_back(
              evalConstExpression( &(model->getModelParam(i)->getInitExprToken()), &expansionStack_) );
          // test parsing error
          if (hadExprError_) {
              reportError( this->hadExprError_, "Error by evaluating model:" + model->getName()
                  + " param:"
                  + model->getModelParam(i)->getName() );
              return;
          }
      }
      // once this is ready add to the model
      expansionStack_.getExpansionLastLevel().addModelParamValues(model, paramValues);
    }

  // expand all elements
  expandXCalls();
  // just call these two routines
  expandActualLevelDevices();
}

void  IP_NetlistExpander::visitSubcircuit(IP_SubcirDef* toplevelToVisit)
{
  // set the actual top-level
  actTopLevel_ = toplevelToVisit;
  isTopLevel_ = true;
  level_ = -1;
  // visit the top level
  visitNode(actTopLevel_);
  // set back the pointer to NULL
  actTopLevel_ = NULL;
}

bool IP_NetlistExpander::isNextTokenEqual(const IP_Element* elem, MYINT index)
{
  bool ret = false;
  // test if the next token is equal sign
  if (elem->getNrTokens() > index+1) {
      if (elem->getToken(index+1).getTokenType() == TK_EQUAL)
        ret = true;
  }
  return ret;
}

void IP_NetlistExpander::getTokenPairs(const IP_Element* elem, MYINT index,
    bool &isPair, std::string &localName, std::string &localValue, bool &error)
{
  MYINT nrTokens = elem->getNrTokens();
  error = false;
  localName = "";
  localValue = "";
  if ( index >= nrTokens ) { error = true; return;}
  if (isNextTokenEqual(elem, index))
     { // this is "name=value"
       if ( index+2 >= nrTokens ) { error = true; return;}
       localName = elem->getToken(index).getValue();
       localValue = elem->getToken(index+2).getValue();
       isPair = true;
     }
   else
     { // this is just "value"
       localValue = elem->getToken(index).getValue();
       isPair = false;
     }
}

Device* IP_NetlistExpander::matchAndCreateDevice(const IP_Element* elem,
    const IP_DeviceMatchinRule* rule)
{
  MYINT index = 1; ///< token index in the element record
  MYINT nrPinns = rule->getNrPinns(), i, actNrPinns, actPin;
  std::vector<std::string>          localPinnNames(nrPinns, std::string("") );
  std::vector<std::string>          devPinnNames(nrPinns, std::string("") );
  std::vector<MYINT>                devPinnIndx(nrPinns, -1 );
  const std::vector< MYINT >       &pinnFlags = rule->getPinFlags();
  MYINT nrTokens = elem->getNrTokens();
  bool hasFirstKeyword = ((rule->getFirstKeyWord() == "") ? false : true);
  bool hasMatch = true;
  bool error = true;
  bool isPair = true;
  bool hadFirstKeyValue = false;
  std::string pairValue = "";
  std::string pairName = "";

  SIM_PRINT_L4(verb(),"current element: "<<elem->getToken(0).getValue());

  // first try to match the pinns
  actNrPinns = 0;
  // ================================= LOOP PIN ==============================
  for (i = 0; i < nrPinns; i++)
    {
      SIM_PRINT_L4(verb(), " find pinn i=" << i << " index=" << index << " nrPinns=" << nrPinns);
      // open and closed bracets shoud be ignored
      if ((elem->getToken(index).getTokenType() == TK_OPEN_BR) ) {
          index++;
          if (index >= nrTokens) { hasMatch = false; break;}
      }
      // closed bracets just should be ignored
      if ((elem->getToken(index).getTokenType() == TK_CLOSE_BR) )
        index++;

      // test if we run out of tokens, then no match
      if ( index >= nrTokens ) { hasMatch = false; break;}

      // test if keyword check is necessary
      if (    hasFirstKeyword
           && (rule->getFirstKeyWord() == elem->getToken(index).getValue())) {
          // test if we expect a construct keyword=fixedvalue
          if (rule->getFirstKeyValue() != "") {
              getTokenPairs(elem, index, isPair, pairValue, pairName, error);
              // than the "fixedvalue" must be matched otherwise we have no match
              if (!isPair || (pairName != rule->getFirstKeyValue())) { hasMatch = false; break;}
              index = index + 3;
            }
          else {
              index++;
          }
          hadFirstKeyValue = true;
          // test if we have roun out of tokens
          if ( index >= nrTokens ) { hasMatch = false; break;}
        }

      // get the pairs if they are any
      getTokenPairs(elem, index, isPair, pairName, pairValue, error);
      SIM_PRINT_L5(verb(), " pair isPair=" << isPair << " pairName=" << pairName
          << " pairValue=" << pairValue << " error=" << error);
      // if error then exit
      if (error) { hasMatch = false; break;}
      actPin = -1;
      if (isPair)
        { // this is pair so we need to position this correctly
          // the pinn name shoudl be found!
          MYINT j, ind = -1;
          // find the entry
          for (j = 0; j < (MYINT)rule->getPinNames().size(); j++) {
              if (rule->getPinNames()[j] == pairName) {
                  actPin = ind = j;
                  break;
              }
          }
          if ( ind < 0 ) { hasMatch = false; break;}
          // set the fields accordingly
          localPinnNames[ind] = pairValue;
          devPinnNames[ind] = pairName;
          // if this is already -2 means error
          if ( devPinnIndx[ind] == -2 ) { hasMatch = false; break;}
          devPinnIndx[ind] = -2; // this means this pinn is set
        }
      else
        {
          localPinnNames[actNrPinns] = pairValue;
          // if this is already -2 means error
          if ( devPinnIndx[actNrPinns] == -2 ) { hasMatch = false; break;}
          devPinnIndx[actNrPinns] = -2;
          actPin = actNrPinns;
        }
      // pinns after "keyword" must have != pinn flags (means they are only input)
      if (hadFirstKeyValue && pinnFlags[actPin] != 0) { hasMatch = false; break;}
      // increment the index in the tokens
      index = ((isPair) ? (index+3) : (index+1));
      // increase the number of pinns
      actNrPinns++;
    }
  // ================================= END LOOP PIN ============================

  SIM_PRINT_L5(verb(), " after pinn-matching=" << hasMatch << " hasFirstKeyword="
      << hasFirstKeyword << " hadFirstKeyValue=" << hadFirstKeyValue );

  // EARLY EXIT LOGIC AFTER THE PINNS WHERE MATCHED
  if (!hasMatch) return NULL;
  // if we did not hit the required keyword than we also have no match
  if (hasFirstKeyword && !hadFirstKeyValue) return NULL;

  // if we got until here then go on with parameter parsing
  bool hasModuleName = (rule->getModuleName() != "");
  bool foundModuleName = false;
  std::string moduleName = rule->getModuleName();
  bool hasModelType = (rule->getModelType() != "");
  bool foundModel = false;
  std::string modelType = rule->getModelType();
  bool hasMNAExpr = (rule->getMNAKeyword() != "");
  bool doParamParsing = true;

  MYINT actParamInd = 0;
  std::vector< std::string >         paramNames = rule->getParamName();
  std::vector<VarType>               paramType  = rule->getParamType();
  std::vector<ConstExpressionValue>  paramDefValues = rule->getParamsDefulatValue();
  std::vector< MYINT >               paramFlags(paramNames.size(), -1);
  MYINT maxParamNr                =  (MYINT)paramNames.size();
  std::vector< TICTAC_SPTR<CS_ExpressionBasis> > MNAExprs(0);

  // ============================== LOOP FOR PARAMETERS =====================
  // loop as long we all the given param
  while (doParamParsing && hasMatch && (actParamInd < maxParamNr))
    {
      SIM_PRINT_L5(verb(), " parse param index=" << index << " foundModuleName="
          << foundModuleName << " hasModuleName=" << hasModuleName);

      // if not enough tokens present exit
      if (index >= nrTokens) break;
      // get the actual token value
      const std::string& actToken = elem->getToken(index).getValue();

      // module name was found
      if (hasModuleName && !foundModuleName && (actToken == moduleName))
        { // the module name is found
          foundModuleName = true;
          index++;
          SIM_PRINT_L5(verb(), " Module found index=" << index);
          continue;
        }

      // the first "parameter" could be the model if this is required
      if (hasModelType && !foundModel && (actParamInd == 0)) {
          // TODO: here a .model could/should be referenced
          // TODO: later for a device if it has a model, we should also look for the model
          //       and parse its parameters and pass everything to the device
          // RIGHT NOW WE DO NOT USE THE .MODEL CARDS, LATER WE SHOULD IMPLEMENT THIS !!!
          foundModel = true;
          index++;
          SIM_PRINT_L5(verb(), " Model found index=" << index);
      }

      // this flag could be used to treat "(", ")" tokens
      bool isNextParamArray = ((paramType[actParamInd] == REAL_ARRAY)
          || (paramType[actParamInd] == STRING_ARRAY));
      SIM_PRINT_L5(verb(), " act param ind=" << actParamInd
          << " type=" << paramType[actParamInd] << " index=" << index
          << " token=" << elem->getToken(index).getValue() << " isNextParamArray=" << isNextParamArray);

      // if next parameter is NOT an array and we have "(" than we should
      if ((elem->getToken(index).getTokenType() == TK_OPEN_BR) && !isNextParamArray) {
        index++;
        SIM_PRINT_L5(verb(), " FOUND TK_OPEN_BR index=" << index );
        if (index >= nrTokens) { hasMatch = false; break; }
      }
      // closed bracets just should be ignored
      if ((elem->getToken(index).getTokenType() == TK_CLOSE_BR) ) {
          index++;
          SIM_PRINT_L5(verb(), " FOUND TK_CLOSE_BR index=" << index );
          if (index >= nrTokens) break;
      }

      // FOR MNA variables exit here
      if (hasMNAExpr && (elem->getToken(index).getValue() == rule->getMNAKeyword())) {
          SIM_PRINT_L5(verb(), " FOUND MNA EXPRESSION index=" << index );
          index++;
          if (index >= nrTokens) { hasMatch = false; break;}
          if (elem->getToken(index).getTokenType() == TK_EQUAL) {
              index++;
              if (index >= nrTokens) { hasMatch = false; break;}
              // parse the expression
              TICTAC_SPTR<CS_ExpressionBasis> expr =
                  parseExpression( &(elem->getToken(index)), &expansionStack_);
              // test parsing error
              if (hadExprError_) {  hasMatch = false; break; }
              MNAExprs.push_back(expr);
              break;
          }
          else {
              hasMatch = false;
              break;
          }
      }

      // get the pairs if they are any
      getTokenPairs(elem, index, isPair, pairName, pairValue, error);
      SIM_PRINT_L5(verb(), " pair index=" << index << " isPair=" << isPair
          << " pairName=" << pairName << " pairValue=" << pairValue << " error=" << error);

      // TEST if we had parameter pairs
      if (isPair)
        { // this is "paramname = paramValue" construct

          MYINT j, ind = -1;
          for (j = 0; j < (MYINT)paramNames.size(); ++j ){
              if (pairName == paramNames[j]) {
                  ind = j;
                  break;
              }
          }
          SIM_PRINT_L6(verb(), " after looking for " << pairName << " ind=" << ind );

          // test if this parameter by name was found
          if ( ind < 0 ) {
              // no param found
              // TODO: tread "M" "TEMP" "DTEMP" (TEMP could be a pinn for pinns we need only ID)
              hasMatch = false;
              break;
          }
          else { // param found than
              // this is simple "paramValue" construct
              ConstExpressionValue paramVal = evalConstExpression(&(elem->getToken(index+2)), &expansionStack_ );
              // treat parsing error
              SIM_PRINT_L5(verb(), " after (param) parsing =" << hadExprError_
                  << " paramValType=" << paramVal.valueType_);
              if (hadExprError_) {  hasMatch = false; break; }
              // parameter type must match!
              if (paramVal.valueType_ != paramType[ind]) {  hasMatch = false; break; }
              paramDefValues[ind] = paramVal;
              paramFlags[ind]     = 0;
              index = index + 3;
          }
        }
      else
        {
          // this will be the value of the parameter which is not given as "pair"
          ConstExpressionValue paramVal;
          SIM_PRINT_L5(verb(), " Token type=" << elem->getToken(index).getTokenType()
              << " isNextParamArray=" << isNextParamArray);
          if (   (elem->getToken(index).getTokenType() == TK_OPEN_BR) && isNextParamArray)
            {
              SIM_PRINT_L5(verb(), " Starting ARRAY parsing index=" << index);
              index++;
              VarType  elemType = ((paramType[actParamInd] == REAL_ARRAY)? REAL : STRING);
              while ( (index < nrTokens) && (elem->getToken(index).getTokenType() != TK_CLOSE_BR))
                {
                  SIM_PRINT_L5(verb(), " parse array elem index=" << index
                      << " token=" << elem->getToken(index).getValue()
                      << " type=" << elem->getToken(index).getTokenType() );
                  ConstExpressionValue arrElem;
                  // try to parse this expression
                  arrElem = evalConstExpression(&(elem->getToken(index)), &expansionStack_ );
                  // treat parsing error
                  if (hadExprError_) {  hasMatch = false; break; }
                  // elem type must match the array element type
                  if (arrElem.valueType_ == elemType) {
                    if (elemType == REAL)
                      paramVal.aval_.push_back(arrElem.dval_);
                    else
                      paramVal.saval_.push_back(arrElem.sval_);
                  }
                  else
                    // no type match ... this could be an error, at this time no match
                    { hasMatch = false; break;}
                  index++;
                }
              // pass the "TK_OPEN_BR" token
              SIM_PRINT_L5(verb(), " pass the TK_CLOSE_BR token index=" << index
                  << " token=" << elem->getToken(index).getValue()
                  << " type=" << elem->getToken(index).getTokenType() );
              index++;
            }
          else {
              SIM_PRINT_L5(verb(), " simple param value actParamInd=" << actParamInd << " v="
                  << elem->getToken(index).getValue());
              // this is simple "paramValue" construct
              paramVal = evalConstExpression(&(elem->getToken(index)), &expansionStack_ );
              SIM_PRINT_L5(verb(), " after parsing =" << hadExprError_
                  << " paramValType=" << paramVal.valueType_);
              // treat parsing error
              if (hadExprError_) {  hasMatch = false; break; }
              // parameter type must match!
              if (paramType[actParamInd] != paramVal.valueType_) {  hasMatch = false; break; }
              index++;
            }
          paramDefValues[actParamInd] = paramVal;
          paramFlags[actParamInd]     = 0;
        }

      SIM_PRINT_L5(verb(), " get next param");
      // increase the number of found parameters
      actParamInd++;
    }
  // ============================== END LOOP FOR PARAMETERS ===================

  SIM_PRINT_L5(verb(), " after param matching=" << hasMatch << " hasModuleName=" << hasModuleName
      << " foundModuleName=" << foundModuleName );
  // EARLY EXIT LOGIC (params must match and pinss must match)
  //if (index != nrTokens) return NULL;
  if (!hasMatch) return NULL;
  // if module not given then no match
  if (hasModuleName && !foundModuleName) return NULL;

  // ALL TOKENS MUST BE USED
  while ((index < nrTokens)  && (elem->getToken(index).getTokenType() == TK_CLOSE_BR))
    index++;
  SIM_PRINT_L5(verb(), " are tokens left? index=" << index << " nrTokens=" << nrTokens
      << " fl=" << (index != nrTokens) );
  // if there are tokens left then we have no match
  if (index != nrTokens) return NULL;


  // ====== FROM HERE WE HAVE MATCH ======
  // add hierarchical info to the device name
  std::string instanceName = expansionStack_.getHierarchicalName(elem->getInstName());
  SIM_PRINT_L3( verb(), " ========== CREATE DEVICE:" << instanceName << " =========== devname:"
      << rule->getDeviceTypeName() << " type:" << rule->getDevType() );

  // first create pinns if necessary for the
  for (i = 0; i < nrPinns; i++) {
    devPinnIndx[i] = this->getLocalPinn( localPinnNames[i]);
    SIM_PRINT_L3(verb(), " pin(" << i << ")=" << devPinnIndx[i] << " pinnname=" << localPinnNames[i]);
  }

  // TODO: set TEMP, DTEMP if they are given

  // create the device throught the rule
  Device* createdDev = rule->createDevice( instanceName,
      devPinnIndx, paramNames, paramDefValues, paramFlags, MNAExprs);

  // TODO: if we have here NULL than this is internal error
  return createdDev;
}

void   IP_NetlistExpander::expandActualLevelDevices()
{
  const std::vector< IP_Element* >& elements = expansionStack_.getExpansionLastLevel().getElements();
  MYINT nrElements = (MYINT)elements.size();
  for (MYINT i = 0; i < nrElements; i++)
    {
      // we get the element and the element type
      const IP_Element* elem = elements[i];
      IP_ElemPreType elemType = elem->getElemType();
      MYINT nrAllDevs = (MYINT)allDevices_.size();
      // get the list for all matchable devices
      const std::vector< TICTAC_SPTR<IP_DeviceMatchinRule> >& devList =
          devMatchList_.getListOfDeviceMatchings(elemType);
      [[maybe_unused]] MYINT found = -1;
      // this is the device pointer which
      Device* matchedDevice = NULL;
      // for the given list of matching devices test in the given order the matching
      for (MYINT j = 0; j < (MYINT)devList.size(); j++)
        {
          // for each element we should have at least one match !!!
          matchedDevice = matchAndCreateDevice(elem, devList[j].get());
          SIM_PRINT_L3(verb(), " Try matching for j=" << j << " result " << matchedDevice);
          // if match was found than exit this loop
          if (matchedDevice != NULL)
            break;
        }
      // test if we found a match
      reportError( matchedDevice == NULL, "No device matching found for " + elem->getInstName());
      // add the device to the netlist
      netlist_->addDevice(TICTAC_SPTR<Device>(matchedDevice));
      // insert the full expanded device name into the list
      allDevices_.insert( std::pair<std::string, MYINT>(
            expansionStack_.getHierarchicalName(elem->getInstName()), nrAllDevs )
          );
    }
}

void   IP_NetlistExpander::expandXCalls()
{
  // copy the X-calls such that they will be saved!
  std::vector< IP_XCall* > xcalls = expansionStack_.getExpansionLastLevel().getXCalls();
  MYINT nrXcalls = (MYINT)xcalls.size();
  for (MYINT i = 0; i < nrXcalls; i++)
    expandXCall(xcalls[i]);
}

void   IP_NetlistExpander::expandXCall(IP_XCall *node)
{
  bool tmpsave = isTopLevel_;
  // we are not on the top-level
  isTopLevel_ = false;

  // get the information from the X-call
  std::string subCirName = node->getSubcktName();
  std::string xCallName  = node->getToken(0).getValue();
  MYINT nrPinns = node->getNrPinns();

  // look for the subcircuit
  IP_SubcirDef *subCirExpanded = expansionStack_.getSubcircuit( subCirName, nrPinns);
  // create a new expansion list,
  reportError(subCirExpanded == NULL, "Subcircuit " + subCirName
      + " not found in expansion of " + xCallName);
  // if NULL than exit this routine
  if (subCirExpanded == NULL) return;
  // if found, ( pinns and params must match!!! )
  reportError( subCirExpanded->getNrPorts() != node->getNrPinns(), "Subcircuit " + subCirName
      + " has " + sim_integerToString(subCirExpanded->getNrPorts())
      + " pinns but required are " + sim_integerToString(node->getNrPinns()) );

  SIM_PRINT_L3(verb(), " IP_NetlistExpander::expandXCall xCallName=" << xCallName
      << " subCirName=" << subCirName);

  //add instantiation pinns (names + index) from the netlist
  std::unordered_map< std::string , MYINT> instantiationPinns;
  for (MYINT i = 0; i < nrPinns; i++)
    {
      MYINT pinID = getLocalPinn( node->getInstantionationPinn(i) );
      instantiationPinns.insert(
          std::pair<std::string , MYINT>(subCirExpanded->getPortInstName(i) , pinID) );
      // build the Xname.pinn string to add this also to the pinn.list
      std::string xCallPinnName = xCallName + IP_HIERACHY_DELIMITER_STRING
                                + subCirExpanded->getPortInstName(i);
      // in allPins_ we should insert all pinns as pinns in the X-call but with the aquired ID
      addLocalPinn(xCallPinnName, pinID);
      SIM_PRINT_L3(verb(), " add local pinn i=" << i << " name=" << subCirExpanded->getPortInstName(i)
          << " pinID=" << pinID);
    }

  // create new level (here since the pinns and other local parameters must be found before)
  std::string fullXName = expansionStack_.getHierarchicalName(xCallName);
  MYINT nrXCalls = (MYINT)allXCalls_.size();
  allXCalls_.insert( std::pair<std::string,MYINT>(fullXName, nrXCalls));

  // ========== add new expansion level to the stack
  expansionStack_.addNewLevel( subCirExpanded, xCallName);
  expansionStack_.getExpansionLastLevel().instantiationPinns_ = instantiationPinns;

  // test if each instantiated parameter is in the subcircuit given
  for (MYINT i = 0; i < node->getNrParams(); ++i)
    {
      reportError(
        (!subCirExpanded->hasParam( node->getCalledParam(i)->getName() ) ),
         "Subcircuit " + subCirName
            + " does not has instant parameter " + node->getCalledParam(i)->getName() );
    }

  // add instantiated parameter values
  std::unordered_map< std::string , ConstExpressionValue > instantiationParamValues;
  for (MYINT i = 0; i < subCirExpanded->getNrParams(); ++i)
    {
      const std::string& pname =  subCirExpanded->getParamName(i);
      expansionStack_.getExpansionLastLevel().addInstantParam( subCirExpanded->getParam(i).get() );
      MYINT instInd = node->hasInstParam(pname);
      if ( instInd >= 0) {
          // this parameter is given on the X-call
          instantiationParamValues.insert(std::pair<std::string , ConstExpressionValue>(
              pname,
              evalConstExpression( &(node->getCalledParam(instInd)->getInitExprToken()) ,
                  &expansionStack_ , true)
              )
          );
      }
      else {
          TICTAC_SPTR<IP_Param>& param = subCirExpanded->getParam(i);
          instantiationParamValues.insert(std::pair<std::string , ConstExpressionValue>(
              pname, evalConstExpression( &(param->getInitExprToken()) , &expansionStack_ , true ) )
            );
      }
      // treat parsing error
      if (this->hadExprError_) {
          reportError(true, "Error in parameter evaluation of " + subCirExpanded->getParam(i)->getName() +
                            " by xcall " + xCallName + " subck:" + subCirName);
          expansionStack_.removeLastLevel();
          return;
      }
    }
  // set the instantiation parameter values
  expansionStack_.getExpansionLastLevel().instantiationParamValues_ = instantiationParamValues;

  // visit this subcircuit
  treatActualLevel(subCirExpanded);

  // remove this actual last expansion level
  expansionStack_.removeLastLevel();
  isTopLevel_ = tmpsave;
}

// =========================================================
// =============== EXPRESSION EVALUATIONS ===================
// =========================================================
ConstExpressionValue IP_NetlistExpander::evalConstExpression(
    const IP_Token *token, const ExpansionStack* stack, bool ignoreActualLevel)
{
  // this is the CENTRAL routine to evaluate an expression which is only in token
  IP_ExpressionParser parser(ignoreActualLevel);
  IP_ExprConstEvaluator evaluator(0 ,ignoreActualLevel);
  ConstExpressionValue ret;
  hadExprError_ = false;
  TICTAC_SPTR<CS_ExpressionBasis> conExpr
     = parser.parseExpression( token, stack);
  hadExprError_ = parser.hasError();  exprProcessError_ = parser.getErrorMessage();
  SIM_PRINT_L6(verb(), " parseExpression flag=" << hadExprError_ );
  if (!hadExprError_) {
      ret = evaluator.evalConstExpression( conExpr.get(), stack);
      hadExprError_ = evaluator.hasError();  exprProcessError_ = evaluator.getErrorMessage();
      SIM_PRINT_L6(verb(), " evalConstExpression flag=" << hadExprError_ << " dv=" << ret.dval_);
  }
  return ret;
}
TICTAC_SPTR<CS_ExpressionBasis> IP_NetlistExpander::parseExpression(
    const IP_Token *token, const ExpansionStack* stack, bool ignoreActualLevel)
{
  IP_ExpressionParser parser(ignoreActualLevel);
  hadExprError_ = false;
  TICTAC_SPTR<CS_ExpressionBasis> conExpr = parser.parseExpression( token, stack);
  hadExprError_ = parser.hasError();  exprProcessError_ = parser.getErrorMessage();
  SIM_PRINT_L6(verb(), " parseExpression flag=" << hadExprError_ );
  // evaluator might had errors store them
  return conExpr;
}
ConstExpressionValue IP_NetlistExpander::evalExpression(
    TICTAC_SPTR<CS_ExpressionBasis>& expr, const ExpansionStack* stack,
    bool ignoreActualLevel)
{
  // CENTRAL routin to evaluate an expression which was parsed already
  IP_ExprConstEvaluator evaluator(0 ,ignoreActualLevel);
  hadExprError_ = false;
  ConstExpressionValue ret = evaluator.evalConstExpression( expr.get(), stack);
  // evaluator might had errors store them
  hadExprError_ = evaluator.hasError();  exprProcessError_ = evaluator.getErrorMessage();
  SIM_PRINT_L6(verb(), " evalConstExpression flag=" << hadExprError_ << " dv=" << ret.dval_);
  return ret;
}

// =======================================================
// ==================== TREAT ERRORS ============
// =======================================================
void IP_NetlistExpander::reportError(bool cond, std::string errorString){
  // if condition is true than set flag and store error message
  if (cond) {
      hadError_ = true;
      allErrors_ += errorString + "\n";
  }
}

// =======================================================
// ==================== PIN EXPANSION ROUTINES ============
// =======================================================
void IP_NetlistExpander::addLocalPinn(const std::string& localPinnName, MYINT pinID)
{
  std::string localPinnNameLow = sim_toLowerString(localPinnName);
  std::string fullPinName = expansionStack_.getHierarchicalName( localPinnNameLow ) ;
  auto it = allPins_.find(fullPinName);
  // if this pinname is not present in the actual list than add it with the ID
  if ( it == allPins_.end()) {
      allPins_.insert( std::pair<std::string , MYINT>(fullPinName, pinID) );
  }
}

MYINT IP_NetlistExpander::getPinn(const std::string& pinnName)
{
  auto it = allPins_.find(pinnName);
  if ( it == allPins_.end())
    return -1;
  else
    return it->second;
}

MYINT IP_NetlistExpander::getDevice(const std::string& devName)
{
  auto it = allDevices_.find(devName);
  if ( it == allDevices_.end())
    return -1;
  else
    return it->second;
}

MYINT IP_NetlistExpander::getLocalPinn(const std::string& localPinnName)
{
  std::string localPinnNameLow = sim_toLowerString(localPinnName);
  std::string fullPinName = expansionStack_.getHierarchicalName( localPinnNameLow ) ;
  auto it = allPins_.find(fullPinName);
  if ( it == allPins_.end())
    {
      MYINT pinnID = expansionStack_.getExpansionLastLevel().hasInstantiationPin(localPinnNameLow);
      // the pinn ID must be a positive number
      if (pinnID < 0)
        {
          // if this is not an instantionation pinn than it could be an internal pinn
          pinnID = expansionStack_.getExpansionLastLevel().hasInternalPin(localPinnNameLow);
          if (pinnID < 0)
            {
              // if it is not an internal pinn than it might be a global pinn
              pinnID = expansionStack_.isGlobalPinn(localPinnNameLow);
              if (pinnID == -1)
                {
                  if (expansionStack_.isGnd(localPinnNameLow)) {
                      pinnID = 0; // this is ground !!!
                      fullPinName = localPinnNameLow;
                      it = allPins_.find(fullPinName);
                      // ground nodes should be added only once
                      if ( it != allPins_.end())
                        return pinnID;
                    }
                  else {
                      expansionStack_.getExpansionLastLevel().addInternalPinn(localPinnNameLow, pinnID);
                      netlist_->addNewExternalPin( fullPinName );
                      pinnID = netlist_->getNrExternalNodes()-1;
                  }
                }
              else {
                  // means global pin has not been assigned
                  if (pinnID < 0) {
                      netlist_->addNewExternalPin( localPinnNameLow );
                      pinnID = netlist_->getNrExternalNodes()-1;
                      expansionStack_.setGlobalPinnID( localPinnNameLow, pinnID);
                      fullPinName = localPinnNameLow;
                      it = allPins_.find(fullPinName);
                      // global names should be added only once and without hierarchy information
                      if ( it != allPins_.end())
                        return pinnID;
                  }
              }
            }
        }
      // adds a pinn to the expanded list
      allPins_.insert( std::pair<std::string , MYINT>(fullPinName, pinnID) );
      return pinnID;
    }
  else
    return it->second;
}


// ================= POSTPROCESS EXPAND =========

MYINT IP_NetlistExpander::isOptionSet(std::string errorString)
{
  for (decltype(globalOptions_.size()) i = 0; i < globalOptions_.size(); ++i) {
      //std::cout << " option " << globalOptions_[i]->getOptionName()  << "\n";
      if ( globalOptions_[i]->getOptionName() == errorString )
        return i;
  }
  return -1;
}

MYREAL IP_NetlistExpander::getOptionRealValue(std::string optionName, MYREAL defaultValue)
{
  MYINT t = isOptionSet(optionName);
  if (t >= 0) {
      ConstExpressionValue val;
      if ( globalOptions_[t]->hasOptionValue()){
          val = evalConstExpression(&globalOptions_[t]->getOptionValue(), &expansionStack_);
          return val.dval_;
      }
  }
  return defaultValue;
}

void IP_NetlistExpander::processTopLevelStuff_And_CreateTictac()
{
  SIM_PRINT_L3(verb(), " ================= IP_NetlistExpander::processTopLevelStuff_And_CreateTictac() ======= ");
  MYINT i, j;

  TICTAC_SPTR<OUT_WaveFormPlotter> plotter;
  ConstExpressionValue val;

  // based on the option set we choose the plotter
  if (isOptionSet("matlab") >= 0)
    plotter = TICTAC_SPTR<OUT_WaveFormPlotter>(new OUT_MatlabPlotter());
  else if (isOptionSet("gnuplot") >= 0)
    // here a gnuplot plotter should be added
    plotter = TICTAC_SPTR<OUT_WaveFormPlotter>(new OUT_GnuplotPlotter());
  else
    plotter = TICTAC_SPTR<OUT_WaveFormPlotter>(new OUT_WaveFormPlotter());

  TICTAC_SPTR<IN_SimOptions> simoptions = TICTAC_SPTR<IN_SimOptions>(new IN_SimOptions());

  // =========== OPTIONS ===========
  simoptions->getTimeStepSizeControlRecord().maximalDeltaT_ = getOptionRealValue("delmax", 1E-8);
  simoptions->getTimeStepSizeControlRecord().minimalDeltaT_ = getOptionRealValue("delmin", 1E-14);
  simoptions->getTimeStepSizeControlRecord().startDeltaT_  = getOptionRealValue("delstart", 1E-14);
  // LTE charge tolerances
  simoptions->getTimeStepSizeControlRecord().chargeRelTol_ = getOptionRealValue("chargereltol",1E-3);
  simoptions->getTimeStepSizeControlRecord().chargeAbsTol_ = getOptionRealValue("chargeabstol",1E-12);
  simoptions->getTimeStepSizeControlRecord().maxChargeLTEFactor_ = getOptionRealValue("chargemaxltefact",2.0);
  simoptions->getTimeStepSizeControlRecord().chargeTRTOL_ = getOptionRealValue("chargetol",40);
  // LTE charge tolerances for the heuristic part
  simoptions->getTimeStepSizeControlRecord().chargeHeuristicAbsTol_= getOptionRealValue("chargeheurabstol",1E-14);
  simoptions->getTimeStepSizeControlRecord().chargeHeuristicRelTol_ = getOptionRealValue("chargeheurreltol",1E-3);
  // =========== readjusting the options ========
  // start delta time must be at least the minimum time step
  if (simoptions->getTimeStepSizeControlRecord().startDeltaT_
       < simoptions->getTimeStepSizeControlRecord().minimalDeltaT_)
    simoptions->getTimeStepSizeControlRecord().startDeltaT_
      = simoptions->getTimeStepSizeControlRecord().minimalDeltaT_;


  // create a TICTAC calculator
  tictacComputer_ = TICTAC_SPTR<TicTacComputer>(new TicTacComputer(this->netlist_, simoptions));

  // set the default save filename for the plotter
  plotter->setFileName(this->saveFile_);

  // all outputs gathered
  std::vector<bool> wasNodePlotted( netlist_->getNrExternalNodes() , false);
  for (i = 0; i < (MYINT)outputs_.size(); ++i )
    {
      const IP_Output* o = outputs_[i];
      // PlotVarType plotType = NODE_VOLTAGE_PLOT;
      // test if we have enough
      reportError (o->getNrTokens() < 3, "Output must have at least three tokens");
      if (o->getNrTokens() < 3) break;

      // TODO: later consider this analysis string for the outputs
      std::string analysis = sim_toLowerString( o->getToken(1).getValue() );
      j = 2;
      while (j < o->getNrTokens() )
        {
          const IP_Token &t = o->getToken(j);
          std::string tv = sim_toLowerString( t.getValue() );
          std::string core = tv; //
          MYINT js,je;
          char plotTypeC = 'v';
          SIM_PRINT_L6(verb(), " core=" << core );
          if (t.getTokenType() == TK_EXPRESSION)
            { // this is either 'V(str)' or 'I(
              // here we need to extract the string
              js = 0;
              while (core[js] == ' ') ++js;
              plotTypeC = core[js];
              js++;
              while ((core[js] == ' ') || (core[js] == '(')) ++js;
              je = core.size() - 1;
              while ((core[je] == ' ') || (core[je] == ')')) --je;
              core = core.substr(js, je-1);
              j = j + 1;
            }
          else
            { // thi is more like "V" "(" "str" ")"
              plotTypeC = tv[0];
              reportError(j+3 >= o->getNrTokens(), "Output must have at least three tokens" );
              if (j+4 > o->getNrTokens() ) { break; }
              reportError(o->getToken(j+1).getTokenType() != TK_OPEN_BR , "Bracket open is missing " );
              reportError(o->getToken(j+3).getTokenType() != TK_CLOSE_BR , "Bracket close is missing " );
              if (hadError_) break;
              SIM_PRINT_L6(verb(), " plotTypeC=" << plotTypeC << "|" << o->getToken(j).getValue()
                  << "|" << o->getToken(j+1).getValue() << "|" << o->getToken(j+2).getValue()
                  << "|" << o->getToken(j+3).getValue() << " hadError_=" << hadError_);
              core = sim_toLowerString(o->getToken(j+2).getValue());
              j = j + 4;
            }
          SIM_PRINT_L6(verb(), " Plot type: " << plotTypeC << " core=" << core );
          // we add the plotting
          if (plotTypeC == 'v')
            {
              // plotType = NODE_VOLTAGE_PLOT;
              // if core == "*" means we want all pinns
              if (core == "*") {
                  for (auto& x: allPins_) {
                      SIM_PRINT_L6(verb(), x.first << " -:- " << x.second );
                      if (!wasNodePlotted[x.second] )
                        plotter->addScalarPlot( x.first , "V", NODE_VOLTAGE_PLOT, x.second-1);
                      wasNodePlotted[x.second] = true;
                  }
                  // go to the next plot statement
                  continue;
              }
              js = getPinn( core );
              SIM_PRINT_L6(verb(), " pinn: " << core << " index=" << js );
              reportError(js < 0, " Pinn '" + core + "' not found");
              if (hadError_) break;
              SIM_PRINT_L6(verb(), " Add Voltage plot  " + netlist_->getNodeName(js));
              if (wasNodePlotted[js]) {
                SIM_PRINT_L6(verb(), " Voltage node " + netlist_->getNodeName(js) + " already plotted");
              }
              else {
                  plotter->addScalarPlot( core, "V", NODE_VOLTAGE_PLOT, js-1);
              }
              wasNodePlotted[js] = true;
            }
          if (plotTypeC == 'i')
            {
              //plotType = CURRENT_PLOT;
              if (core == "*") {
                  SIM_OUT(" I(*) can not be processed, ignoring this output ")
                  continue;
              }
              js = getDevice( core );
              reportError(js < 0, " Device '" + core + "' not found");
              if (hadError_) break;
              SIM_PRINT_L6(verb(), " Add Current plot  " + netlist_->getDevice(js)->getInstanceName()
                  + " dev ind=" + sim_integerToString(js) );
              plotter->addScalarPlot(
                              std::string(netlist_->getDevice(js)->getInstanceName()+":I"),
                              "A", CURRENT_PLOT, js, 0);
              // TODO: Here we assume that this device has always one current -> index 0
            }
        }
    }
  // ========== all anaylisis ===================
  for (i = 0; i < (MYINT)analysis_.size(); ++i )
    {
      const IP_AnaysisCard* o = analysis_[i];
      TICTAC_SPTR<AN_Analysis> an;
      switch (o->getAnalysisType()) {
      case IP_ANALYSIS_DC :
      case IP_ANALYSIS_OP : {
        std::vector<MYINT> ic_node(0);
        std::vector<MYREAL> ic_value(0);
        reportError(o->getNrTokens() > 2, " .DC and .OP analysis does not take any argument at this moment ");
        an = TICTAC_SPTR< AN_Analysis >(new AN_DCAnalysis( netlist_.get(), simoptions.get(), plotter));
        //TODO: an->setInitialCondition( nodeset_node, nodeset_value, nrNodeset);
        for (decltype(globalICs_.size()) j = 0; j < globalICs_.size(); ++j){
            IP_SubcirSettings *ic = globalICs_[j];
            std::string nodeName = ic->getToken(3).getValue();
            const IP_Token &t = ic->getToken(6);
            val = evalConstExpression(&t, &expansionStack_);
            MYINT nodeID = netlist_->getPinID(nodeName);
            // apply this only when this node is found in the netlist
            if (nodeID > 0) {
                ic_node.push_back(nodeID-1);
                ic_value.push_back(val.dval_);
            }
            SIM_PRINT_L6(10, " IC node=" << nodeID << " val=" << val.dval_);
        }
        an->setInitialCondition( ic_node, ic_value, (MYINT)ic_value.size());
        SIM_PRINT_L6(verb(), " Create DC analysis ");
        tictacComputer_->addAnlysis( an );
        break;
      }
      case IP_ANALYSIS_TR : {
        MYREAL startTime, endTime;
        std::vector<MYINT> ic_node(0);
        std::vector<MYREAL> ic_value(0);
        reportError(o->getNrTokens() < 2, " .TRAN requires two scalar arguments ");
        val = evalConstExpression(&o->getToken(1), &expansionStack_);
        reportError( (val.valueType_ != REAL) || this->hadExprError_,
            " Could not evalaute start time for .TRAN ");
        startTime = val.dval_;
        // we also support simple .tran endtime
        if (o->getNrTokens() >= 3)
          {
            val =  evalConstExpression(&o->getToken(2), &expansionStack_);
            reportError( (val.valueType_ != REAL) || this->hadExprError_,
                " Could not evalaute end time for .TRAN ");
            endTime = val.dval_;
          }
        else
          endTime = startTime;
        // create Analysis
        an = TICTAC_SPTR<AN_Analysis>(
            new AN_TranAnalysis( netlist_.get(), startTime, endTime, simoptions.get(), plotter ));
        // process all the ICs is there are any
        for (decltype(globalICs_.size()) j = 0; j < globalICs_.size(); ++j){
            IP_SubcirSettings *ic = globalICs_[j];
            std::string nodeName = ic->getToken(3).getValue();
            const IP_Token &t = ic->getToken(6);
            val = evalConstExpression(&t, &expansionStack_);
            MYINT nodeID = netlist_->getPinID(nodeName);
            // apply this only when this node is found in the netlist
            if (nodeID > 0) {
                ic_node.push_back(nodeID-1);
                ic_value.push_back(val.dval_);
            }
            SIM_PRINT_L6(10, " IC node=" << nodeID << " val=" << val.dval_);
        }
        an->setInitialCondition( ic_node, ic_value, (MYINT)ic_value.size());
        SIM_PRINT_L6(verb(), " Create TRAN analysis ");
        tictacComputer_->addAnlysis( an );
        break;
      }
      case IP_ANALYSIS_AC :
      default: {
        reportError(true, " Analysis not supported yet ");
        break;
      }
      }
    }
  // all the options on the toplevel
  for (i = 0; i < (MYINT)globalOptions_.size(); ++i )
    {
      // TODO: options on the top-level
    }

  // global options
  for (i = 0; i < (MYINT)globalSettings_.size(); ++i )
    {
      // NOTHING TO DO HERE YET (.save was processed already)
      switch (globalSettings_[i]->getGlobalSettingType()) {
      case IP_GSETTING_GLOBAL : { break; }
      case IP_GSETTING_SAVE:    { break; }
      case IP_GSETTING_TEST:    { break; }
      case IP_GSETTING_TEMP:    { break; }
      default: {break; }
      }
    }

  // ================= FINAL ERROR PROCESSING ===============
  if (this->hadError_) {
      SIM_OUT("ERRORs: " + this->allErrors_);
      // TODO: what do do then we have errors
  }
}

// ============================================
// ====================== VISITORS ============
// ============================================

void IP_NetlistExpander::Visit(IP_Param &node){
  // add then as local parameters
  if (!(this->isParamWithinXCall_ ||  this->isParamWithinSubcktDef_ || this->isParamWithinModel_))
    {
      SIM_PRINT_L6(verb(), " addLocalParam() param=" << node.getName() << " subckt name="
          << expansionStack_.getExpansionLastLevel().getActualSubcir()->getName() );
      expansionStack_.getExpansionLastLevel().addLocalParam(&node);
    }
  VisitChildren(node);
}

void IP_NetlistExpander::Visit(IP_ConditionedNetlist &node)
{
  // this expression must be evaluable here !!!
  ConstExpressionValue val = evalConstExpression( &node.getConditionToken(), &expansionStack_);
  // returned value must be scalar number
  reportError( val.valueType_ != REAL, "Expression in condition must be scalar number ");
  // based on the condition wisit one branch or other
  if (val.dval_ != 0.0) {
    visitNode( node.getTrueBranch().get() );
  }
  else {
    if (node.hasFalseBranch())
      visitNode( node.getFalseBranch().get() );
  }
}

void IP_NetlistExpander::Visit(IP_NetlistStructBase &node) { VisitChildren(node); }

void IP_NetlistExpander::Visit(IP_NetlistStructSequence &node) { VisitChildren(node); }

void IP_NetlistExpander::Visit(IP_ControlBase &node) { VisitChildren(node); }

void IP_NetlistExpander::Visit(IP_SubcirDef &node) {
  // depending on the request either visit nodes or just add the subcircuit definition
  SIM_PRINT_L6(verb(), " aIP_NetlistExpander::Visit(IP_SubcirDef &node) level="
      << level_ << " name=" << node.getName());
  if (level_ < 0)
    VisitChildren(node);
  else
    expansionStack_.getExpansionLastLevel().addSubcircuitDef(&node);
}

void IP_NetlistExpander::Visit(IP_Element &node) {
  // just add the element which should be matched
  expansionStack_.getExpansionLastLevel().addElem(&node);
  VisitChildren(node);
}
void IP_NetlistExpander::Visit(IP_Function &node) {
  // add this function to the actual expansion list
  expansionStack_.getExpansionLastLevel().addFunctionDef(&node);
  VisitChildren(node);
}
void IP_NetlistExpander::Visit(IP_Model &node) {
  // add this model to the actual expansion list
  expansionStack_.getExpansionLastLevel().addModel(&node);
  VisitChildren(node);
}
void IP_NetlistExpander::Visit(IP_SubcirSettings &node) {
  if (expansionStack_.getExpansionLastLevel().getActualSubcir()->isSubcirTopLevel())
    {
      if (node.getType() == IP_SSETTING_IC)
        globalICs_.push_back(&node);
    }
  VisitChildren(node);
}
void IP_NetlistExpander::Visit(IP_XCall &node) {
  // add this X-call to the actual expansion list
  expansionStack_.getExpansionLastLevel().addXCall(&node);
  // do not visit, since we might visit the params which they were instantiated
  //VisitChildren(node);
}

void IP_NetlistExpander::Visit(IP_Option &node) {
  // options on the top level should be stored
  if (expansionStack_.getExpansionLastLevel().getActualSubcir()->isSubcirTopLevel())
    globalOptions_.push_back(&node);
  // TODO: subcircuit local options should also be stored and processed later
  VisitChildren(node);
}

void IP_NetlistExpander::Visit(IP_Output &node) {
  // Outputs can come at this time only at the toplevel
  reportError(
      !(expansionStack_.getExpansionLastLevel().getActualSubcir()->isSubcirTopLevel()),
      "Outputs can appear only at the top level");
  // gather such output request
  outputs_.push_back(&node);
  VisitChildren(node);
}
void IP_NetlistExpander::Visit(IP_AnaysisCard &node) {
  // Analysis can come only at the toplevel
  reportError(
      !(expansionStack_.getExpansionLastLevel().getActualSubcir()->isSubcirTopLevel()),
      "Analysis can appear only at the top level");
  // store all the anlysis for later evaluation
  analysis_.push_back(&node);
  VisitChildren(node);
}

void IP_NetlistExpander::Visit(IP_GlobalSettings &node)
{
  reportError(
      !(expansionStack_.getExpansionLastLevel().getActualSubcir()->isSubcirTopLevel()),
      "Global Settings can appear only at the top level");
  // on the toplevel gather
  // .global options
  if ((node.getGlobalSettingType() == IP_GSETTING_GLOBAL))
    expansionStack_.addGlobalPinn( sim_toLowerString(node.getToken(1).getValue()), -10);
  // .save filename
  if ((node.getGlobalSettingType() == IP_GSETTING_SAVE))
    // TODO: this should be evaluataed, since this might an expression with $HOME or similar
    saveFile_ = node.getToken(1).getValue(); // noToLower
  // store the global options for later evaluation
  globalSettings_.push_back(&node);
  VisitChildren(node);
}
void IP_NetlistExpander::Visit(IP_Measurement &node) { VisitChildren(node); }
void IP_NetlistExpander::Visit(IP_Assertion &node) { VisitChildren(node); }
