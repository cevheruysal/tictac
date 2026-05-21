/*
 * IP_ExpressionParser.cpp
 *
 *  Created on: Feb 24, 2015
 *      Author: benk
 */

#include "IP_ExpressionParser.hpp"
#include "CS_Number.hpp"
#include "CS_String.hpp"
#include "CS_ParameterReference.hpp"
#include "CS_AnalogConcatenation.hpp"
#include "CS_UnaryOperator.hpp"

#include "IP_SpiceExpression_grammar.hpp"

#include "CS_Number.hpp"
#include "CS_BinaryOperator.hpp"
#include "CS_AnalogBuiltInMathFunction.hpp"
#include "CS_UnaryOperator.hpp"
#include "CS_AnalogConditionalExpression.hpp"
#include "CS_AnalogFunctionDeclaration.hpp"
#include "CS_AnalogFunctionCall.hpp"
#include "CS_VariableDeclaration.hpp"
#include "CS_VariableReference.hpp"

#include <iostream>
#include <stack>
#include <functional>
#include <string>
#include <cassert>
#include <fstream>
#include <sstream>

#include  <boost/algorithm/string/trim.hpp>
#include  <boost/algorithm/string/predicate.hpp>
#include  <boost/lexical_cast.hpp>

using namespace tictac;
using namespace BOOST_SPIRIT_CLASSIC_NS;

typedef char const* iterator_t;
typedef tree_match<iterator_t> parse_tree_match_t;
typedef parse_tree_match_t::tree_iterator iter_t;

#define verbosity 0
#define old_parser 0
#define error_verbose 0

std::vector< TICTAC_SPTR<CS_ExpressionBasis> > IP_ExpressionParser::garbigeCollection_;

std::vector< TICTAC_SPTR<CS_AnalogFunctionDeclaration> > IP_ExpressionParser::garbigeCollectionFunct_;

std::vector< TICTAC_SPTR<CS_VariableDeclaration> > IP_ExpressionParser::garbigeCollectionVarRef_;


IP_ExpressionParser::IP_ExpressionParser( bool ignoreActualLevel )
{
  hasError_ = false;
  errorMessage_ = "";
  ignoreActualLevel_ = ignoreActualLevel;
  //this->setVerb(7);
}

TICTAC_SPTR<CS_ExpressionBasis> IP_ExpressionParser::getExprForConstantValue(const ConstExpressionValue& constVal)
{
  // TODO: for error reporting this is nor optimal
  switch (constVal.valueType_){
  case REAL: {
    auto tmp = TICTAC_SPTR<CS_ExpressionBasis>(new CS_Number( constVal.dval_,0, 0, 0));
    garbigeCollection_.push_back(tmp);
    return tmp;
    break;
  }
  case REAL_ARRAY: {
    CS_AnalogConcatenation* ret = new CS_AnalogConcatenation(0,0,0);
    for (MYINT i = 0; i < (MYINT)constVal.aval_.size(); i++){
        // TODO: memoty LEAK
        CS_ExpressionBasis* tmp = new CS_Number( constVal.aval_[i],0, 0, 0);
        auto tmps = TICTAC_SPTR<CS_ExpressionBasis>(tmp);
        garbigeCollection_.push_back(tmps);
        ret->addExpr( tmp );
    }
    auto tmp = TICTAC_SPTR<CS_ExpressionBasis>(ret);
    garbigeCollection_.push_back(tmp);
    return tmp;
    break;
  }
  case STRING: {
    std::string val = constVal.sval_;
    auto tmp = TICTAC_SPTR<CS_ExpressionBasis>(new CS_String( val,0, 0, 0));
    garbigeCollection_.push_back(tmp);
    return tmp;
    break;
  }
  case STRING_ARRAY: {
    CS_AnalogConcatenation* ret = new CS_AnalogConcatenation(0,0,0);
    for (MYINT i = 0; i < (MYINT)constVal.aval_.size(); i++){
        std::string val =  constVal.saval_[i];
        CS_ExpressionBasis* tmp = new CS_String( val,0, 0, 0);
        auto tmps = TICTAC_SPTR<CS_ExpressionBasis>(tmp);
        garbigeCollection_.push_back(tmps);
        ret->addExpr( tmp );
    }
    auto tmp = TICTAC_SPTR<CS_ExpressionBasis>(ret);
    garbigeCollection_.push_back(tmp);
    return tmp;
    break;
  }
  default:
    //TODO: no error? reportError(true, "IP_ExpressionParser::getExprForConstantValue error");
    return TICTAC_SPTR<CS_ExpressionBasis>(NULL);
  }

}

