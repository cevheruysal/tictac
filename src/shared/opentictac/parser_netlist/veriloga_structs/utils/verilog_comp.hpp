/*
 * verilog_comp.hpp
 *
 *  Created on: Nov 28, 2012
 *      Author: benk
 */

#ifndef VERILOG_COMP_HPP_
#define VERILOG_COMP_HPP_

#include <string>
#include <iomanip>
#include <locale>
#include <sstream>
#include <vector>
#include <math.h>
#include <set>
#include <map>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>

/** type of the variable */
typedef enum {
    NO_TYPE = 0, // can be both INTEGER or REAL, such as a constant "1"
    INTEGER = 1,
    REAL = 2,
    INTEGER_ARRAY = 4,
    REAL_ARRAY = 8,
    STRING = 16,
    STRING_ARRAY = 32,
    GET_STRING = 64, // flag special to get directly the string, without " from an expression
    GET_DERIVATIVE = 128, // flag to show that we generate code for the derivative
    IP_EVALUATE = 256,
    GET_LATEX = 512
} VarType;

/** *the class of the variable, these also represents bits
 * we set the enums as bits */
typedef enum {
    CONSTANT = 0,
    PARAMETER = 1,
    INTERNALVAR = 2,
    PORT = 4,
    NOISE_VAR = 8
} VariableClass;

/** port type */
typedef enum {
  PORT_INOUT_EXTERNAL = 0, /// external
  PORT_INOUT_INTERNAL = 1, /// internal pin
  PORT_GROUND = 2 /// ground pin
} PortType;

typedef enum {
  VF_FUNCVAR_UNKNOWN = -2,
  VF_FUNCVAR_IN = -1,
  VF_FUNCVAR_INOUT = 0,
  VF_FUNCVAR_OUT = 1,
  VF_FUNCVAR_LOCAL = 2
} FunctionArgumentVariableType;


#define VERILOG_TIT_INF_DBL   DBL_MAX
#define VERILOG_TIT_INT_INF   INT_MAX

#define VERILOG_MY_EPSILON 1E-90

/// our typedef of smart pointer, which in C11 we can switch to the standard
/*template<typename T>
class MY_SMPO : public boost::shared_ptr<T> {
public:
  MY_SMPO() {;};
  MY_SMPO(T *a) : boost::shared_ptr<T>(a) {;};
  MY_SMPO(const MY_SMPO &){}
  MY_SMPO & operator=(const MY_SMPO& a){return *this;}
  //virtual ~MY_SMPO() { ~boost::shared_ptr();};
};*/

#ifdef C11_SUPPORT
#include <memory>
#define MY_SMPO std::shared_ptr
#else
#include <boost/smart_ptr.hpp>
#define MY_SMPO boost::shared_ptr
#endif

// error and warning macros within a Verilog Compiler struct
#define VERILOG_ERROR_STOP(X,Z) { std::cout << "\nERROR: " << X << std::endl;\
  verilog_report_error(Z); }
#define VERILOG_ERROR_COND_STOP(Y,X,Z) { if (Y) {std::cout << "\nERROR: " << X << std::endl;\
  verilog_report_error(Z); } }
#define VERILOG_WARNING(X,Y) { std::cout<< "\nWARNING: " << X << std::endl; \
  verilog_report_warning(Y); }

// error and warning macros within Verific data structure context
#ifdef WITH_VERIFIC
#define VERILOG_ERROR_STOP1(X,Z) { std::cout << "\nERROR: " << X << std::endl;\
    verilog_report_error_verific(Z); }
#define VERILOG_ERROR_COND_STOP1(Y,X,Z) { if (Y) {std::cout << "\nERROR: " << X << std::endl;\
    verilog_report_error_verific(Z); } }
#define VERILOG_WARNING1(X,Z) { std::cout<< "\nWARNING: " << X << std::endl; \
  verilog_report_warning_verific(Z); }
#endif

#define VERILOG_PRINT_L0(L,X) if (L > 0) { std::cout << X << std::endl; }
#define VERILOG_PRINT_L1(L,X) if (L > 1) { std::cout << X << std::endl; }
#define VERILOG_PRINT_L2(L,X) if (L > 2) { std::cout << X << std::endl; }
#define VERILOG_PRINT_L3(L,X) if (L > 3) { std::cout << X << std::endl; }
#define VERILOG_PRINT_L4(L,X) if (L > 4) { std::cout << X << std::endl; }
#define VERILOG_PRINT_L5(L,X) if (L > 5) { std::cout << X << std::endl; }
#define VERILOG_PRINT_L6(L,X) if (L > 6) { std::cout << X << std::endl; }

