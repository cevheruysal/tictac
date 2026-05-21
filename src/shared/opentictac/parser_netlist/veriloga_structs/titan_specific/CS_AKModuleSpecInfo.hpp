/*
 * CSAKModuleSpecInfo.hpp
 *
 * In this file we packed all the global informations that are required for
 * the AK and IP code generation
 *
 *  Created on: Jan 14, 2013
 *      Author: benk
 */

#ifndef CSAKMODULESPECINFO_HPP_
#define CSAKMODULESPECINFO_HPP_

#include "verilog_comp.hpp"

#include "CS_AKDependency.hpp"

//class CS_BranchProbeFunction; //  ... I(p,n) ... -> additional variable
//class CS_ContributionStatement; // class for V(p,n) <+  ... -> additional variable
class CS_AnalogEventFunction;
class CS_ExpressionBasis;
class CS_AnalogFilterOperationFunction;
class CS_AnalogSmallSignalFunction;
class CS_BranchProbeFunction;

// ============================ AK Specific VARABLE INFO =========================

typedef enum {
  VAMSVAR_PORT_VOLTAGE,         /* [MNA] Module port voltage                        */
  VAMSVAR_INTERNAL_VOLTAGE,     /* [MNA] Internal node voltage                      */
  VAMSVAR_BRANCH_CURRENT,       /* [MNA] Internal branch current                    */
  VAMSVAR_PORT_CURRENT,         /* [MNA] Port current                               */
  VAMSVAR_IDT_RESULT,           /* [MNA] Variable for the result of idt(expression) */
  VAMSVAR_ABSDELAY_RESULT,      /* [MNA] Variable for the result of absdelay        */
  VAMSVAR_LAPLACE_RESULT,       /* [MNA] Variable for the result of laplace         */
  VAMSVAR_DDT_RESULT,           /* [FCT] Variable for the result of ddt(expression) */
  VAMSVAR_TRANS_RESULT,         /* [FCT] Variable for the result of transition      */
  VAMSVAR_SLEW_RESULT,          /* [FCT] Variable for the result of slew            */
  VAMSVAR_LASTCROSS_RESULT,     /* [FCT] Variable for the result of last_crossing   */
  VAMSVAR_AC_STIM_RESULT,       /* [FCT] Variable for the result of an AC stimulus  */
  VAMSVAR_WHITE_NOISE,          /* [FCT] White noise source                         */
  VAMSVAR_FLICKER_NOISE,        /* [FCT] Flicker noise source                       */
  VAMSVAR_NOISE_TABLE           /* [FCT] Noise table source                         */
} VarTypes;

/** this contains all the information that is needed for an AK variable
 * generation  */
typedef struct sAKVar{
  /** name of the variable */
  std::string name_;
  /** index in the global variable list*/
  int globalVarIndex_;
  /** type of the variable */
  VarTypes kind_;
  /** the same information (type of the variable) in string */
  std::string kindInString_;
  /** only in case of currents we need to store the branch index */
  int branchIndex_;

  /** additional specific information (e.g., for function calls)
   * that should be created are put into this string*/
  std::string restAKGenerate_;
} AKVar;

// ============================ AK Specific BRANCH INFO =========================
typedef enum
  {
    VAMSCONTRIB_DIRECT,      /* Direct contribution */
    VAMSCONTRIB_INDIRECT_I,  /* Indirect current contribution. */
    VAMSCONTRIB_INDIRECT_V,  /* Indirect voltage contribution. */
    VAMSCONTRIB_NONE         /* No contribution, should not occur.
                              * (when one branch is only current probed)*/
  }
  BranchContribType;

/** data structure to store all the possible branch expressions */
typedef struct sAKBranch{
  /** the smaller index of the branch */
  int brI1_;
  /** larger index of the branch */
  int brI2_;
  /** index in the evaluation list */
  int evalIndx_;
  /** index in the evaluation list for indirect contribution of the right hand side*/
  int evalIdxRhs_;
  /** integer contains flags showing which kind of possible contributions are on this branch*/
  int branchContributionsTypes_;

  /** type of the contribution, direct or indirect */
  BranchContribType branchType_;

  /** the name of the branch if this is defined by a name, if it is defined by pins then this is simply "" */
  std::string branchName_;

  /** same information as above, but in a string */
  std::string branchTypeString_;

  /** the type stored in a string, see vams_cif.h */
  std::string indirExprKind_;

  /** test if two branches are equal.
   * In the argument rev, the function returns if the ports are in the reverse order in
   * the case of match */
  bool isEqual(sAKBranch *comp, bool &rev)
  {
    // compare 1<->1  2<->2
    if ((comp->brI1_ == brI1_) && (comp->brI2_ == brI2_) && (comp->branchName_ == branchName_))
      {
        rev = false;
        return true;
      }
    // compare 1<->2  1<->2
    if ((comp->brI2_ == brI1_) && (comp->brI1_ == brI2_) && (comp->branchName_ == branchName_))
      {
        rev = true;
        return true;
      }
    // no match
    return false;
  }
} AKBranch;