TICTAC_SPTR<CS_ExpressionBasis> IP_ExpressionParser::parseExpression(
    const IP_Token *token, const ExpansionStack* stack)
{
  hasError_ = false;
  errorMessage_ = "";

  if (!old_parser)
    return parseExpression_new( token, stack);

  MYREAL signFactor = 1.0;

  // test if this token is an expression token
  //reportError((token->getTokenType() != TK_EXPRESSION),
  //    "IP_ExpressionParser::parseExpression was called with a non-expression token");

  // get the string from the token
  std::string exprStr = token->getValue();
  if (verbosity) std::cout << "INPUT expr=" <<exprStr << "\n";
  // test the '-' in the front of the ID or number
  if (exprStr.front() == '-') {
      exprStr.erase(0,1);
      signFactor = -1.0;
  }

  if ( isdigit ( exprStr.front() ) ) {
      // this must be a number
      MYREAL factor = 1.0, value; // the faktor for
      if ( !isdigit ( exprStr.back() ))
        {
          factor = getScalingFactor_old( exprStr.back() );
          exprStr.pop_back(); // delete last character
        }
      // compute the value
      value = ((MYREAL)atof(exprStr.c_str())) * factor * signFactor;
      auto tmp = TICTAC_SPTR<CS_ExpressionBasis>(
                new  CS_Number(value, NULL, token->getRowIndex(), token->getColumnIndex()) );
      if (verbosity) std::cout << "E expr=" <<exprStr << " value=" << value << " factor=" << factor << "\n";
      garbigeCollection_.push_back(tmp);
      return tmp;
  }
  else
    {
      // this might also be a string , the it must start with " and end with "
      if (exprStr.front() == '"')
        {
          exprStr.pop_back(); // delete the trailing ", since this must be there for STRING !!!
          auto tmp = TICTAC_SPTR<CS_ExpressionBasis>(
              new  CS_String(exprStr, NULL, token->getRowIndex(), token->getColumnIndex()) );
          garbigeCollection_.push_back(tmp);
          if (verbosity) std::cout << "E expr=" <<exprStr << " string \n";
          return tmp;
        }
      // we use case insensitive IDs here
      std::string id = sim_toLowerString( exprStr );
      bool hasInitialValue = false;
      bool paramFound = stack->hasParameter( id, hasInitialValue, ignoreActualLevel_);

      if (verbosity)
        std::cout << "paramFound=" << paramFound << " varName=" << id
                  << " hasInitialValue=" << hasInitialValue
                  << " ignoreActualLevel_=" << ignoreActualLevel_ << "\n";
      // if this parameter not found than throw error
      reportError( !paramFound, "Parameter (IP_ExpressionParser::parseExpression) not found " + id);

      // if we had error than reutrn NULL
      if (this->hasError_)
        return TICTAC_SPTR<CS_ExpressionBasis>(NULL);

      // just either return the initial Value, if found or if not than return the parameter reference
      TICTAC_SPTR<CS_ExpressionBasis> minExpr;
      if (hasInitialValue) {
          minExpr = stack->getParameterInitialValue( id , ignoreActualLevel_);
          if (verbosity) std::cout << "E expr=" <<exprStr << " init param value \n";
      }
      else {
          minExpr = TICTAC_SPTR<CS_ExpressionBasis>(
              new CS_ParameterReference( (stack->getParameterReference(id, ignoreActualLevel_)).get(), NULL,
                  (const CS_ModuleDeclaration*)token->getFileNameDummyMod(), token->getRowIndex(), token->getColumnIndex()) );
          garbigeCollection_.push_back(minExpr);
          if (verbosity) std::cout << "E expr=" <<exprStr << " param \n";
      }
      if (signFactor < 0.0) {
        minExpr = TICTAC_SPTR<CS_ExpressionBasis>(new CS_UnaryOperator( VERILOG_UNAR_MIN, minExpr.get(),
            (const CS_ModuleDeclaration*)token->getFileNameDummyMod(), token->getRowIndex(), token->getColumnIndex()) );
        garbigeCollection_.push_back(minExpr);
        if (verbosity) std::cout << "E expr=" <<exprStr << " param -1\n";
      }
      return minExpr;
  }
  // here at the moment we only accept either numbers or IDs
}