// defines to avoid that a variable name is a C key word we use these prefixes
#define VERILOG_VAR_PREFIX "v_"
#define VERILOG_VAR_BACKUP_PREFIX "vb_"
#define VERILOG_PAR_PREFIX "p_"
#define VERILOG_PARAMDESC_PREFIX "pd_"
#define VERILOG_OUTPARAMDESC_PREFIX "pdo_"
#define VERILOG_FUNCTION_PREFIX "f_"
#define VERILOG_OPT_VAR_PREFIX "tp_"
#define VERILOG_FUNCTION_AUXILIAR_VAR_PREFIX "aux_f_"
#define VERILOG_OUTPUT_ARRAY_VAR_PREFIX "__ARRAY_TTN_OUTVAR__"
#define VERILOG_AUX_VAR_FOR_STRING_OP "__tmpTN_stringVars_"
#define VERILOG_FUNCTION_EXPANDED_VAR_INT "__funcAUX__i_"
#define VERILOG_FUNCTION_EXPANDED_VAR_DOUBLE "__funcAUX__d_"
// for constant arrays
#define VERILOG_VAR_FOR_CONST_VECTORS "__const_arrays_"
// for limexp
#define VERILOG_VAR_LIMEXP "_tmp_limexp_"
#define VERILOG_DVAR_LIMEXP "_d_tmp_limexp_"
// for table models
#define VERILOG_VAR_RES_TABLE "_tmp_table_"
#define VERILOG_VAR_COEF_TABLE "_tmp_table_coef_"


// ==================== UTILITY FUNCTOINS ==============
// forward declaration of the error information class
class CS_ErrorInfo;

/** function to report an error and terminate the run */
void verilog_report_error(const CS_ErrorInfo* errorInf);

/** function to report a warning within a file */
void verilog_report_warning(const CS_ErrorInfo* errorInf);

// forward declaration of the Verific class that contains the informations needed for errors
namespace Verific{
  class ColLineFile;
}
/** function to report an error and terminate the run */
void verilog_report_error_verific(const Verific::ColLineFile* errorInf);

/** function to report warnings */
void verilog_report_warning_verific(const Verific::ColLineFile* errorInf);

/** the function that returns the date and time in a standard format */
const std::string verilog_currentDateTime();

/** converts one double to a string*/
std::string verilog_doubleToString(double val);

/** converts one integer to a string*/
std::string verilog_integerToString(int val);

/** rounding function from double to the nearest int*/
int verilog_nearestInt(double val);

/** test if these two types are compatible within one expression */
bool verilog_testCompatibleTypes( VarType target, VarType source, const CS_ErrorInfo *errInf);

/** returns the scalar type of a given array type*/
VarType verilog_getScalarType(VarType arrayType, const CS_ErrorInfo *errInf);

// forward declaration
class CS_ExpressionBasis;

/** this function is meant to manipulate the output argument of the system
 * display's functions, by replacing, e.g., %m with %s and one additional
 * argument (the corresponding function call) in the argument list
 * @param argFormat string format (which contains %s %)
 * @param nrArg number of arguments for the output (this might increase when there is %m)
 * @param args vector containing the codes of each argument
 * @param pArgs pointer to the expressions of the output arguments
 * @param startArgs (not used) ()
 * @param defaultErrorInfo pointer to an object which always can be used
 *                         as a fallback to error output
 * @param isWriteFunction if the arguments are read and not written, especially
 * for strings we need to make extra operations */
void verilog_manipulateDisplayFunctionArgs(std::string &argFormat, int &nrArg,
    std::vector< std::string> &args,
    std::vector<const CS_ExpressionBasis*> &pArgs, int startpArgs,
    const CS_ErrorInfo &defaultErrorInfo,
    bool isWriteFunction = true);

/** this functions returns a string according to the input argument.
 * It is mostly used for message printing */
std::string verilog_typeToString(VarType varType);




// ========================= Utility classes ============================

/** class that specifies which class has which verbose level (for debug info).
 * Internally this class has a list of regular expressions and the corresponding
 * verbose levels. If the called string matches one regular expression than
 * it returns the verbose level provided by the user. */
class UT_VerboseLevelHandle {
public:
  /** Ctor to create the handle. The inputs are
   * a vector of regular expressions and the associated verbose levels */
  UT_VerboseLevelHandle();

  virtual ~UT_VerboseLevelHandle() {;}

  /** sets the vectors of verbosity with the given arrays of */
  static void setVerbLevels(
      std::vector<std::string>& regExps,
      std::vector<int>& verbLevels);

  /** adds one verbose level */
  static void addVerbLevel(std::string& regExps, int verbLevel);

  /** returns the associated verbose level corresponding the name.
   * If this string does not match any of the regular expression than the
   * function returns the lowest level "0". */
  static int getVerbLevel( std::string& className );

