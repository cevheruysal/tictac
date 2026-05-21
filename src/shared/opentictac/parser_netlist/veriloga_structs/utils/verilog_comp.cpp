/*
 * verilog_comp.cpp
 *
 *  Created on: Jan 2, 2013
 *      Author: benk
 */

#include "verilog_comp.hpp"

#include <ctime>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>

// this would require -std=c++11 or -std=gnu++11
//#include <regex>

#include "CS_ModuleDeclaration.hpp"
#include "CS_VerilogModuleCollection.hpp"
#include "CS_AnalogFunctionDeclaration.hpp"
#include "CS_VariableDeclaration.hpp"
#include "CS_ModuleDeclaration.hpp"
#include "CS_ErrorInfo.hpp"
#include "CS_ExpressionBasis.hpp"

#ifdef WITH_VERIFIC
// Verific class
#include "LineFile.h"
#endif

#define signum(VAL) ((VAL>0.0)?1:-1)

static void verilog_intern_report_fileline(int startLine, int startCol, std::string& fileName)
{
  // get that line form the file and display this error line including an arrow
  std::string STRING;
  std::ifstream infile;
  infile.open (fileName.c_str());
  std::cout << " In FILE: " << fileName << "  " << startLine << ":" << startCol << std::endl;
  int a=1;
  // get the line
  while( a <= startLine) // To get you all the lines.
  {
      getline(infile,STRING); // Saves the line in STRING.
      //std::cout << STRING << std::endl;
      a++;
  }
  // close the file
  infile.close();
  // print the line and
  std::cout << STRING << std::endl;
  for (int c = 0; c < startCol-1; c++)
    std::cout << ".";
  std::cout << "^" << std::endl;
}

void verilog_report_error(const CS_ErrorInfo* errorInf)
{
  // if the object is given then use the information from it
  if (errorInf != 0)
    {
      int startLine = errorInf->getStartLine();
      int startColumn = errorInf->getStartColumn();
      std::string fileNameWithPath = errorInf->getModule()->getModuleFile();
      // call the internal error report function
      verilog_intern_report_fileline(startLine, startColumn, fileNameWithPath);
    }
  else
    { /*std::cout << " Internal error "<< std::endl;*/ }
  // exit with a positive code
  //int *p = 0;
  //p[3] = 2;
  exit(1);
}


void verilog_report_warning(const CS_ErrorInfo* errorInf)
{
  // if the object is given then use the information from it
  if (errorInf != 0)
    {
      int startLine = errorInf->getStartLine();
      int startColumn = errorInf->getStartColumn();
      std::string fileNameWithPath = errorInf->getModule()->getModuleFile();
      //std::cout << "In line: " << startLine << ":" << startColumn << " of file: "
      //    << fileNameWithPath << std::endl;
      verilog_intern_report_fileline(startLine,startColumn,fileNameWithPath);
    }
}

#ifdef WITH_VERIFIC
void verilog_report_error_verific(const Verific::ColLineFile* errorInf)
{
  // if the object is given then use the information from it
  if (errorInf != 0)
    {
      int startLine = errorInf->GetLeftLine();
      int startColumn = errorInf->GetLeftCol();
#ifdef WINNT
      std::string fileNameWithPath = std::string(errorInf->GetFileName());
#else
      std::string fileNameWithPath = std::string(errorInf->GetAbsFileName());
#endif
      // print the file informations
      //std::cout << "In line: " << startLine << ":" << startColumn << " of file: "
      //    << fileNameWithPath << std::endl;
      // call the internal error report function
      verilog_intern_report_fileline(startLine, startColumn, fileNameWithPath);
    }
  else
    { /*std::cout << std::endl;*/ }
  // exit with a positive code
  //int *p = 0;
  //p[3] = 2;
  exit(1);
}
#endif

#ifdef WITH_VERIFIC
void verilog_report_warning_verific(const Verific::ColLineFile* errorInf)
{
  // if the object is given then use the information from it
  if (errorInf != 0)
    {
      int startLine = errorInf->GetLeftLine();
      int startColumn = errorInf->GetLeftCol();

#ifdef WINNT
      std::string fileNameWithPath = std::string(errorInf->GetFileName());
#else
      std::string fileNameWithPath = std::string(errorInf->GetAbsFileName());
#endif
      // print the file informations
      //std::cout << "In line: " << startLine << ":" << startColumn << " of file: "
      //    << fileNameWithPath << std::endl;
      // call the internal error report function
      verilog_intern_report_fileline(startLine, startColumn, fileNameWithPath);
    }
}
#endif

