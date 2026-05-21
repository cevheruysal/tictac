/*
 * IP_ExpressionParser.hpp
 *
 *  Created on: Feb 24, 2015
 *      Author: benk
 */

#ifndef IP_EXPRESSIONPARSER_HPP_
#define IP_EXPRESSIONPARSER_HPP_

#include "sim_utils.hpp"
#include "IP_NetlistExpander.hpp"
#include "IP_Token.hpp"
#include "IP_ExprConstEvaluator.hpp"

// forward declarations
class CS_ExpressionBasis;
class CS_AnalogFunctionDeclaration;
class CS_VariableDeclaration;

/** this class should return from a string an expression.
 * It should also require a netlist expansion stack such that it can create
 * concrete values!!!
 *
 * In the beginning we might have an initial dummy implementation
 * which acceps only parameters */
class IP_ExpressionParser : public IP_NetlistVisitBase
{
public:

  IP_ExpressionParser(  bool ignoreActualLevel = false);

  virtual ~IP_ExpressionParser(){;}

  /** parse an expression which is given by a token, which must be an expression
   * token */
  TICTAC_SPTR<CS_ExpressionBasis> parseExpression(const IP_Token *token,
      const ExpansionStack* stack);

  /** parse an expression which is given by a token, which must be an expression
   * token
   * TODO: this is NOT MT-SAFE !!!*/
  TICTAC_SPTR<CS_ExpressionBasis> parseExpression_new(const IP_Token *token,
      const ExpansionStack* stack);

  /** get the scaling factor, and modifies the string to a parsable real */
  static MYREAL getScalingFactor(std::string &value);
  static MYREAL getScalingFactor_old(char value);

  /** static function to get an expression for a constant value (required at some places)*/
  static TICTAC_SPTR<CS_ExpressionBasis> getExprForConstantValue(const ConstExpressionValue& constVal);

  bool hasError() const { return hasError_; }
  const std::string& getErrorMessage() const { return errorMessage_; }

  /** delete the leading and trailing spaces */
  static std::string deleteSpaces(std::string &input);

  /** here we gather all the created objects such that at the end we do not cause
   * memory leak and we can delete everything properly */
  static std::vector< TICTAC_SPTR<CS_ExpressionBasis> > garbigeCollection_;

  /** garbige collection for function calls */
  static std::vector< TICTAC_SPTR<CS_AnalogFunctionDeclaration> > garbigeCollectionFunct_;

  /** garbige collection for variable references */
  static std::vector< TICTAC_SPTR<CS_VariableDeclaration> > garbigeCollectionVarRef_;

private:

  /** internal method to report error */
  void reportError(bool cond, std::string message) {
    if (cond) {
        SIM_PRINT_L5(verb(), " ERR " << message );
        hasError_ = true;
        errorMessage_ += message + "\n";
    }
  }

  /** flag signals if there was an error */
  bool hasError_;
  /** the accumulated error message */
  std::string errorMessage_;

  /// if by parsing the actual level should be ignored
  bool ignoreActualLevel_;


};

#endif /* IP_EXPRESSIONPARSER_HPP_ */
