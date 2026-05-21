/*
 * CS_VariableDeclaration.hpp
 *
 *  Created on: Nov 30, 2012
 *      Author: benk
 */

#ifndef CS_VARIABLEDECLARATION_HPP_
#define CS_VARIABLEDECLARATION_HPP_

#include "verilog_comp.hpp"
#include "CS_Identifier.hpp"
#include "CS_ModuleDeclaration.hpp"
#include "CS_Range.hpp"
#include "CS_CompilerTreeNode.hpp"
#include "CS_ErrorInfo.hpp"

/** enum type required for the various variables which are
 * inserted later in the code (mostly for the function expansions)*/
typedef enum {
  USER_DEFINED = 0,
  FUNCTION_VARIABLE = 1,
  AUX_FUNCTION_VARIABLE = 2
}InternalVarType;

/** class for integer and real variable declarations
 *
 * see integer_declaration and real_declaration in LRM */
class CS_VariableDeclaration : public CS_Identifier, public CS_CompilerTreeNode, public CS_ErrorInfo
{
public:

  CS_VariableDeclaration(const CS_ModuleDeclaration* module,
      int startLine, int startColumn,
      std::string name,
      int varIndex, CS_Range* range = 0, CS_ExpressionBasis* initValue = 0,
      VarType type = INTEGER,
      bool isGenerate = false,
      InternalVarType variableType = USER_DEFINED);

  virtual ~CS_VariableDeclaration() {;}

  /** returns the size of the variable (1 -> scalar, 1< array variable)*/
  virtual VarType getType() const { return type_; }

  /** returns the string of the actual entity (e.g., the corresponding C variable of a Verilog parameter)*/
  virtual std::string getGenerate([[maybe_unused]] const VarType resType) const {
    return std::string("vars."+name_);
  }

  /** we have one variable */
  virtual VariableClass getClass() const {  return INTERNALVAR;  }

  /** returns the name of the variable */
  virtual const std::string& getName() const { return name_; }

  /** returns the size of the variable */
  virtual int getSize() const { return size_; }

  /** returns the offset of an array that is specified in the declaration, */
  virtual int getOffset() const { return offset_; };

  /** this function shows if the pins are numbered in reverse order */
  virtual bool isInReversedOrder() const { return isReverseNumbered_; }

  /** returns the initial value's expression */
  CS_ExpressionBasis* getInitialExpr() { return initValue_;}
  const CS_ExpressionBasis* getInitialExpr() const { return initValue_;}

  /** returns the range object */
  CS_Range* getRangeObj() { return range_; }
  const CS_Range* getRangeObj() const { return range_; }

  /** method for the tree traversal */
  virtual void Accept(CS_CompilerVisitorBasis &visitor) { visitor.Visit( (CS_VariableDeclaration &)(*this)); }

  /** returns the dependency if this is variable is scalar*/
  CS_AKDependency& getVariableScalarDependency() { return actVarDependency_; }
  const CS_AKDependency& getVariableScalarDependency() const { return actVarDependency_; }

  /** dependency of the variable if this variable is declared as an array */
  CS_AKDependency& getVariableArrayDependency(int i) { return actArrayVarDependency_[i]; }
  const CS_AKDependency& getVariableArrayDependency(int i) const { return actArrayVarDependency_[i]; }

  /** returned flag shows if this variable is declared as an array
   * (arrays can also have a size of one )*/
  virtual bool isArray() const { return isArray_; }

  /** returns the name in the AK C code of the derivative of this variable, this should only used in
   * a static context, to generate the variable declaration, but not in a dynamic context */
  std::string getStaticDerivName(int sysInd) const
  {
    std::string ret = "";
    switch (variableType_) {
    case USER_DEFINED: {
      ret = (!isArray_)?("_d_" + std::string(VERILOG_VAR_PREFIX)
        + name_ + "_" + verilog_integerToString(sysInd))
        :("_d_" + std::string(VERILOG_VAR_PREFIX) + name_);
      break;
    }
    case FUNCTION_VARIABLE: {
      ret = (!isArray_)?("_d_" + std::string(VERILOG_FUNCTION_PREFIX)
        + name_ + "_" + verilog_integerToString(sysInd))
        :("_d_" + std::string(VERILOG_FUNCTION_PREFIX) + name_);
      break;
    }
    case AUX_FUNCTION_VARIABLE: {
      ret = (!isArray_)?("_d_" + std::string(VERILOG_FUNCTION_AUXILIAR_VAR_PREFIX)
                + name_ + "_" + verilog_integerToString(sysInd))
                :("_d_" + std::string(VERILOG_FUNCTION_AUXILIAR_VAR_PREFIX) + name_);
      break;
    }
    default: { ret = ""; break;}
    }
    return ret;
   }

