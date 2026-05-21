/*
 * IP_ExpansionStack.hpp
 *
 *  Created on: Feb 28, 2015
 *      Author: benk
 */

#ifndef IP_EXPANSIONSTACK_HPP_
#define IP_EXPANSIONSTACK_HPP_

#include "sim_utils.hpp"
#include "Netlist.hpp"
#include "IP_SubcirDef.hpp"
#include "IP_XCall.hpp"
#include "IP_NetlistVisitBase.hpp"
#include "CS_ExpressionBasis.hpp"
#include "CS_Parameter.hpp"

#include "IP_Param.hpp"
#include "IP_Function.hpp"
#include "IP_Model.hpp"
#include "IP_AnaysisCard.hpp"

#include "IP_ExprConstEvaluator.hpp"


#define IP_HIERACHY_DELIMITER_STRING "."


/** class which should only contain the information of one X-call, the stack
 * of such information is used in the expansion process */
class ExpansionLevelInformation{
public:
  /// empty Ctor
  ExpansionLevelInformation(IP_SubcirDef *actSubCir, std::string xcallName);
  // empty Dtor
  virtual ~ExpansionLevelInformation() {;}

  /** the name of the X-call */
  std::string xcallName_;

  /// the actual subcircuit definition which gets expanded
  IP_SubcirDef *actSubCir_;

  const IP_SubcirDef* getActualSubcir() const { return actSubCir_; }
  IP_SubcirDef* getActualSubcir() { return actSubCir_; }

  // THESE FIELDS MUST BE HERE DUE TO CONDITIONED NETLISTING !!!

  /// the list of pins with this subcircuit was instantiated with
  std::unordered_map< std::string , MYINT> instantiationPinns_;
  /// the list of all internal pins which were created (subcircuit internal pinns)
  std::unordered_map< std::string , MYINT> internalPinns_;

  void addInstantPinn( const std::string &pinn , MYINT id);
  void addInternalPinn( const std::string &pinn , MYINT id);
  /** returns a positive number (which is the pinn in the netlist)
   * if the given name is an instantiation pin, else negative < 0*/
  MYINT hasInstantiationPin(const std::string &pinName);
  /** returns a positive number (which is the pinn in the netlist)
   * if the given name is a subcircuit internal pin, else negative < 0*/
  MYINT hasInternalPin(const std::string &pinName);

  /// the parameters with the subcircuit is instantiated with
  std::unordered_map< std::string , IP_Param*> instantiationParams_;
  /// we create for each parameter a reference, which can be used by building the expressions
  std::unordered_map< std::string , TICTAC_SPTR<CS_Parameter> > instantParamsRefs_;
  /// the values of the instantiated
  std::unordered_map< std::string , ConstExpressionValue > instantiationParamValues_;

  void addInstantParam( IP_Param* param );
  void addInstantParamValue( std::string &param, ConstExpressionValue& epr );

  /// local parameter (not those from the subcircuit header)
  std::unordered_map< std::string , IP_Param*> localParams_;
  /// we create for each parameter a reference, which can be used by building the expressions
  std::unordered_map< std::string, TICTAC_SPTR<CS_Parameter> > localParamsRefs_;
  /// the value of each local parameter
  std::unordered_map< std::string , TICTAC_SPTR<CS_ExpressionBasis> > localParamValues_;

  /// returns the const map of the local parameters
  const std::unordered_map< std::string , IP_Param*>& getLocalParamMap() const { return localParams_; }
/*  /// returns the const map of the local parameter references
  const std::unordered_map< std::string, TICTAC_SPTR<CS_Parameter> >& getLocalParamRefMap() { return localParamsRefs_; }
  /// returns the const map of the local parameter values
  const std::unordered_map< std::string , TICTAC_SPTR<CS_ExpressionBasis> >& getLocalParamValueMap() { return localParamValues_; } */