  /** sets the global verbose level (for all classes)*/
  static void setGlobalVerbose(int v) { globalVerbLevel_ = v; }

  /** return the global verbosity */
  static int getGlobalVerbose() { return globalVerbLevel_; }

private:
  /** vector that contains the regular expression */
  static std::vector<std::string> regExps_;

  /** array containing the associated verbose levels */
  static std::vector<int> verbLevels_;

  /** number of regular expressions */
  static int nrExps_;

  /** for general compiler verbosity we use this global integer */
  static int globalVerbLevel_;
};


/** class that only contains the verbosity level, that might be needed for structured outputting */
class UT_VerboseClass {
public:
  /** empy Ctor */
  UT_VerboseClass(std::string className)
  { // set the verbosity level according to the debug information
    verb_ = UT_VerboseLevelHandle::getVerbLevel(className);
  }

  virtual ~UT_VerboseClass() {;}

  int verb() const { return verb_; }
  void setVerb(int v) { verb_ = v; }
private:
  volatile int verb_;
};



// forward declaration of the required classe
class CS_VerilogModuleCollection;
class CS_AnalogFunctionDeclaration;
class CS_VariableDeclaration;
class CS_ModuleDeclaration;
/** class to store all the actual compiled Verilog modules and global functions
 * needed for the hierarchical model instantiation */
class UT_GlobalModuleAndFunctionContainer {
public:
  /** empy Ctor */
  UT_GlobalModuleAndFunctionContainer() {;}

  /** returns the actual model container */
  static CS_VerilogModuleCollection* getModelCollection();

  /** sets the actual model container */
  static void setModelCollection(CS_VerilogModuleCollection* modelCollection);

  /** returns the global function at the given position */
  static CS_AnalogFunctionDeclaration* getGlobalFunction(int i);

  /** returns the variables of one global functions */
  static std::vector<CS_VariableDeclaration*> getGlobalFunctionVars(int i);

  static void addFunctionToThisModule(int globIndex, CS_ModuleDeclaration* newModule);

  /** adds one function declaration */
  static void addGlobalFunction(CS_AnalogFunctionDeclaration* newFunc,
      std::vector<CS_VariableDeclaration*> functionVars);

  /** returns the number of global functions */
  static int getNrGlobalFunction();

  /** returns the index of the function of a function with "funcName" exists in the
   * global list, otherwise returns -1 */
  static int hasGlobalFunction(std::string funcName);

private:
  /** contains all the actual models */
  static CS_VerilogModuleCollection* modelCollection_;

  /** number of global functions */
  static int nrGlobalFunctions_;

  /** container of the global functions */
  static std::vector<CS_AnalogFunctionDeclaration*> globalFunctions_;

  /** container of the variables of each function */
  static std::vector< std::vector<CS_VariableDeclaration*> > globalFunctionsVars_;
};



/** this class handles the aliases by its static functions
 * (these are the -m options in the compiler options )*/
class UT_AliasHandle {
public:
  /** empy Ctor */
  UT_AliasHandle() {;}

  /** adds one alias to the alias list*/
  static void addModuleAlias(std::string &moduleName, std::string &aliasName);

  /** returns the alias name of one module, if this is not present then
   * returns the module name itself
   * if the flag is set then we map everything to lower case*/
  static std::string getModuleAlias(std::string &moduleName, bool toLower = true );

  /** returns a flag if a module should be compiled or not */
  static bool doModuleCompile(const std::string &moduleName);

private:

  /** all module names added (with or without aliases) */
  static std::vector<std::string> moduleNames_;

  /** the alias names, if "" means no alias */
  static std::vector<std::string> moduleAliases_;

  /** the total number of modules names added (with or without aliases) */
  static int nrNames_;

};

// ------------- GENERATE -------------
/** class with static methods to handle the generate loop's expansions.
 * This class stores statically the value and the name of the genvar variable
 * during the expansion of the for loop, such that in the expansion the genvar
 * can be directly replaced by its value. This is crucial for the arrays of
 * pins/ports.
 * The static methods serve this purpose. */
class UT_GenVarHandle {
public:
  /** empy Ctor */
  UT_GenVarHandle() {;}

  /** returns true if the input name matches the actual variable name */
  static bool isActGenvarName(const std::string& variableName) { return (variableName == aktualGenvarName_); }

  /** sets the genvar name*/
  static void setActGenvarName(std::string variableName) { aktualGenvarName_ = variableName; }

  /** sets the actual value of genvar */
  static void setActGenvarValue(double value) { actGenvarvalue_ = value; }

  /** returns the actual value of genvar */
  static double getActGenvarValue() { return actGenvarvalue_; }

  /** returns the flag which shows if this mechanism is active */
  static bool isGenvarActive() { return isGenerateActive_; }