  /** @returns the prefix for all variables which need to backuped */
  std::string getVarAkBackupPrefix() const {
    return std::string(VERILOG_VAR_BACKUP_PREFIX);
  }

  /** in the generated C code we use different prefixes to distinguish
   * the user defined variables and the artificially generated variables
   * (in order to avoid names conflicts)*/
  std::string getVarAkPrefix() const {
    std::string ret = "";
    switch (variableType_) {
    case USER_DEFINED:          { ret = std::string(VERILOG_VAR_PREFIX); break; }
    case FUNCTION_VARIABLE:     { ret = std::string(VERILOG_FUNCTION_PREFIX); break;    }
    case AUX_FUNCTION_VARIABLE: { ret = std::string(VERILOG_FUNCTION_AUXILIAR_VAR_PREFIX); break; }
    default: { ret = ""; break;}
    }
    return ret;
  }

  /** returns true if a variable should be output as a parameter*/
  bool isVariableOutput(CS_ExpressionBasis **desc, CS_ExpressionBasis **unit, int &varSize) const;

  /** returns the module */
  const CS_ModuleDeclaration* getModule() const { return module_;}

  /** line nr in the Verilog file*/
  int getStartLine() const { return startLine_; }

  /** column number in the Verilog file */
  int getStartColumn() const { return startColumn_; }

  /** if this variable is declared as generate (important for the generate constructs) */
  bool isVariableGenerate() const { return isGenerateVariable_; }

  /** returns the given variable index in the module (variables are numbered) */
  int getVaribleIndex() const { return varIndex_; }

  /** returns true if this vairble in any possible condition is consumed before is produces */
  bool getIsVariableConsumedBeforeProd() const { return isVariableConsumedBeforeProd_; }

  /** sets the flag isVariableConsumedBeforeProd_ */
  void setIsVariableConsumedBeforeProd(bool b) { isVariableConsumedBeforeProd_ = b; }

  /** only for internal purposes the categorization: user definer, or was artifilicaaly introduced for
   * function expansions */
  InternalVarType getInternalVariableType() const { return variableType_; }

private:

  /** module containing the variable */
  const CS_ModuleDeclaration* module_;

  /** start line of the expression */
  const int startLine_;

  /** start column */
  const int startColumn_;

  /** if it is an array than this is not null*/
  CS_Range* range_;

  /** expression containing the initial value, otherwise this is null*/
  CS_ExpressionBasis* initValue_;

  /** type of this variable */
  VarType type_;

  /** name of the internal variable */
  std::string name_;

  /** index of the variable within the Verilog module */
  int varIndex_;

  /** 1 if is scalar otherwise it is the size of the array*/
  int size_;

  /** flag to show if this is numbered inversely */
  bool isReverseNumbered_;

  /** flag shows if this variable is defined as an array */
  bool isArray_;

  /** offset of the variable if it is an array, otherwise it is 0*/
  int offset_;

  /** this flag is true if this variable is a generate variable */
  bool isGenerateVariable_;

  /** this flag shows if this variable is consumed/used before it is produced/set
   * if this is true then we might need to save the value before evaluation
   * and restore if this is not the final step in the TR time step or DC ...*/
  bool isVariableConsumedBeforeProd_;

  /** in case of functions we need to add them (and their local variables) as
   * variables of the module (in order to have transparent derivative handling)
   * In order to avoid naming conflicts these should get different naming prefixes
   * therefore these variables should be marked */
  InternalVarType variableType_;

  // these variables contain the actual dependencies of the variable, these fields are used
  // during the date elaboration phase to find out the dependencies between inputs and
  // contributions

  /** contains the (actual) variable dependencies of this variable */
  CS_AKDependency actVarDependency_;

  /** in case of vector variable we should trace each scalar dependency separately
   * therefore for each scalar item we store the actual dependencies */
  std::vector< CS_AKDependency > actArrayVarDependency_;

  /** this is the default unit for output variables which have no "units"
   * defined */
  static CS_ExpressionBasis* noUNit_;

};

#endif /* CS_VARIABLEDECLARATION_HPP_ */