// this functions returns the actual time and date in a string
const std::string verilog_currentDateTime()
{
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}

bool verilog_testCompatibleTypes( VarType target, VarType source , const CS_ErrorInfo *errInf)
{
  // todo: make later a more sophisticated error output

  // simply test for compatibility, but might be extended later
  // int can be converted to double and vice versa
  if ( ((target < source) && (source > 2)) || ((target >= STRING) && (source < STRING)) ){
      // throw error, since this is incompatible
      //int *p = 0;
      //p[3] = 0;
      VERILOG_ERROR_STOP("Incompatible types in , target '" << verilog_typeToString(target)
          << "' source '" << verilog_typeToString(source) <<"'", errInf);
  }
  // issue a warning for double to int conversion
  //if ( (target < source) ) { VERILOG_WARNING(" Conversion from double to int "); }
  return true;
}

std::string verilog_doubleToString(double val)
{
  // convert double to string
  std::ostringstream s;
  s << val;
  std::string ret = std::string(s.str());
  // avoid plotting a double as an integer
  size_t dot = ret.find('.');
  size_t esmall = ret.find('e');
  size_t ebig = ret.find('E');
  // add .0 if this is not already in a double format
  if (    (dot == std::string::npos) && (esmall == std::string::npos)
       && (ebig == std::string::npos) )
    ret += ".0";
  return ret;
}

std::string verilog_integerToString(int val)
{
  // convert int to string
  std::ostringstream s;
  s << val;
  return std::string(s.str());
}

VarType verilog_getScalarType(VarType arrayType, const CS_ErrorInfo *errInf)
{
  // return the scalar type of a vector
  if ( arrayType == INTEGER_ARRAY) return INTEGER;
  if ( arrayType == REAL_ARRAY) return REAL;
  if ( arrayType == STRING_ARRAY) return STRING;
  VERILOG_ERROR_STOP("Type has no scalar type '"
      << verilog_typeToString(arrayType) << "'", errInf);
  return REAL;
}

void verilog_manipulateDisplayFunctionArgs(
    std::string &argFormat, int &nrArg, std::vector< std::string> &args,
    std::vector<const CS_ExpressionBasis*> &pArgs, int startpArgs,
    const CS_ErrorInfo &defaultErrorInfo, bool isWriteFunction )
{
  unsigned found = 0;
  int verb = 0, argIndex = 0, offs = 0;
  // start looking for % characters
  found = (int)argFormat.find("%", found);
  VERILOG_PRINT_L3( verb, "startpArgs = " << startpArgs);
  VERILOG_PRINT_L3( verb, "str:" << argFormat );
  VERILOG_PRINT_L3( verb , "first ." << found << "-" << std::string::npos);
  while (found < argFormat.size())
    {
      // the string might end with a %
      if (found + 1 >= argFormat.size()) break;

      // if %s found then the corresponding argument must be a string type
      if (argFormat[found+1] == 's') {
          VERILOG_ERROR_COND_STOP( (pArgs[argIndex-offs] == 0), " %s found but "
              "the corresponding argument not found.", &defaultErrorInfo);
          VERILOG_ERROR_COND_STOP(
             (  (pArgs[argIndex-offs]->getTypes() != STRING)) ,
              " %s found but the corresponding argument is not of a string type.",
              pArgs[argIndex-offs]);
      }

      // replace the %M or %m calls in the first string as argument
      if ( (   (argFormat[found+1] == 'm')
            || (argFormat[found+1] == 'M'))
          && isWriteFunction )
        {
          // replace the %m with %s
          argFormat[found] = '%';
          argFormat[found+1] = 's';
          // add to the array
          args.push_back(std::string(""));
          for (int i = nrArg-1; i >= argIndex; i--)
            args[i+1] = args[i];
          args[argIndex] = "VamsHierarchicalName(_c)";
          // increase the number of arguments
          nrArg++;
          offs++;
        }
      if (   (argFormat[found+1] == 'l')
          || (argFormat[found+1] == 'L') )
        {
          // we delete the %l or %L calls, and instead we insert
          argFormat[found] = 'X';
          argFormat[found+1] = 'X';
          argIndex--; // since for this there is no argument
        }
      if (   (argFormat[found+1] == 'r')
          || (argFormat[found+1] == 'R') )
        {
          // %r and %R means double numbers
          // TODO: this will now only print the number in classic format as a double
          // users would like to have it in format such as 1u (instead of 1e-6) or 1M
          argFormat[found+1] = 'e';
        }

      // in addition add always the double precision for C output
      if (   ((argFormat[found+1] == 'f') && !isWriteFunction)
          || ((argFormat[found+1] == 'e') && !isWriteFunction)
          || ((argFormat[found+1] == 'g') && !isWriteFunction)
          || (argFormat[found+1] == 'd')
          || ((argFormat[found+1] == 'F') && !isWriteFunction)
          || ((argFormat[found+1] == 'E') && !isWriteFunction)
          || ((argFormat[found+1] == 'G') && !isWriteFunction)
          || (argFormat[found+1] == 'D') )
        { // for INTEGERS only we always replace e.g. %d with %ld
          argFormat[found+1] =std::towlower( argFormat[found+1] );
          argFormat.insert( argFormat.begin()+found+1, 'l' );
        }
      // increase
      argIndex++;
      found = (int)argFormat.find("%", found+1);
      VERILOG_PRINT_L3( verb, "argIndex=" << argIndex << "  ." << found
          << "-" << std::string::npos << ".");
    }
}