// TODO: LATER THIS SHOULD CONTAIN An EXPRESSION PARSER !!!!
// TODO: right now this is only capable of referencing other parameters or constant values

MYREAL IP_ExpressionParser::getScalingFactor_old(char unit)
{
  MYREAL fact = 1.0;
  switch (unit) {
  case 'T': { fact = 1.0E+12; break;}
  case 'G': { fact = 1.0E+09; break;}
  case 'M': { fact = 1.0E+06; break;}
  case 'K':
  case 'k': { fact = 1.0E+03; break;}
  case 'm': { fact = 1.0E-03; break;}
  case 'u': { fact = 1.0E-06; break;}
  case 'n': { fact = 1.0E-09; break;}
  case 'p': { fact = 1.0E-12; break;}
  case 'f': { fact = 1.0E-16; break;}
  // default no scaling factor
  default : { fact = 1.0E+00; break;}
  }
  return fact;
}


MYREAL IP_ExpressionParser::getScalingFactor(std::string &value)
{
  char unit = value.back();
  MYREAL fact = 1.0;

  // --------------- THREAT THE UNITS --------------
  // TODO: if we have units then delete them
  if (unit == 'V' || unit == 'v' || unit == 'F' )
    {
      value.pop_back();
      unit = value.back();
    }

  // --------------- THREAT THE SCALING FACTOR --------------
  if (verbosity) std::cout << " value=" << value << " ";
  // Test "Meg" at the end of the constant
  if (value.size() > 3)
    {
      if (boost::algorithm::ends_with(value, "Meg")
          || boost::algorithm::ends_with(value, "MEG")
          || boost::algorithm::ends_with(value, "meg"))
        {
          fact = 1.0E+06;
          value.pop_back();
          value.pop_back();
          value.pop_back();
          if (verbosity) std::cout << " fact=" << fact << "\n";
          return fact;
        }
    }

  switch (unit)
    {
  case 'T': //case 't':
    {
      fact = 1.0E+12;
      value.pop_back();
      break;
    }
  case 'G': //case 'g':
    {
      fact = 1.0E+09;
      value.pop_back();
      break;
    }
  case 'K':
  case 'k':
    {
      fact = 1.0E+03;
      value.pop_back();
      break;
    }
  case 'm':   case 'M':
    {
      fact = 1.0E-03;
      value.pop_back();
      break;
    }
  case 'u': //case 'U':
    {
      fact = 1.0E-06;
      value.pop_back();
      break;
    }
  case 'n': //case 'N':
    {
      fact = 1.0E-09;
      value.pop_back();
      break;
    }
  case 'p': //case 'P':
    {
      fact = 1.0E-12;
      value.pop_back();
      break;
    }
  case 'f':
    {
      fact = 1.0E-16;
      value.pop_back();
      break;
    }
    // default no scaling factor
  default:
    {
      fact = 1.0E+00;
      break;
    }
    }
  if (verbosity) std::cout << " fact=" << fact << "\n";
  return fact;
}

static TICTAC_SPTR<CS_ExpressionBasis> ret_ = 0;
static const CS_ModuleDeclaration* module_ = nullptr;
static int startLine_ = 0;
static int startColumn_ = 0;
static std::string latVarName_ = "";
static const ExpansionStack* stack_ = nullptr;
static bool ignoreActualLevel_S_ = false;

