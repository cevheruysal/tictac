/*
 * CS_String.hpp
 *
 *  Created on: Jan 2, 2013
 *      Author: benk
 */

#ifndef CS_STRING_HPP_
#define CS_STRING_HPP_

#include "CS_ExpressionBasis.hpp"

/** this class models the string expression that are usually only specific function arguments "xyz".
 *
 * see string in LRM*/
class CS_String : public CS_ExpressionBasis
{
public:

  /** Ctor */
  CS_String(std::string &value, const CS_ModuleDeclaration* module, int startLine, int startColumn);

private:

  /** completes the list of special characters */
  static void setupSpecialChars();

public:

  /** empty Dtor*/
  virtual  ~CS_String() {;}

  virtual int getClasses() const { return (int)CONSTANT; } // we consider strings as constants

  virtual int getTypes() const { return STRING; }

  virtual double evalConst() const {
    VERILOG_PRINT_L3(verb()," StringExpr can not be evaluated ");
    VERILOG_ERROR_STOP("Internal Error", this);
    return 0.0;
  }

  /** return the string as value */
  std::string& getStringValue() { return value_; }
  const std::string& getStringValue() const { return value_; }
  /** sets the string value */
  void setStringValue(std::string newValue) { value_ = newValue; }

  /** method for the tree traversal */
  virtual void Accept(CS_CompilerVisitorBasis &visitor) { visitor.Visit( (CS_String &)(*this)); }

  /** String should not appear in composed expressions */
  virtual CS_AKDependency propagateDependencies()
    {
      // no dependency returned
      return CS_AKDependency();
    }

  /** function that creates a deep copy of the expression itself */
  virtual CS_ExpressionBasis* copy() {
    // create new object
    return (new CS_String( value_,  this->getModule(),
        this->getStartLine(), this->getStartColumn()));
  }

  /** the size of the expression, -1 means error */
  virtual int getExprSize() const { return -1;}

private:

  /** the value in the string stored */
  std::string value_;

  /** all the special characters */
  static std::vector<char> specialChars_;
  static std::vector<std::string> specialCharsString_;
};

#endif /* CS_STRING_HPP_ */