std::string verilog_typeToString(VarType varType){
  std::string ret = "";
  switch (varType)
  {
    case NO_TYPE:
    case INTEGER: { ret = "integer"; break;}
    case REAL:    { ret = "real"; break;}
    case INTEGER_ARRAY: { ret = "array of integers"; break;}
    case REAL_ARRAY:    { ret = "array of reals"; break;}
    case STRING:        { ret = "string"; break;}
    case STRING_ARRAY:  { ret = "array of strings"; break;}
    default: { ret = "UNKNOWN type"; break;}
  }
  return ret;
}

int verilog_nearestInt(double val)
{
  return (int)(val + 0.5 * signum(val));
}

// ============================= methods of the classes ==================

// the static fields initializations
std::vector<std::string> UT_VerboseLevelHandle::regExps_;
std::vector<int> UT_VerboseLevelHandle::verbLevels_;
int UT_VerboseLevelHandle::nrExps_ = 0;

int UT_VerboseLevelHandle::globalVerbLevel_ = 0;

UT_VerboseLevelHandle::UT_VerboseLevelHandle()
{
  // only static fields, nothing to do here locally
}

void UT_VerboseLevelHandle::setVerbLevels(
    std::vector<std::string>& regExps,
    std::vector<int>& verbLevels)
{
  // store the lists
  regExps_ = regExps;
  verbLevels_ = verbLevels;
  nrExps_ = (int)regExps_.size();
}

void UT_VerboseLevelHandle::addVerbLevel(std::string& regExps, int verbLevel)
{
  // add a new reg expression
  regExps_.resize(nrExps_+1);
  verbLevels_.resize(nrExps_+1);
  regExps_[nrExps_] = regExps;
  verbLevels_[nrExps_] = verbLevel;
  nrExps_++;
}

int UT_VerboseLevelHandle::getVerbLevel(std::string& className)
{
  for (int i = 0; i < nrExps_; i++)
    {
      // if we have a math than
      // todo: implement also for regular expressions match, such as *Visitor*
      // we might use ... <regex>, but this needs additional informations
      if (className == regExps_[i])
        {
          //std::cout << "VERB:" << verbLevels_[i] << std::endl;
          return verbLevels_[i];
        }
    }
  // return the lowes level
  //std::cout << "VERB:" << globalVerbLevel_ << std::endl;
  return globalVerbLevel_;
}



// ----------------- Model collection -----------------
int UT_GlobalModuleAndFunctionContainer::nrGlobalFunctions_ = 0;

std::vector<CS_AnalogFunctionDeclaration*> UT_GlobalModuleAndFunctionContainer::globalFunctions_;
std::vector< std::vector<CS_VariableDeclaration*> > UT_GlobalModuleAndFunctionContainer::globalFunctionsVars_;

CS_VerilogModuleCollection* UT_GlobalModuleAndFunctionContainer::modelCollection_ = 0;

CS_VerilogModuleCollection* UT_GlobalModuleAndFunctionContainer::getModelCollection()
{
  return modelCollection_;
}

void UT_GlobalModuleAndFunctionContainer::setModelCollection(CS_VerilogModuleCollection* modelCollection)
{
  modelCollection_ = modelCollection;
}