static TICTAC_SPTR<CS_ExpressionBasis>& parseExpression_xml(iter_t const& i)
{

  using namespace tictac;
  using namespace std;

  if (i->value.id() == ThetaParser::constantID)
    {
      string value = string(i->value.begin(), i->value.end());
      value = IP_ExpressionParser::deleteSpaces(value);
      MYREAL fact = IP_ExpressionParser::getScalingFactor(value);
      MYREAL final_value = ((MYREAL)atof(value.c_str())) * fact;
      // for integers generate the correct Number representation
      if (round(final_value) == final_value)
        ret_ = TICTAC_SPTR<CS_ExpressionBasis>(
            new CS_Number( (int)(final_value), module_, startLine_ , startColumn_) );
      else
        ret_ = TICTAC_SPTR<CS_ExpressionBasis>(
            new CS_Number( final_value, module_, startLine_ , startColumn_) );
      IP_ExpressionParser::garbigeCollection_.push_back(ret_);
      //const double d = boost::lexical_cast<double>(value);
      if (verbosity) std::cout << "C(" << ret_->evalConst() << ")";
    }
  else if (i->value.id() == ThetaParser::factorID)
    {
      iter_t const BaseNode = i->children.begin();
      iter_t const ExpNode = i->children.begin() + 1;
      parseExpression_xml(BaseNode);
      CS_ExpressionBasis *arg[2] =
        { 0, 0 };
      auto left = ret_;
      arg[0] = ret_.get();
      if (verbosity) std::cout << "**";
      parseExpression_xml(ExpNode);
      arg[1] = ret_.get();
      ret_ = TICTAC_SPTR<CS_ExpressionBasis>(
          new CS_AnalogBuiltInMathFunction( VERILOG_FUNCMATH_POW,
              arg, 2 , module_, startLine_ , startColumn_) );
      IP_ExpressionParser::garbigeCollection_.push_back(ret_);
    }
  else if (i->value.id() == ThetaParser::termID)
    {
      string value = string(i->value.begin(), i->value.end());
      value = IP_ExpressionParser::deleteSpaces(value);
      iter_t const BaseNode = i->children.begin();
      iter_t const ExpNode = i->children.begin() + 1;
      parseExpression_xml(BaseNode);
      TICTAC_SPTR<CS_ExpressionBasis> left = ret_;
      if (verbosity) std::cout << value;
      parseExpression_xml(ExpNode);
      BinarOpType t = VERILOG_BINAR_MUL;
      if (value == "/")
        t = VERILOG_BINAR_DIV;
      ret_ = TICTAC_SPTR<CS_ExpressionBasis>(
          new CS_BinaryOperator( t, left.get(),
              ret_.get() , module_, startLine_ , startColumn_) );
      IP_ExpressionParser::garbigeCollection_.push_back(ret_);
    }
  else if (i->value.id() == ThetaParser::unaryopID)
    {
      string value = string(i->value.begin(), i->value.end());
      value = IP_ExpressionParser::deleteSpaces(value);
      iter_t const BaseNode = i->children.begin();
      UnaryOpType t = VERILOG_UNAR_NONE;
      if (value == "+")
        t = VERILOG_UNAR_PLUS;
      if (value == "-")
        t = VERILOG_UNAR_MIN;
      if (value == "!")
        t = VERILOG_UNAR_LOGNOT;
      if (value == "~")
        t = VERILOG_UNAR_REDNOT;

      if (verbosity) std::cout << "(" << value;
      parseExpression_xml(BaseNode);
      if (verbosity) std::cout << ")";
      ret_ = TICTAC_SPTR<CS_ExpressionBasis>(
          new CS_UnaryOperator( t,
              ret_.get() , module_, startLine_ , startColumn_) );
      IP_ExpressionParser::garbigeCollection_.push_back(ret_);
    }
  else if (i->value.id() == ThetaParser::tenoropID)
    {
      string value = string(i->value.begin(), i->value.end());
      value = IP_ExpressionParser::deleteSpaces(value);
      iter_t const cond = i->children.begin();
      iter_t const t = i->children.begin() + 1;
      iter_t const f = i->children.begin() + 2;
      if (verbosity) std::cout << "(";
      parseExpression_xml(cond);
      auto c = ret_;
      if (verbosity) std::cout << ")?(";
      parseExpression_xml(t);
      auto t_b = ret_;
      if (verbosity) std::cout << "):(";
      parseExpression_xml(f);
      auto f_b = ret_;
      if (verbosity) std::cout << ")";
      ret_ = TICTAC_SPTR<CS_ExpressionBasis>(
          new CS_AnalogConditionalExpression( c.get(), t_b.get(), f_b.get(),
              module_, startLine_ , startColumn_) );
      IP_ExpressionParser::garbigeCollection_.push_back(ret_);
    }
  else if (i->value.id() == ThetaParser::expressionID)
    {
      string value = string(i->value.begin(), i->value.end());
      value = IP_ExpressionParser::deleteSpaces(value);
      iter_t const firstNode = i->children.begin();
      iter_t const secondNode = i->children.begin() + 1;
      parseExpression_xml(firstNode);
      auto left = ret_;
      if (verbosity) std::cout << value;
      parseExpression_xml(secondNode);
      // decide which expression to create
      BinarOpType t = VERILOG_BINAR_MODULUS;
      if (boost::algorithm::equals(value, string("+")))
        {
          t = VERILOG_BINAR_PLUS;
        }
      else if (boost::algorithm::equals(value, string("-")))
        {
          t = VERILOG_BINAR_MIN;
        }
      else if (boost::algorithm::equals(value, string("<=")))
        { //&lt;=
          t = VERILOG_BINAR_LEQ;
        }
      else if (boost::algorithm::equals(value, string(">=")))
        { //&gt;=
          t = VERILOG_BINAR_GEQ;
        }
      else if (boost::algorithm::equals(value, string("==")))
        {
          t = VERILOG_BINAR_LOGEQ;
        }
      else if (boost::algorithm::equals(value, string("!=")))
        {
          t = VERILOG_BINAR_LOGNEQ;
        }
      else if (boost::algorithm::equals(value, string("&&")))
        { //&amp;&amp;
          t = VERILOG_BINAR_LOGAND;
        }
      else if (boost::algorithm::equals(value, string("||")))
        {
          t = VERILOG_BINAR_LOGOR;
        }
      else if (boost::algorithm::equals(value, string("&")))
        { //&amp;&amp;
          t = VERILOG_BINAR_REDAND;
        }
      else if (boost::algorithm::equals(value, string("|")))
        {
          t = VERILOG_BINAR_REDOR;
        }
      else if (boost::algorithm::equals(value, string(">")))
        { //"&gt;"
          t = VERILOG_BINAR_GT;
        }
      else if (boost::algorithm::equals(value, string("<")))
        { //"&lt;"
          t = VERILOG_BINAR_LT;
        }
      ret_ = TICTAC_SPTR<CS_ExpressionBasis>(
          new CS_BinaryOperator( t, left.get(),
              ret_.get() , module_, startLine_ , startColumn_) );
      IP_ExpressionParser::garbigeCollection_.push_back(ret_);
    }
  else if (i->value.id() == ThetaParser::variableID)
    {
      string varName = string(i->value.begin(), i->value.end());
      varName = IP_ExpressionParser::deleteSpaces(varName);
      latVarName_ = varName; // store the variable name

      // in some cases the stack might be NULL
      bool makeVariable = (stack_ == nullptr);

      if (verbosity) std::cout << "makeVariable=" << makeVariable << "\n";

      if (!makeVariable)
        {
          bool hasInitialValue = false;
          std::string id = sim_toLowerString( varName );
          bool paramFound = stack_->hasParameter( id, hasInitialValue, ignoreActualLevel_S_);

          if (verbosity)
            std::cout << "paramFound=" << paramFound << " varName=" << id
                      << " hasInitialValue=" << hasInitialValue
                      << " ignoreActualLevel_S_=" << ignoreActualLevel_S_ << "\n";

          if (!paramFound)
            {
              makeVariable = true;
            }
          else
            {
              if (hasInitialValue)
                {
                  // IMPORTANT: in SPICE we treat everything with lower case
                  ret_ = stack_->getParameterInitialValue( id , ignoreActualLevel_S_);
                  if (verbosity) std::cout << "E expr=" << id << " init param value \n";
                }
              else
                {
                  ret_ = TICTAC_SPTR<CS_ExpressionBasis>(
                      new CS_ParameterReference(
                          (stack_->getParameterReference(id, ignoreActualLevel_S_)).get(),
                          nullptr, module_, startLine_ , startColumn_ ));
                  if (verbosity) std::cout << "E expr=" <<id << " param \n";
                  IP_ExpressionParser::garbigeCollection_.push_back(ret_);
                }
            }
        }

      // TODO: this might be a parameter
      if (makeVariable)
        {
          auto varDeclr = TICTAC_SPTR<CS_VariableDeclaration>(
              new CS_VariableDeclaration( module_, startLine_ , startColumn_,
                  varName, 0 ));
          IP_ExpressionParser::garbigeCollectionVarRef_.push_back(varDeclr);
          ret_ = TICTAC_SPTR<CS_ExpressionBasis>(
              new CS_VariableReference( varDeclr.get(),
                  0 , module_, startLine_ , startColumn_) );
          IP_ExpressionParser::garbigeCollection_.push_back(ret_);

          // once we left only with the variable name then add
          if (verbosity) std::cout << "V(" << varName << ")";
        }
    }
  else if (i->value.id() == ThetaParser::endtermID)
    {
      // throw some error
      if (error_verbose)
        std::cout << "ERROR \n";
    }
  else if (i->value.id() == ThetaParser::functionCallID)
    {
      iter_t const firstNode = i->children.begin();
      // first node is the function name
      parseExpression_xml(firstNode);
      //latVarName_;
      auto funcdeclr = TICTAC_SPTR<CS_AnalogFunctionDeclaration>(
          new CS_AnalogFunctionDeclaration( latVarName_, REAL, nullptr,
              module_, startLine_ , startColumn_));
      IP_ExpressionParser::garbigeCollectionFunct_.push_back(funcdeclr);
      if (verbosity) std::cout << "_(";
      std::vector<CS_ExpressionBasis*> args;
      for (decltype(i->children.size()) ii = 1; ii < i->children.size(); ++ii)
        {
          iter_t const n = i->children.begin() + ii;
          if (verbosity) std::cout << ",";
          parseExpression_xml(n);
          args.push_back(ret_.get());
          funcdeclr->addArgumentVariable(nullptr, VF_FUNCVAR_IN, REAL);
        }
      ret_ = TICTAC_SPTR<CS_ExpressionBasis>(
          new CS_AnalogFunctionCall( funcdeclr.get(),
              args, (int)args.size() , module_, startLine_ , startColumn_) );
      IP_ExpressionParser::garbigeCollection_.push_back(ret_);
      if (verbosity) std::cout << "_)";
    }
  else if (i->value.id() == ThetaParser::quoteStringID)
    {
      string str = string(i->value.begin(), i->value.end());
      ret_ = TICTAC_SPTR<CS_ExpressionBasis>(
          new CS_String(str, module_, startLine_ , startColumn_) );
      IP_ExpressionParser::garbigeCollection_.push_back(ret_);
      if (verbosity) std::cout << "str(" << str << ")";
    }
  else
    {
      // throw some error
      if (error_verbose)
        std::cout << "ERROR \n";
    }
  return ret_;
}