// ============================ AK Specific EVALUATION INFO =========================
typedef enum
{
 VAMSEVAL_CONTRIB_I = 1,     /* Contribution I(br) <+ expr, V(br) <+ impossible. */
 VAMSEVAL_CONTRIB_V = 2,     /* Contribution V(br) <+ expr, I(br) <+ impossible */
 VAMSEVAL_CONTRIB_IV = 3,    /* Contribution I(br) <+ expr or V(br) <+ expr */
 VAMSEVAL_FCT_ARG = 8,       /* it is a function argument */
 VAMSEVAL_EQN_LHS = 16,       /* Left-hand side of the equation for an indirect contribution. */
 VAMSEVAL_EQN_RHS = 32,        /* Right-hand side of the equation for an indirect contribution. */
 VAMSEVAL_ANY = 0           /* Evaluate for any kind (used only in evaluation phase) */
} AKEvalTypes;

/** data structure to store all the evaluations through the AK interface
 * such as I(n,p) <+ ... or noise & system functions */
typedef struct sAKEvaluation
{
  /** the dependency list of this evaluation */
  CS_AKDependency dependency_;

  /** function clears an initializes the dependency list*/
  void initStruct()
  {
    branchIndex_ = -1;
    kind_ = VAMSEVAL_ANY;
  }

  /** */
  void updateStrings()
  {
    // update the string in this
    switch (kind_)
    {
      case VAMSEVAL_CONTRIB_I :  { kindInString_ = "VAMSEVAL_CONTRIB_I";  break;}
      case VAMSEVAL_CONTRIB_V :  { kindInString_ = "VAMSEVAL_CONTRIB_V";  break;}
      case VAMSEVAL_CONTRIB_IV : { kindInString_ = "VAMSEVAL_CONTRIB_IV"; break;}
      case VAMSEVAL_FCT_ARG :    { kindInString_ = "VAMSEVAL_FCT_ARG";    break;}
      case VAMSEVAL_EQN_LHS :    { kindInString_ = "VAMSEVAL_EQN_LHS";    break;}
      case VAMSEVAL_EQN_RHS :    { kindInString_ = "VAMSEVAL_EQN_RHS";    break;}
      case VAMSEVAL_ANY :        { kindInString_ = "VAMSEVAL_ANY";        break;}
    }
  }

  /** kind of the evaluation entry*/
  AKEvalTypes kind_;

  /** the kind of this eval item, stored as string */
  std::string kindInString_;

  /** only for branch contributions, otherwise it should be -1*/
  int branchIndex_;
} AKEvaluation;

// ================================== CLASS ===================================

/** In this class we dump all the AK generate specific global module informations
 *
 * This class has no LRM correspondence */
class CS_AKModuleSpecInfo
{
public:

  /** Ctor*/
  CS_AKModuleSpecInfo();

  virtual ~CS_AKModuleSpecInfo() {;}

  /** return the number of variables */
  int getNrVars() const                  { return nrVars_; }
  /** returns the AK variable structure */
  AKVar* getVar(int i)                   {return akVariables_[i]; }
  const AKVar* getVar(int i)       const { return akVariables_[i]; }
  /** adds a new uninitialized variable*/
  void addNewVar();
  /** swap variables at positions i and j*/
  void swapVariables(int i, int j);

  /** looks for a variable */
  int hasVariable(std::string varName) const;


  /** returns the number of branches */
  int getNrBranches() const                   { return nrBranches_; }
  /** returns the ak branch object */
  AKBranch* getBranch(int i)                  { return akBranches_[i]; }
  const AKBranch* getBranch(int i)    const   { return akBranches_[i]; }
  /** adds a new branch to the list of branches */
  void addNewBranch(int p1, int p2, std::string brName);
  /** tests if a branch (defined by the pin index p1 and p2) already exists,
   * if yes then it returns true and "index" is set to the index in the array
   * and reversed shows if the pins are reversed.
   * Otherwise it returns false and the mentioned arguments are not set.
   * brName is the name of the branch if we are looking for a particular branch,
   * it should be emtpy if we look for a branch defined by pins */
  bool hasBranch(int p1, int p2, std::string brName, int &index, bool &reversed);