CS_AnalogFunctionDeclaration* UT_GlobalModuleAndFunctionContainer::getGlobalFunction(int i)
{
  if (i < nrGlobalFunctions_)
    return globalFunctions_[i];
  else
    return 0;
}

void UT_GlobalModuleAndFunctionContainer::addGlobalFunction(
    CS_AnalogFunctionDeclaration* newFunc,
    std::vector<CS_VariableDeclaration*> functionVars)
{
  globalFunctions_.push_back(newFunc);
  globalFunctionsVars_.push_back(functionVars);
  nrGlobalFunctions_++;
}

int UT_GlobalModuleAndFunctionContainer::getNrGlobalFunction()
{
  return nrGlobalFunctions_;
}

int UT_GlobalModuleAndFunctionContainer::hasGlobalFunction(std::string funcName)
{
  int ret = -1;
  for (int i = 0; i < nrGlobalFunctions_; i++)
    {
      if (funcName == globalFunctions_[i]->getFunctionName())   ret = i;
    }
  return ret;
}

/** returns the variables of one global functions */
std::vector<CS_VariableDeclaration*> UT_GlobalModuleAndFunctionContainer::getGlobalFunctionVars(int i)
{
  return globalFunctionsVars_[i];
}

void UT_GlobalModuleAndFunctionContainer::addFunctionToThisModule(int globIndex, CS_ModuleDeclaration* newModule)
{
  for (const auto& elem : globalFunctionsVars_[globIndex])
    {
      //std::string varName = globalFunctionsVars_[globIndex][i]->getName();
      newModule->addVariable(elem);
    }
}


// --------------- Alias Handle ----------------------
/* all module names added (with or without aliases) */
std::vector<std::string> UT_AliasHandle::moduleNames_;

/* the alias names, if "" means no alias */
std::vector<std::string> UT_AliasHandle::moduleAliases_;

/* the total number of modules names added (with or without aliases) */
int UT_AliasHandle::nrNames_ = 0;

 /* adds one alias to the alias list*/
 void UT_AliasHandle::addModuleAlias(std::string &moduleName, std::string &aliasName)
 {
   // transform to lower case
   std::transform(aliasName.begin(), aliasName.end(),aliasName.begin(), ::tolower);
   // add to the static data structure
   moduleNames_.resize(nrNames_+1);
   moduleAliases_.resize(nrNames_+1);
   moduleNames_[nrNames_] = moduleName;
   moduleAliases_[nrNames_] = aliasName;
   nrNames_++;
 }

/* returns the alias name of one module, if this is not present then
 * returns the module name itself
 * if the flag is set then we map everything to lower case*/
std::string UT_AliasHandle::getModuleAlias(std::string &moduleName, bool toLower )
{
  std::string ret = moduleName;
  for (int i = 0; i < nrNames_ ;i++)
    {
      if (moduleName == moduleNames_[i])
        if (moduleAliases_[i].size() > 0)
          // if we found a match and there is a valid alias
          ret = moduleAliases_[i];
    }
  // if requested then transform to lower case ()
  if (toLower)
    std::transform(ret.begin(), ret.end(),ret.begin(), ::tolower);
  // returns the string
  return ret;
}

/* returns a flag if a module should be compiled or not */
bool UT_AliasHandle::doModuleCompile(const std::string &moduleName)
{
  // if no alias is given then always return true
  if ( nrNames_ < 1)
    return true;
  else
    {
      // if there is an alias list given then check if the module name is in
      // return true if yes, no otherwise
      for (int i = 0; i < nrNames_ ;i++)
        {
          if (moduleName == moduleNames_[i])
            return true;
        }
      return false;
    }
}

// ----------------- GENVAR HANDLE --------------------

std::string UT_GenVarHandle::aktualGenvarName_ = "";

double UT_GenVarHandle::actGenvarvalue_ = 0.0;

bool UT_GenVarHandle::isGenerateActive_ = false;

// ---------------- GENERAL CONFIGURATIONS --------------

bool UT_GeneralConfiguration::isInternalGenerate_ = false;

bool UT_GeneralConfiguration::doExpressionOptimization_ = false;

bool UT_GeneralConfiguration::isCMGenerate_ = false;

bool UT_GeneralConfiguration::doInsertMfactor_ = false;

bool UT_GeneralConfiguration::init_touch_first_ = false;

bool UT_GeneralConfiguration::topology_code_ = false;

std::string UT_GeneralConfiguration::mFactorNameCM_ = "M"; // default value is parameter "M"

std::string UT_GeneralConfiguration::generateTitleCM_ = "";