  void addLocalParam( IP_Param* param );
  void addLocalParamValue( std::string &param, TICTAC_SPTR<CS_ExpressionBasis>& epr );

private:
  /// list of local models
  std::unordered_multimap< std::string , IP_Model*> localModels_;
  /// here we store for each found model the values of their parameters
  std::unordered_multimap< std::string , std::vector<ConstExpressionValue> > modelParamValues_;
  /// internal state used to get correct parameter set when also the model type is given
  /// when there are multiple models with the same name but different types
  int indexInTheInsertion_;

public:
  /// returns the map with the models
  const std::unordered_multimap< std::string , IP_Model*>& getLocalModels() const { return localModels_; }

  void addModel(IP_Model* model);
  void addModelParamValues(const IP_Model* model, std::vector< ConstExpressionValue >& values);

  /** looks for a model within the subcircuit, if not found than NULL
   * if there multiple model names but with different types, this will return the first one */
  IP_Model* getModel(std::string const &modelName);
  /** looks for a model with a given name and of a given type */
  IP_Model* getModel(std::string &modelName, std::string &modelType);
  /** returns the value of the parameters for the model (the model must be existent within the subcircuit)
   * if there are multiple models with the same name but different types this will return the first one */
  std::vector< ConstExpressionValue >& getModelParams(std::string &modelName, bool &error) ;
  /** returns the value of the parameters for the model (the model must be existent within the subcircuit )*/
  std::vector< ConstExpressionValue >& getModelParams(std::string &modelName, std::string &modelType, bool &error) ;

  // TODO: for models we should be evaluate their parameters staright-ahead after the
  // parameters are ready
  /// get the model according to its name, we should also integrate a type check as well !!! ???


  /// list of local functions
  std::unordered_map< std::string , IP_Function*> localFunctions_;
  void addFunctionDef(IP_Function* function) {
    localFunctions_.insert(std::pair<std::string,IP_Function*>(function->getName(), function));
  }

  /// on this level pointers to all subcircuit definitions
  // TODO: we want also to consider the number of pinns
  std::unordered_map< std::string , IP_SubcirDef*> subcircuitDefs_;
  void addSubcircuitDef(IP_SubcirDef* subcir) {
    subcircuitDefs_.insert(std::pair<std::string,IP_SubcirDef*>(subcir->getName(),subcir));
  }

 private:
  /// list of the elements which should be parsed and added to the netlist
  std::vector< IP_AnaysisCard* > analysiscardToExpand_;

 public:
  void addAnalysisCard(IP_AnaysisCard* elem) { analysiscardToExpand_.push_back(elem); }
  std::vector< IP_AnaysisCard* >& getAnalysisCard() { return analysiscardToExpand_; }

  /** looks for a given instance name on the current level */
  IP_AnaysisCard* getAnalysisCard(AnalysisCardType analysis_type);
  bool is_analysis_used(AnalysisCardType analysis_type);
private:
  /// list of the elements which should be parsed and added to the netlist
  std::vector< IP_Element* > elementsToExpand_;

public:
  void addElem(IP_Element* elem) { elementsToExpand_.push_back(elem); }
  std::vector< IP_Element* >& getElements() { return elementsToExpand_; }

  /** looks for a given instance name on the current level */
  IP_Element* getElement(std::string name);

private:
  /// list of all X-calls (for which expansion should be applied recursively)
  std::vector< IP_XCall* > xCallsToExpand_;

public:
  void addXCall(IP_XCall* xelem) { xCallsToExpand_.push_back(xelem); }
  std::vector< IP_XCall* >& getXCalls() { return xCallsToExpand_; }

  IP_XCall* getXCall(std::string name);

  /** returns a subcircuit if it contains, otherwise NULL
   * Number of pinns are not considered yet, now cound be any value */
  IP_SubcirDef* getSubcircuit(const std::string &subcktName, MYINT nrPinns);
};


/**
 * \brief Class containing all the expansion stack information (multiple levels)
 *
 * This class is needed to resolve models and subcircuits that are referenced
 * in the netlist, for example, by an x-call.
 * The expansion stack is created (or extended) in IP_SubcirDef visitors.
 */
class ExpansionStack : public UT_SIMVerboseClass{
public:
  /// Ctor
  ExpansionStack();
  /// empty Dtor
  virtual ~ExpansionStack() {;}