  /** returns the number of evaluations */
  int getNrEvaluations()              const  { return nrEvaluations_; }
  /** returns the evaluation */
  AKEvaluation* getEval(int i)               { return akEvaluations_[i]; }
  const AKEvaluation* getEval(int i)  const  { return akEvaluations_[i]; }

  /** add a new initialized evaluation item */
  void addNewEval();


  /** returns the number of Event functions in the module */
  int getNrEventFunctions() const              { return nrEventFnuctions_; }
  CS_AnalogEventFunction* getEventFunc(int i)  { return akEventFunctions_[i]; }
  const CS_AnalogEventFunction* getEventFunc(int i) const { return akEventFunctions_[i]; }
  /** adds a new Event function to the list of event function */
  void addAnalogEventFunction(CS_AnalogEventFunction* newFunc) {
    akEventFunctions_.push_back(newFunc); nrEventFnuctions_++;
  }

  // --------- LIMEXP ---
  /** returns the number of limexp function calls in the module */
  int getNrLimexp() const { return nrLimExpFuncs_; }

  /** returns the limexp expression */
  CS_ExpressionBasis* getLimExpExpression(int i) { return limexpExpressions_[i]; }

  /** add a LimExp construct and returns its index */
  int addLimExp(CS_ExpressionBasis *node)
  {
    limexpExpressions_.push_back(node);
    nrLimExpFuncs_++;
    return (nrLimExpFuncs_ - 1);
  }

  // ------- total internal int and real variables ----
  // (might be needed, but actually is not used )
  int getNrInterRealVars() const { return nrRealVars_; }
  int addInterRealVar(int siz) { nrRealVars_ += siz; return (nrRealVars_ - siz); }
  int getNrInterIntVars() const { return nrIntVars_; }
  int addInterIntVar(int siz) { nrIntVars_ += siz; return (nrIntVars_ - siz); }

  // --------- for define or static (can be evaluated only once) functions ----
  // SF-> static function
  int getNrSF() const { return  nrStaticFunctions_; }
  std::string& getSFInitCode(int i) { return moduleInitializations_[i]; }
  std::string& getSFCorrespondent(int i) { return coorespondingVariable_[i]; }

  /** returns the number of monitor function calls */
  int getNrMonitorCalls() const { return nrMonitors_; }
  /** add one to the number of "monitor" calls */
  void incNrMonitorCalls() { nrMonitors_++; }

  /** returns the number of WrtCtrlCalls*/
  int getNrWrtCtrlCalls() const { return nrWrtCtrlCalls_; }
  /** add one to the number of WrtCtrlCalls*/
  void incNrWrtCtrlCalls() {
    /*std::cout << nrWrtCtrlCalls_ << "!!!\n";*/
    nrWrtCtrlCalls_++;
  }

  /** checks weather one correspondent exists , then returns this index, otherwise returns -1*/
  int hasSFCorrespondent(const std::string& corr );
  void addSF( std::string corr, std::string initCode);


  /** @returns the actual number of temporary string variables */
  int getNrTmpStrModuleVars() const { return nrTmpStrinVars_; }

  /** increment the number of temporary string variables */
  void incrementNrTmpStrModuleVars() { nrTmpStrinVars_++; }

  /** central name for these temporary string variables */
  std::string getTmpStringVarName(int i) { return std::string(VERILOG_AUX_VAR_FOR_STRING_OP) + verilog_integerToString(i); }

  /** returns the actual index for aux array variables for constant arrays in Verilog-A*/
  int getNrArrayVarsForConstArrays() const { return nrArrayVars_For_Const_Arrays_; }

  /** increment the nr. of AUX variables for const arrays*/
  void incrementNrArrayVarsForConstArrays(VarType v, int s) {
    const_aray_types_.push_back(v);
    const_aray_sizes_.push_back(s);
    nrArrayVars_For_Const_Arrays_++;
  }

  /** returns the type of the const array */
  VarType getConstArrayType(int i) const { return const_aray_types_[i]; }

  /** returns the size of the const array */
  int getConstArraySize(int i) const { return const_aray_sizes_[i]; }