  /** sets the activation flag for the genvar expansion active*/
  static void setGenvarActive(bool newVal) { isGenerateActive_ = newVal; }

private:

  /** the name of the actual genvar variable */
  static std::string aktualGenvarName_;

  /** the actual integer value of the genvar */
  static double actGenvarvalue_;

  /** flag to show if this mechanism is active */
  static bool isGenerateActive_;

};


/** This class contains all the general configurations for the generation
 * process. These configurations are usually set on the cmd line */
class UT_GeneralConfiguration {
public:
  /** empy Ctor */
  UT_GeneralConfiguration() {;}

  /** @returns false when the VAC is used in a classic context by the user.
   * if returns true than the compiler is used for TITAN internal purposes */
  static bool isInternalGenerate() { return isInternalGenerate_; }

  /** sets the isInternalGenerate flag (default is false)*/
  static void setIsInternalGenerate(bool b) { isInternalGenerate_ = b; }

  /** @returns true if expressions should be optimized */
  static bool doExprOptim() { return doExpressionOptimization_; }

  /** turns on or off globaly the expression optimization */
  static void setExprOptim(bool b) { doExpressionOptimization_ = b; }

  /** @returns true when the VAC is used for compact model code generation,
   * false otherwise */
  static bool isCMGenerate() { return isCMGenerate_; }

  /** sets the isCMGenerate_ flag (default is false)*/
  static void setCMGenerate(bool b) { isCMGenerate_ = b; }

  /** for CM code generation one might choose to insert M factor into the
   * formula or not */
  static bool doInsertMfactorCM() { return doInsertMfactor_; }

  /** sets the flag to automatically insert the Mfactor into all computations
   * when CM code is generated */
  static void setdoInsertMfactorCM(bool a) { doInsertMfactor_ = a; }

  /** for CM code generation it returns the name of the Mfactor parameter,
   * by default this is "M" but could be others */
  static std::string getMfactorNameCM() { return mFactorNameCM_; }

  /** sets the parameter name of the Mfactor for CM code generation */
  static void setMfactorNameCM(std::string a) { mFactorNameCM_ = a; }

  /** returns by the user specified title of the CM C-generate, "" if not specified */
  static std::string getGenerateTitleCM() { return generateTitleCM_; }

  /** sets the title of the C-code if the users specifies it  */
  static void setGenerateTitleCM(std::string a) { generateTitleCM_ = a; }

  /** usually variables in the initialization section are set and never set during
   * evaluation (they are only used during evaluation)
   * but in some models such variables can be set during evaluation
   * and the question is how to separate between initialization of the model
   * evaluation. If this flag is set then we consider that the frist instruction
   * where a voltage is accessed is considered as the end of the initialization
   * section and all the coming instructions are in the evaluation routine.
   * This also means that all the instructions (variable assignments) before the
   * first voltage access are part of the device initialization and those assigned
   * Variables need to be stored in LOADPAR and might or have any derivative
   * tracing */
  static bool get_init_touch_first() { return init_touch_first_; }

  /** see function above */
  static void set_init_touch_first(bool a) { init_touch_first_ = a; }

  /** in some weird compact models the topology is not dependent just on the
   * instance parameterization but also on some internal variables which are
   * set through some logic based on the instance parameters. If this is the
   * case the topology initialization routine needs to compute these variables.
   * If this is required then we need to signal this by setting this flag to true.
   * This NEEDs to be set by the user! */
  static bool get_topology_code() { return topology_code_; }

  /** see function above*/
  static void set_topology_code(bool a) { topology_code_ = a; }

private:

  /** this flag shows if we generate TITAN internal purposes */
  static bool isInternalGenerate_;

  /** if this flag is true then we use the expression "twin" information
   * to optimize code (evaluate only once intermediate results and store them
   * in temporary variables)*/
  static bool doExpressionOptimization_;

  /** this flag shows if the compiler is used for compact model code generation
   * fir TITAN internal purposes */
  static bool isCMGenerate_;

  /** in the case of the CM code generation one might decide to insert m factor
   * implicitly into the computation */
  static bool doInsertMfactor_;

  /** true when for CMS we should adjust the initialization of the initialization
   * code such that all the statements before any voltage access should be part
   * of the initializations */
  static bool init_touch_first_;

  /** in some cases we need Loadpar initializations within the check the topology
   * but then ldpar is not initialized */
  static bool topology_code_;

  /** for CM code generation we need the name of the Mfactor parameter,
   * by default this is "M" but could be others */
  static std::string mFactorNameCM_;

  /** only for the CM generate the user can specify a different title,
   * otherwise this should be empty */
  static std::string generateTitleCM_;

};

#endif /* VERILOG_COMP_HPP_ */