  /// returns true if a parameter if found, and also says if this parameter has already an initial value
  bool hasParameter(std::string &paramName, bool &hasInitValue,
      bool ignoreActualLevel=false) const;

  /// returns the parameter reference (needed in expressions)
  TICTAC_SPTR<CS_Parameter> getParameterReference(std::string &paramName,
      bool ignoreActualLevel=false) const ;

  /// returns the initial value of a parameter (if there is none than ERROR)
  TICTAC_SPTR<CS_ExpressionBasis> getParameterInitialValue(std::string &paramName,
      bool ignoreActualLevel=false) const;

  /// adds a global pinn name
  void addGlobalPinn(std::string globalPinName, MYINT ind);

  ExpansionLevelInformation& getExpansionLevel(int i) { return expansionStack_[i]; }
  const ExpansionLevelInformation& getExpansionLevel(int i) const { return expansionStack_[i]; }

  ExpansionLevelInformation& getExpansionLastLevel() { return expansionStack_.back(); }
  const ExpansionLevelInformation& getExpansionLastLevel() const { return expansionStack_.back(); }

  /// adds a new level
  void addNewLevel(IP_SubcirDef *actSubCir, const std::string &xcallName );

  /// returns true if we are on the toplevel, false otherwise
  bool isTopLevel() const { return (expansionStack_.size() <= 1); }

  /// removes last level
  void removeLastLevel();

  /// clear all the stack
  void clearAllStack() { expansionStack_.clear(); globalPinns_.clear(); listOfGndNames_.clear(); }


  bool isGnd(std::string pinName) const;
  /** the global pinss are initiated with -10 until they are referred somewhere in the circuit.
   * If a global pin is simply not found than this returns -1, otherwise -10 (or similar) when is found but is not
   * assigned yet. If found and is assigned, then returns the ID of the pin in the netlist */
  MYINT isGlobalPinn(std::string pinName) const;
  /** once a global pin is instantiated it should be assedned to a ID in the real netlist, this method does this. */
  void setGlobalPinnID(std::string pinName, MYINT id);

  /** returns the found subcircuit in the expansion stack, if not found than returns NULL*/
  IP_SubcirDef* getSubcircuit(const std::string &subcktName, MYINT nrPinns);

  /** returns the model if it is found, NULL otherwise*/
  IP_Model* getModel(std::string &modelName, std::string &modelType);

  /** returns the model if it is found, NULL otherwise*/
  IP_Model* getModel(std::string &modelName);

  IP_AnaysisCard* getAnalysisCard(AnalysisCardType analysis_type);
  bool is_analysis_used(AnalysisCardType analysis_type);

  /** returns the element of a given name, if found on the current level*/
  IP_Element* getElement(std::string &modelName);
  std::vector<IP_Element*> getElements();

  /**
   * \brief this function returns a pointer to an xcall
   *
   * \param[in] xcallname is the name of the subcircuit instance
   *
   * \return pointer to the xcall
   */
  IP_XCall* getXCall(std::string &xcallname);
  std::vector<IP_XCall *> getXCalls();

  /** returns the hierarchical name in the current expanded context */
  std::string getHierarchicalName(std::string posStr);

private:

  /// returns true if a parameter if found, and also sais if this parameter has already an initial value
  bool foundParameter(std::string &paramName, bool &hasInitValue,
      TICTAC_SPTR<CS_ExpressionBasis> &internalFoundParamValue,
      TICTAC_SPTR<CS_Parameter>       &internalFoundParamRef,
      IP_Param*                             &internalFoundParam,
      bool                                  &internalFoundParamIsLocal,
      bool ignoreActualLevel=false ) const;

  /// vector representing the stack
  std::vector<ExpansionLevelInformation> expansionStack_;

  /** list of all global signals defined on the toplevel */
  std::unordered_map< std::string , MYINT> globalPinns_;
  /** list of all names eqvivalent to ground */
  std::unordered_map< std::string , MYINT> listOfGndNames_;

};

#endif /* IP_EXPANSIONSTACK_HPP_ */