#define BOOST_SPIRIT_DUMP_PARSETREE_AS_XML

TICTAC_SPTR<CS_ExpressionBasis> IP_ExpressionParser::parseExpression_new(
    const IP_Token *token,
    const ExpansionStack* stack)
  {
    tree_parse_info<> info;
    ThetaParser parser;

    std::stringstream stringstr_out (std::stringstream::out);

    if (verbosity) std::cout << "INPUT expr=" <<token->getValue() << "\n";
    info = ast_parse(token->getValue().c_str(), parser);

    if (info.full)
      {
#if defined(BOOST_SPIRIT_DUMP_PARSETREE_AS_XML)
        // dump parse tree as XML
        std::map<parser_id, std::string> rule_names;
        rule_names[ThetaParser::constantID] = "constant";
        rule_names[ThetaParser::factorID] = "factor";
        rule_names[ThetaParser::termID] = "term";
        rule_names[ThetaParser::expressionID] = "expression";
        rule_names[ThetaParser::variableID] = "variable";
        rule_names[ThetaParser::endtermID] = "endterm";
        rule_names[ThetaParser::functionCallID] = "FctCall";

        rule_names[ThetaParser::unaryopID] = "unary";
        rule_names[ThetaParser::tenoropID] = "tenor";
        rule_names[ThetaParser::quoteStringID] = "strop";

        tree_to_xml(stringstr_out, info.trees, token->getValue().c_str(), rule_names);

        stack_ = stack;
        module_ = nullptr;
        startLine_ = 0;
        startColumn_ = 0;
        ignoreActualLevel_S_ = this->ignoreActualLevel_;
        auto p = parseExpression_xml( info.trees.begin() );
#endif
        if (verbosity) std::cout << "parsing succeeded \n"; // << info.trees;
        return p;
        //return TICTAC_SPTR<CS_ExpressionBasis>(nullptr);
      }
    else
      {
        if (error_verbose)
          std::cout << "!!!!!!!!!!parsing failed for!!!!" << token->getValue() << "\n";
        // TODO: this is memory leak ...
        return TICTAC_SPTR<CS_ExpressionBasis>(TICTAC_SPTR<CS_ExpressionBasis>(
            new CS_Number( 0.0, nullptr, 0 , 0) ));
      }
  }

// ---- clean string from empty spaces -----
std::string
IP_ExpressionParser::deleteSpaces(std::string &input)
{
  // delete empty spaces from the string
  boost::algorithm::trim_right(input);
  boost::algorithm::trim_left(input);
  return input;
}