  /** central name for AUX varables needed for contant arrays */
  std::string getAuxVarNameForConstArray(int i) { return std::string(VERILOG_VAR_FOR_CONST_VECTORS) + verilog_integerToString(i); }

  /** adds one table_model call of the module */
  int addTableModelCall() {
    nrTables_++;
    return (nrTables_-1);
  }
  /** returns the number of table model calls in this module */
  int getNrTableModelCalls() const { return nrTables_; }

  /** increase the nr. of Laplace operator within this module */
  void addLaplaceOperator() { nrLaplaceOperators_++; }

  /** returns the number of Laplace operator */
  int getNrLaplaceOperators() const { return nrLaplaceOperators_; }

  /// returns pointer to the DDT function of a given index
  CS_AnalogFilterOperationFunction* getDDT(int i);
  const CS_AnalogFilterOperationFunction* getDDT(int i) const;
  /// increases the number of DDT operators
  void addDDT(CS_AnalogFilterOperationFunction* ddt);
  /// returns the number of DDT opertors
  int getNrDDTs() const { return nrDDTs_; }

  /// returns pointer to the small signal function of a given index
  CS_AnalogSmallSignalFunction* getSmallSignalFunction(int i);
  const CS_AnalogSmallSignalFunction* getSmallSignalFunction(int i) const ;
  CS_BranchProbeFunction* getSmallSignalContrib(int i);
  const CS_BranchProbeFunction* getSmallSignalContrib(int i) const;
  /// adds a new noise source
  void addSmallSignalSource(CS_AnalogSmallSignalFunction *func, CS_BranchProbeFunction* branch=NULL);
  /// returns the number of noise sources
  int getNrSmallSignalSources() const { return nrNoiseSources_;}

private:

  /** list of all variables */
  std::vector<AKVar*> akVariables_;
  int nrVars_;

  /** list of branches */
  std::vector<AKBranch*> akBranches_;
  int nrBranches_;

  /** list of evaluation calls */
  std::vector<AKEvaluation*> akEvaluations_;
  int nrEvaluations_;

  /** list of event function calls */
  std::vector<CS_AnalogEventFunction*> akEventFunctions_;
  int nrEventFnuctions_;

  /** list all Limexp functions within this module */
  std::vector<CS_ExpressionBasis*> limexpExpressions_;
  /**  the total number of limexp function calls */
  int nrLimExpFuncs_;

  /** the number of integer and real variables */
  int nrRealVars_;
  int nrIntVars_;

  /** number of monitor function calls */
  int nrMonitors_;

  /** returns the number of WrtCtrl calls (all syscalls expect debug and monitor)*/
  int nrWrtCtrlCalls_;

  /** some functions calls can be done at the beginning of the module
   * such as simparam("gmin") could be evaluated only at the beginning of the module
   * then its value (double) could be reused*/
  // SF -> static functions
  std::vector<std::string>  coorespondingVariable_;
  std::vector<std::string>  moduleInitializations_;
  int nrStaticFunctions_;

  /** number of temporary string variables in the module */
  int nrTmpStrinVars_;

  /** for a constant array we need a real array in C which will hold the values
   * of this array. e.g. for laplace one might use "{1.2, myparam, 3, 4}".
   * Such an array need to be created an initialized in a C-module generated for
   * a Verilog-A module */
  int nrArrayVars_For_Const_Arrays_;
  /** in this array we gather the types of the arrays */
  std::vector<VarType> const_aray_types_;
  /** size of each such an array */
  std::vector<int>     const_aray_sizes_;

  /** here we count the number of table_model calls within this module,
   * LATER IMPROVEMENTS: for each such table 2 variables are required on the stack,
   * this might be optimized later to reuse the created variables multiple times */
  int nrTables_;

  /** nr of Laplace operator within the module */
  int nrLaplaceOperators_;

  /// list of collected DDT
  std::vector<CS_AnalogFilterOperationFunction*> ddtFunctions_;
  /// number of DDTs
  int nrDDTs_;
  /// list of collected limexps
  std::vector<CS_AnalogFilterOperationFunction*> limexpFunctions_;

  /// list of small sig functions
  std::vector<CS_AnalogSmallSignalFunction*> smalsignalFunctions_;
  /// the asociated branch contib ...
  std::vector<CS_BranchProbeFunction*>       smalsignalBranchContrib_;
  /// number of noise sources
  int nrNoiseSources_;

};

#endif /* CSAKMODULESPECINFO_HPP_ */
