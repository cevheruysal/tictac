/*
 * IP_ParserBase.cpp
 *
 *  Created on: Jan 29, 2015
 *      Author: benk
 */

#include "IP_ParserBase.hpp"

#include "IP_ConditionedNetlist.hpp"
#include "IP_Element.hpp"
#include "IP_Function.hpp"
#include "IP_Model.hpp"
#include "IP_Param.hpp"
#include "IP_SubcirDef.hpp"
#include "IP_XCall.hpp"
#include "IP_Option.hpp"
#include "IP_Output.hpp"
#include "IP_AnaysisCard.hpp"
#include "IP_GlobalSettings.hpp"
#include "IP_SubcirSettings.hpp"

#include "IP_Measurement.hpp"
#include "IP_Assertion.hpp"

#include "IP_SimpleScanner.hpp"
#include "IP_NetlistStructSequence.hpp"
#include "sim_utils.hpp"

IP_ParserBase::IP_ParserBase() : UT_SIMVerboseClass(std::string("parser"))
{
  // initalization is made when the parsing starts

  subcircuitLevel_ = -1; // such that ++ will give 0 and this means top-level
  scanner_ = TICTAC_SPTR<IP_ScannerBase>(new IP_SimpleScanner());
  //this->setVerb(10);
}


void IP_ParserBase::parseFile(std::string netlistFile)
{
  // initialize parser
  subcircuitLevel_ = -1;
  // start reading the netlist
  scanner_->startReadingFile(netlistFile);
  // only if we did not finished reading
  if (scanner_->finishedReading()) {
      reportError("Scanner not open file" + netlistFile, IP_Parser_NOFATAL);
  }
  else {
      isParsingTopLevel_ = true;
      topDefTree_ = buildSubcirDef();
  }
}


void IP_ParserBase::reportError(std::string errMsg, ParserErrorSeverity severity)
{
  SIM_ERROR_NOSTOP(errMsg);
  if (severity == IP_Parser_FATAL) {
      exit(1);
  }
}


void IP_ParserBase::reportError(std::string errMsg, ParserErrorSeverity severity, const IP_Token &token)
{
  if (severity == IP_Parser_FATAL) {
      std::cout << " At line: " << token.getRowIndex() << " col:" << token.getColumnIndex()
                << " starting with token:\"" << token.getValue() << "\" of file:"
                << ((token.getFileName() != NULL) ? *token.getFileName() : std::string("")) << "\n";
      // TODO: open the file and read until this line ... and display this line
      displayParsingErrorAtToken(token);
  }
  SIM_ERROR_COND_NOSTOP(severity == IP_Parser_FATAL, errMsg);
  if (severity == IP_Parser_FATAL) {
      exit(1);
  }
}

void IP_ParserBase::displayParsingErrorAtToken(const IP_Token &token)
{
  const std::string* fileName = token.getFileName();
  int rowInt = token.getRowIndex();
  int colInt = token.getColumnIndex();
  std::ifstream actFileHandle;
  std::string   actLine;

  actFileHandle.open(fileName->c_str(), std::ifstream::in);
  for (int i = 0; i < rowInt; i++ ) {
      std::getline(actFileHandle, actLine);
  }
  std::cout << actLine << "\n";
  for (int i = 0; i < colInt; i++ ) {
      std::cout << ".";
  }
  std::cout << "^\n";
  actFileHandle.close();
}

bool IP_ParserBase::matchOptions([[maybe_unused]] const std::vector<IP_Token> &tokens)
{
  IP_Token &firstToken = tokens_[0];
  // match to options
  if (   sim_toLowerString(firstToken.getValue()) == ".option"
      || sim_toLowerString(firstToken.getValue()) == ".options" )
    return true;
  else
    return false;
}

bool IP_ParserBase::matchOutput([[maybe_unused]] const std::vector<IP_Token> &tokens)
{
  IP_Token &firstToken = tokens_[0];
  // match for output
  if (   sim_toLowerString(firstToken.getValue()) == ".output"
      || sim_toLowerString(firstToken.getValue()) == ".probe"
      || sim_toLowerString(firstToken.getValue()) == ".plot"
      || sim_toLowerString(firstToken.getValue()) == ".print"
     )
    return true;
  else
    return false;
}
bool IP_ParserBase::matchMeasurement(const std::vector<IP_Token> &tokens)
{
  const IP_Token &firstToken = tokens[0];
  // match for measurement
  if (   sim_toLowerString(firstToken.getValue()) == ".measure"
      || sim_toLowerString(firstToken.getValue()) == ".meas" )
    return true;
  else
    return false;
}
bool IP_ParserBase::matchAssertion(const std::vector<IP_Token> &tokens)
{
  const IP_Token &firstToken = tokens[0];
  // match for assertion
  if ( sim_toLowerString(firstToken.getValue()) == ".assert" )
    return true;
  else
    return false;
}
bool IP_ParserBase::matchGlobalSettings([[maybe_unused]] const std::vector<IP_Token> &tokens)
{
  IP_Token &firstToken = tokens_[0];
  // match for global settings
  if (   sim_toLowerString(firstToken.getValue()) == ".global"
      || sim_toLowerString(firstToken.getValue()) == ".save"
      || sim_toLowerString(firstToken.getValue()) == ".test"
      || sim_toLowerString(firstToken.getValue()) == ".temp"
      || sim_toLowerString(firstToken.getValue()) == ".systemc"
     )
    return true;
  else
    return false;
}

bool IP_ParserBase::matchAnalysisCard(const std::vector<IP_Token> &tokens)
{
  const IP_Token &firstToken = tokens[0];
  // match various analysys cards
  if (   sim_toLowerString(firstToken.getValue()) == ".op"
      || sim_toLowerString(firstToken.getValue()) == ".dc"
      || sim_toLowerString(firstToken.getValue()) == ".tran"
      || sim_toLowerString(firstToken.getValue()) == ".ac"
     )
    return true;
  else
    return false;
}
bool IP_ParserBase::matchConditionedNetlist(const std::vector<IP_Token> &tokens)
{
  const IP_Token &firstToken = tokens[0];
  const IP_Token &thirdToken = (tokens.size() >= 3) ? tokens[2] : tokens[0];
  // match for conditioned netlisting
  if (    sim_toLowerString(firstToken.getValue()) == ".if"
       && sim_toLowerString(thirdToken.getValue()) == "then"
     )
    return true;
  else
    return false;
}
bool IP_ParserBase::matchFunction(const std::vector<IP_Token> &tokens)
{
  const IP_Token &firstToken = tokens[0];
  // match for function declaration
  if (   sim_toLowerString(firstToken.getValue()) == ".func" )
    return true;
  else
    return false;
}
bool IP_ParserBase::matchParam(const std::vector<IP_Token> &tokens)
{
  const IP_Token &firstToken = tokens[0];
  // match for function declaration
  if (   sim_toLowerString(firstToken.getValue()) == ".param"
      || sim_toLowerString(firstToken.getValue()) == ".params" )
    return true;
  else
    return false;
}
bool IP_ParserBase::matchModel(const std::vector<IP_Token> &tokens)
{
  const IP_Token &firstToken = tokens[0];
  // match for model declaration
  if (    sim_toLowerString(firstToken.getValue()) == ".model"
       && (tokens.size() > 2) // this must have at least a model name and a model type
     )
    return true;
  else
    return false;
}
bool IP_ParserBase::matchSubcirDef(const std::vector<IP_Token> &tokens)
{
  const IP_Token &firstToken = tokens[0];
  // match for subcircuit declaration
  if (   sim_toLowerString(firstToken.getValue()) == ".subckt" )
    return true;
  else
    return false;
}
bool IP_ParserBase::matchSubcirSettings([[maybe_unused]] const std::vector<IP_Token> &tokens)
{
  IP_Token &firstToken = tokens_[0];
  // match for subcircuit settings/options
  if (   sim_toLowerString(firstToken.getValue()) == ".connect"
      || sim_toLowerString(firstToken.getValue()) == ".nodeset"
      || sim_toLowerString(firstToken.getValue()) == ".ic"
      || sim_toLowerString(firstToken.getValue()) == ".comopt"
      || sim_toLowerString(firstToken.getValue()) == ".hdl"
     )
    return true;
  else
    return false;
}
bool IP_ParserBase::matchXcall(const std::vector<IP_Token> &tokens)
{
  const IP_Token &firstToken = tokens[0];
  std::string potentialInstanceName = sim_toLowerString(firstToken.getValue());
  // match for potential X-call, based on the potential instance name
  if ( potentialInstanceName[0] == 'x' )
    return true;
  else
    return false;
}

bool IP_ParserBase::matchElement(const std::vector<IP_Token> &tokens)
{
  const IP_Token &firstToken = tokens[0];
  std::string potentialInstanceName = sim_toLowerString(firstToken.getValue());
  // test for all potential instance names
  // TODO: later extend this list with other elements
  if (    potentialInstanceName[0] == 'r'
       || potentialInstanceName[0] == 'c'
       || potentialInstanceName[0] == 'l'
       || potentialInstanceName[0] == 'v'
       || potentialInstanceName[0] == 'd'
       || potentialInstanceName[0] == 'e'
       || potentialInstanceName[0] == 'f'
       || potentialInstanceName[0] == 'g'
       || potentialInstanceName[0] == 'i'
       || potentialInstanceName[0] == 'h'
       || potentialInstanceName[0] == 'm'
       || potentialInstanceName[0] == 'q'
       || potentialInstanceName[0] == 'j'

       || potentialInstanceName[0] == 'b'

       || potentialInstanceName[0] == 's'
       || potentialInstanceName[0] == 'z'

       || potentialInstanceName[0] == 'k'
       || potentialInstanceName[0] == 'u'
       || potentialInstanceName[0] == 'a'
     )
    return true;
  else
    return false;
}

// method to read in the next record (a record should contain at least one token)
bool IP_ParserBase::readInNextRecord(bool showErrorIfNotEndedProperly)
{
  bool needsNextRead = false;

  SIM_PRINT_L6(verb(), " IP_ParserBase::readInNextRecord() ");
  // loop as long we either finish reading, have no error,
  // and have at least one token in the record
  do
    {
      IP_ScannerStatusInfo status;
      scanner_->getNextRecord(status);
      SIM_PRINT_L6(verb(), "  nr. tokens = " << scanner_->getNrTokensInActualRecord());
      // check scanner for error
      if (status.isError())
        {
          reportError(std::string("ScannerMSG: ") + status.errorMessage(), IP_Parser_NOFATAL);
          needsNextRead = true;
        }

      // if no record is there then we should read further
      if (scanner_->getNrTokensInActualRecord() < 1)
        needsNextRead = true;

      // if we have new tokens than put them into out token lis:
      if ( needsNextRead == false ) {
          SIM_PRINT_L6(verb(), " store tokens nr.tokens=" << scanner_->getNrTokensInActualRecord()
              << " needsNextRead="<< needsNextRead);
          this->tokens_.clear();
          for (int i = 0; i < scanner_->getNrTokensInActualRecord(); i++)
            tokens_.push_back( scanner_->getToken(i) );
      }

      // if reading is finished then exit the whole parser
      if (scanner_->finishedReading() && needsNextRead ) {
          SIM_PRINT_L6(verb(), " finish fl=" << scanner_->finishedReading() << " nr. tokens=" << scanner_->getNrTokensInActualRecord());
          if (showErrorIfNotEndedProperly)
            reportError(std::string("Scanner: finished reading internal error "), IP_Parser_NOFATAL);
          // return false means
          return false;
      }
      // read as long we have at least one token as a valid recors
      SIM_PRINT_L6(verb(), " readInNextRecord nrTokens="
          << scanner_->getNrTokensInActualRecord() << " needsNextRead=" << needsNextRead
          << " status.isError()=" << status.isError());
    }
  while (needsNextRead == true);
  return true;
}



// =============== ELEMENT/DEVICE BUILDING =============
TICTAC_SPTR<IP_Element>             IP_ParserBase::buildElement()
{
  IP_Token &firstToken = tokens_[0];
  std::string potentialInstanceName = sim_toLowerString(firstToken.getValue());
  IP_ElemPreType elemt = ERR_ELEM;
  // get the type of the element
  if (potentialInstanceName[0] == 'r') elemt = R_ELEM;
  if (potentialInstanceName[0] == 'c') elemt = C_ELEM;
  if (potentialInstanceName[0] == 'l') elemt = L_ELEM;
  if (potentialInstanceName[0] == 'v') elemt = V_ELEM;
  if (potentialInstanceName[0] == 'd') elemt = D_ELEM;

  if (potentialInstanceName[0] == 'e') elemt = E_ELEM;
  if (potentialInstanceName[0] == 'f') elemt = F_ELEM;
  if (potentialInstanceName[0] == 'g') elemt = G_ELEM;
  if (potentialInstanceName[0] == 'h') elemt = H_ELEM;
  if (potentialInstanceName[0] == 'i') elemt = I_ELEM;

  if (potentialInstanceName[0] == 'm') elemt = M_ELEM;
  if (potentialInstanceName[0] == 'q') elemt = Q_ELEM;
  if (potentialInstanceName[0] == 'j') elemt = J_ELEM;

  if (potentialInstanceName[0] == 'b') elemt = B_ELEM;

  if (potentialInstanceName[0] == 's') elemt = S_ELEM;
  if (potentialInstanceName[0] == 'z') elemt = Z_ELEM;

  if (potentialInstanceName[0] == 'k') elemt = K_ELEM;
  if (potentialInstanceName[0] == 'u') elemt = U_ELEM;
  if (potentialInstanceName[0] == 'a') elemt = A_ELEM;

  if (elemt == ERR_ELEM) {
      reportError(std::string("No match for element, return dummy Resistor ")
                  + potentialInstanceName, IP_Parser_NOFATAL);
      return TICTAC_SPTR<IP_Element>(new  IP_Element(tokens_, R_ELEM));
  }
  else {
    // we generate an element
      SIM_PRINT_L5(verb(), " buildElement no error ");
    return TICTAC_SPTR<IP_Element>(new  IP_Element(tokens_, elemt));
  }
}

// =============== FUNCTION BUILDING =============
TICTAC_SPTR<IP_Function>            IP_ParserBase::buildFunction()
{
  // just build a function with the actual tokens
  return TICTAC_SPTR<IP_Function>(new  IP_Function(tokens_));
}

// =============== MODEL BUILDING =============
TICTAC_SPTR<IP_Model>               IP_ParserBase::buildModel()
{
  // get the parameter declarations on the model card
  std::vector< TICTAC_SPTR<IP_Param> > params =
      buildParameterSequence( 3, (int)tokens_.size(), tokens_);

  // just build a model with the actual tokens
  return TICTAC_SPTR<IP_Model>(new  IP_Model(tokens_, tokens_[1].getValue(),
      tokens_[2].getValue(),  params ));
}


// =============== X-CALL BUILDING ============
TICTAC_SPTR<IP_XCall>               IP_ParserBase::buildXCall()
{
  int n = (int) tokens_.size(), i = 1;
  int nrPinn = 0;
  bool hadSubckt = false;
  std::string subcirName = ":=| :-("; // to signal error
  std::vector< TICTAC_SPTR<IP_Param> > xparams(0);
  std::vector< std::string > pinNameBinding(0); // name binding is empty for TITAN
                                               // since there is only binding by order

  // test if we should have at least 3 tokens
  if (tokens_.size() < 3)
    reportError("Not enough tokens for X-call", IP_Parser_NOFATAL, tokens_[0]);

  // now loop through each token and see which are pinns and which are params of the subckt
  while (i < n)
    {
      std::vector<IP_Token> tokensTmp(0); // required only for parameter declaration
      if (    (i + 1 < n)
           && (tokens_[i + 1].getTokenType() == TK_EQUAL)
         )
        { // here we just call the method to parse the parameter sequence
          xparams = buildParameterSequence( i, (int) tokens_.size(), tokens_);
          // break the loop, since we are finished
          if ( i < 2 )
            reportError("At least one pinn is required for an X-call", IP_Parser_NOFATAL, tokens_[i]);
          else {
              // take care of special string "params:" :-(
              if (tokens_[i-1].getValue() != "params:")
                subcirName = tokens_[i-1].getValue();
              else
                { subcirName = tokens_[i-2].getValue(); nrPinn--; }
            hadSubckt = true;
          }
          break;
        }
      else
        { // here we have simple port/pin, and just count the pinn numbers
          i++; nrPinn++;
        }
    }

  // if no parameters are given, then we will end up here, than the last one is the subcircuit name
  if (!hadSubckt) {
      // take care of special string "params:" :-(
      if (tokens_[tokens_.size()-1].getValue() != "params:")
        subcirName = tokens_[tokens_.size()-1].getValue();
      else
        { subcirName = tokens_[tokens_.size()-2].getValue(); nrPinn--; }
  }
  // debug the name of the called subcircuit name
  SIM_PRINT_L6(verb(), " IP_XCall name=" << subcirName);

  // build the X-call
  return TICTAC_SPTR<IP_XCall>(new  IP_XCall(tokens_,
      nrPinn-1, subcirName, xparams, pinNameBinding));
}

// ============== OUTPUT =============
TICTAC_SPTR<IP_Output>             IP_ParserBase::buildOutput()
{
  // just build the probe statements, such that later they could be postprocessed
  return TICTAC_SPTR<IP_Output>(new  IP_Output(tokens_));
}

// ============== MEASUREMENT =============
TICTAC_SPTR<IP_Measurement>        IP_ParserBase::buildMeasurement()
{
  // just build a measurement
  return TICTAC_SPTR<IP_Measurement>(new  IP_Measurement(tokens_));
}

// ============== ASSERTION =============
TICTAC_SPTR<IP_Assertion>          IP_ParserBase::buildAssertion()
{
  // just build an assertion
  return TICTAC_SPTR<IP_Assertion>(new  IP_Assertion(tokens_));
}

// =============== BUILD PARAMETER declaration
std::vector< TICTAC_SPTR<IP_Param> >  IP_ParserBase::buildParam()
{
  // just call a function to build the list of parameter declarations
  // starting from the first token
  return buildParameterSequence(1, (int)tokens_.size(), tokens_);
}

// ================= BUILD OPTIONS ============
std::vector< TICTAC_SPTR<IP_Option> >        IP_ParserBase::buildOptions()
{
  //here it could be multiple options .. and an option is either NAME or NAME=VALUE

  // this is the return vector
  std::vector< TICTAC_SPTR<IP_Option> > retVect;
  int i = 1, n = (int)tokens_.size();

  // loop for all tokens
  while ( i < n )
    {
      std::vector<IP_Token> tokens(0);
      // the name of the option
      tokens.push_back(tokens_[i]);
      // there must be an "=" sign and then an expression token
      if ( (i+2 < n) && (tokens_[i+1].getTokenType() == TK_EQUAL) ){
          tokens.push_back(tokens_[i+2]);
          tokens[1].setTokenType(TK_EXPRESSION); // this must be an expression
          // we need to pass this Ctor only 2 tokens
          retVect.push_back( TICTAC_SPTR<IP_Option>(new IP_Option(tokens) ));
          i = i + 3;
      }
      else {
          // report error
          retVect.push_back( TICTAC_SPTR<IP_Option>(new IP_Option(tokens) ));
          i++;
      }
    }
  // just return the vector with options
  return retVect;
}


// ================= BUILD ANALYSIS CARD ============
TICTAC_SPTR<IP_AnaysisCard>         IP_ParserBase::buildAnalysisCard()
{
  IP_Token &firstToken = tokens_[0];
  AnalysisCardType analyType = IP_ANALYSIS_ERR;
  // match various analysys cards
  if ( sim_toLowerString(firstToken.getValue()) == ".op" ) analyType = IP_ANALYSIS_OP;
  if ( sim_toLowerString(firstToken.getValue()) == ".dc" ) analyType = IP_ANALYSIS_DC;
  if ( sim_toLowerString(firstToken.getValue()) == ".tran" ) analyType = IP_ANALYSIS_TR;
  if ( sim_toLowerString(firstToken.getValue()) == ".ac" ) analyType = IP_ANALYSIS_AC;
  // return the analysis card
  return TICTAC_SPTR<IP_AnaysisCard>(new  IP_AnaysisCard(tokens_, analyType));
}

TICTAC_SPTR<IP_GlobalSettings>        IP_ParserBase::buildGlobalSettings()
{
  IP_Token &firstToken = tokens_[0];
  std::string fTokenStr = sim_toLowerString(firstToken.getValue());
  GlobalSettingType settingType = IP_GSETTING_ERR;
  // get the type of the element
  if (fTokenStr == ".global") settingType = IP_GSETTING_GLOBAL;
  if (fTokenStr == ".save")   settingType = IP_GSETTING_SAVE;
  if (fTokenStr == ".test")   settingType = IP_GSETTING_TEST;
  if (fTokenStr == ".temp")   settingType = IP_GSETTING_TEMP;
  if (fTokenStr == ".systemc")   settingType = IP_GSETTING_SYSTEMC;

  // see if this was an error
  if (settingType == IP_GSETTING_ERR)
        reportError(std::string("No match for global setting ") + fTokenStr, IP_Parser_NOFATAL);

  return TICTAC_SPTR<IP_GlobalSettings>(new  IP_GlobalSettings(tokens_, settingType));
}

TICTAC_SPTR<IP_SubcirSettings>  IP_ParserBase::buildSubcirSettings()
{
  IP_Token &firstToken = tokens_[0];
  std::string fTokenStr = sim_toLowerString(firstToken.getValue());
  SubcktSettingType settingType = IP_SSETTING_ERR;
  // get the type of the element
  if (fTokenStr == ".connect") settingType = IP_SSETTING_CONNECT;
  if (fTokenStr == ".nodeset") settingType = IP_SSETTING_NODESET;
  if (fTokenStr == ".ic")      settingType = IP_SSETTING_IC;
  if (fTokenStr == ".comopt")  settingType = IP_SSETTING_COMOPT;
  if (fTokenStr == ".hdl")     settingType = IP_SSETTING_HDL;

  // see if this was an error
  if (settingType == IP_SSETTING_ERR)
        reportError(std::string("No match for subcircuit setting ") + fTokenStr, IP_Parser_NOFATAL);

  return TICTAC_SPTR<IP_SubcirSettings>(new  IP_SubcirSettings(tokens_, settingType));
}


// ========== BUILD a sequence of parameters from a token sequence
std::vector< TICTAC_SPTR<IP_Param> > IP_ParserBase::buildParameterSequence(
    int startIndex,  int endIndex,
    const std::vector<IP_Token> &tokens)
{
  // this is the general method to try to get a sequence of parameters from a token list
  // where the key concept is to use "=" then the token before is an ID and the other is
  // an expression

  int i = startIndex; // we start with the first element
  int n = ((int)tokens.size() < endIndex) ? ((int)tokens.size()) : endIndex ;
  // the returned vector
  std::vector< TICTAC_SPTR<IP_Param> > retVect(0);

  //std::cout << " buildParameterSequence " << i << "\n";

  // loop for all tokens
  while ( i < n )
    {
      std::vector<IP_Token> tokens_tmp(0);
      // the name of the param
      if (    (tokens[i].getTokenType() != TK_OPEN_BR)
           && (tokens[i].getTokenType() != TK_CLOSE_BR) )
        tokens_tmp.push_back(tokens[i]);

      // std::cout << " i=" << i << " " << tokens[i].getValue() << "\n";
      // std::cout << " i+1=" << i+1 << " " << tokens[i+1].getValue() << "\n";
      // std::cout << " i+2=" << i+2 << " " << tokens[i+2].getValue() << "\n";

      // there must be an "=" sign and then an expression token
      if ((i + 2 < n) && (tokens[i + 1].getTokenType() == TK_EQUAL) && tokens_tmp.size() > 0)
        {
          tokens_tmp.push_back(tokens[i+2]);
          tokens_tmp[1].setTokenType(TK_EXPRESSION); // this must be an expression
          // we need to pass this Ctor only 2 tokens
          retVect.push_back( TICTAC_SPTR<IP_Param>(new IP_Param(tokens_tmp) ));
          //std::cout << " add param " << i << " " << tokens[i].getValue() << ","
          //          << tokens[i+1].getValue() << "," <<  tokens[i+2].getValue() << "\n";
          i = i + 3;
        }
      else
        { // report error
          if (    (tokens[i].getTokenType() != TK_OPEN_BR)
               && (tokens[i].getTokenType() != TK_CLOSE_BR)
               && (tokens[i].getTokenType() != TK_SEMICOLON) ) {
            reportError("Cannot build parameter", IP_Parser_NOFATAL, tokens[i]);
            //int *p = 0; p[3]=4;
          }
          i++;
        }
    }
  // return the build vector
  return retVect;
}


// ================== CIRCUIT BODY BUILDER ========================
TICTAC_SPTR<IP_NetlistStructSequence> IP_ParserBase::buildStuff(
    std::vector< std::string> &expectedEnd, bool showErrorIfNotEndedProperly)
{
  // this reads as long we have a match of the body components
  bool hasMatch = false;
  int i = 0;
  TICTAC_SPTR<IP_NetlistStructSequence> ret(new IP_NetlistStructSequence( tokens_ ) );
  SIM_PRINT_L6(verb(), " IP_ParserBase::buildStuff() START " << this->subcircuitLevel_);
  do
    {
      SIM_PRINT_L6(verb(), " IP_ParserBase::buildStuff() i=" << i << " isParsingTopLevel_="
          << isParsingTopLevel_ );
      if (tokens_.size() > 0)
        SIM_PRINT_L6(verb(), " first token " << tokens_[0].getValue());
      // TODO: in the mathing one could also consider the flag "this->isParsingTopLevel_"
      // to allow stuff only on the top-level ... or only within subcircuits

      // if file is done than exit
      if (scanner_->finishedReading() && (i > 0) ) break;

      hasMatch = false;
      // read in next record
      if (!readInNextRecord(showErrorIfNotEndedProperly)) continue;
      i = i + 1;

      // now for each possible construct we check if there is a match
      // conditioned netlisting
      if (matchConditionedNetlist(tokens_)) {
          SIM_PRINT_L6(verb(), " buildConditionedNetlist() i=" << i);
          ret->addNetlistStruct(buildConditionedNetlist());
          hasMatch = true; continue;
      }
      // function declaration
      if (matchFunction(tokens_))  {
          SIM_PRINT_L6(verb(), " buildFunction() i=" << i);
          ret->addNetlistStruct(buildFunction());
          hasMatch = true; continue;
      }
      // params declarations
      if (matchParam(tokens_))  {
          SIM_PRINT_L6(verb(), " buildParam() i=" << i);
          std::vector< TICTAC_SPTR<IP_Param> > tmpVect = buildParam();
          for (const auto& tmpVectElem : tmpVect)
            ret->addNetlistStruct(tmpVectElem);
          hasMatch = true; continue;
      }
      // model declaration
      if (matchModel(tokens_))  {
          SIM_PRINT_L6(verb(), " buildModel() i=" << i);
          ret->addNetlistStruct(buildModel());
          hasMatch = true; continue;
      }
      // subcircuit declaration
      if (matchSubcirDef(tokens_))  {
          SIM_PRINT_L6(verb(), " buildSubcirDef() i=" << i);
          ret->addNetlistStruct(buildSubcirDef());
          hasMatch = true; continue;
      }
      // X-call subcircuit instantiation
      if (matchXcall(tokens_))  {
          SIM_PRINT_L6(verb(), " buildXCall() i=" << i);
          ret->addNetlistStruct(buildXCall());
          hasMatch = true; continue;
      }
      // instantiation of an element (device)
      if (matchElement(tokens_))  {
          SIM_PRINT_L6(verb(), " buildElement() i=" << i);
          ret->addNetlistStruct(buildElement());
          hasMatch = true; continue;
      }
      // match options
      if (matchOptions(tokens_))  {
          SIM_PRINT_L6(verb(), " buildOptions() i=" << i);
          std::vector< TICTAC_SPTR<IP_Option> > tmpVect = buildOptions();
          for (const auto&  tmpVectElem : tmpVect)
            ret->addNetlistStruct(tmpVectElem);
          hasMatch = true; continue;
      }
      // match output
      if (matchOutput(tokens_))  {
          SIM_PRINT_L6(verb(), " buildOutput() i=" << i);
          ret->addNetlistStruct(buildOutput());
          hasMatch = true; continue;
      }
      // match analysis cards
      if (matchAnalysisCard(tokens_))  {
          SIM_PRINT_L6(verb(), " buildAnalysisCard() i=" << i);
          ret->addNetlistStruct(buildAnalysisCard());
          hasMatch = true; continue;
      }
      // match measurement
      if (matchMeasurement(tokens_))  {
          SIM_PRINT_L6(verb(), " buildMeasurement() i=" << i);
          ret->addNetlistStruct(buildMeasurement());
          hasMatch = true; continue;
      }
      // match assertion
      if (matchAssertion(tokens_))  {
          SIM_PRINT_L6(verb(), " buildAssertion() i=" << i);
          ret->addNetlistStruct(buildAssertion());
          hasMatch = true; continue;
      }
      // match global settings
      if (matchGlobalSettings(tokens_) && isParsingTopLevel_)  {
          SIM_PRINT_L6(verb(), " buildGlobalSettings() i=" << i);
          ret->addNetlistStruct(buildGlobalSettings());
          hasMatch = true; continue;
      }
      // match subckt settings, only when we are
      if (matchSubcirSettings(tokens_))  {
          SIM_PRINT_L6(verb(), " buildSubcirSettings() i=" << i);
          ret->addNetlistStruct(buildSubcirSettings());
          hasMatch = true; continue;
      }
      // IMPORTANT: DO NOT PUT ANY CODE HERE DUE TO THE "contine" in the IFs
      // for the case when there is a metch this will not be executed !!!!

      // test if in case of non-match this s
      for (int j = 0; (j < (int)expectedEnd.size()) && (hasMatch == false); j++)
        {
          SIM_PRINT_L6(verb(), " end str '" << tokens_[0].getValue() << "' exp:'" << expectedEnd[j] << "'");
          if ( sim_toLowerString(tokens_[0].getValue()) == expectedEnd[j] )
            hasMatch = true;
        }

      // this means this sequence ended with something unexpected
      if (!hasMatch)
        {
          // this means sequence ended
          std::string errMsg = "Sequence not ended with " + expectedEnd[0];
          for (int j = 1; j < (int)expectedEnd.size(); j++)
            errMsg += " | " + expectedEnd[j];
          errMsg += " ... Ignoring this record";
          reportError( errMsg , IP_Parser_NOFATAL, tokens_[0]);
          // go on if there are further records
          hasMatch = !(scanner_->finishedReading());
        }
      else // the else branch means sequence ended OK, and we should exit the loop
        hasMatch = false;
    }
  // loop as long we find matching for our staff
  while (hasMatch);
  SIM_PRINT_L6(verb(), " IP_ParserBase::buildStuff() END " << this->subcircuitLevel_);
  // return the sequence
  return ret;
}


// ============================= CONDITIONED NETLISTING =======================
TICTAC_SPTR<IP_ConditionedNetlist>  IP_ParserBase::buildConditionedNetlist()
{
  IP_Token &secondToken = tokens_[1];
  std::vector< std::string > endings2(0);
  std::vector< std::string > endings1(0);

  secondToken.setTokenType(TK_EXPRESSION);
  SIM_PRINT_L6(verb(), " IP_ParserBase::buildConditionedNetlist START l = " << subcircuitLevel_);

  TICTAC_SPTR<IP_ConditionedNetlist> ret
    = TICTAC_SPTR<IP_ConditionedNetlist>(new IP_ConditionedNetlist(tokens_));

  // prepare the ending strings
  endings2.push_back(".else");
  endings2.push_back(".endif");
  endings2.push_back(".elseif");
  endings1.push_back(".endif");

  // parse the true branch
  TICTAC_SPTR<IP_NetlistStructSequence> newThanBr = buildStuff(endings2, true);
  ret->setThanBranch(newThanBr);

  // store this first token in order to preserv
  std::string keyword = sim_toLowerString(tokens_[0].getValue());
  std::string third   =  (tokens_.size() >= 3) ? sim_toLowerString(tokens_[2].getValue()) : keyword;

  // at this point we either should have ".else" or ".endif"
  if ( keyword == ".else" )
    {
      SIM_PRINT_L6(verb(), " IP_ParserBase::buildConditionedNetlist START ELSE");
      TICTAC_SPTR<IP_NetlistStructSequence> newFalseBr = buildStuff(endings1, true);
      ret->setFalseBranch( newFalseBr );
      SIM_PRINT_L6(verb(), " IP_ParserBase::buildConditionedNetlist END ELSE");
    }
  else {
      // this is the elseif branch, IMPORTANT: then we do not require ".ENDIF" at the end,
      // since this is a recursive construct !!!
      if (   (keyword == ".elseif")
          && (third   == "then" ) )
        {
          SIM_PRINT_L6(verb(), " IP_ParserBase::buildConditionedNetlist START ELSEIF");
          TICTAC_SPTR<IP_NetlistStructSequence> newFalseBr(new IP_NetlistStructSequence(tokens_));
          TICTAC_SPTR<IP_ConditionedNetlist>  anotherIf = buildConditionedNetlist();
          newFalseBr->addNetlistStruct(anotherIf);
          ret->setFalseBranch( newFalseBr );
          // no next record reading !!!
          SIM_PRINT_L6(verb(), " IP_ParserBase::buildConditionedNetlist END ELSEIF");
        }
      else {
          SIM_PRINT_L6(verb(), " IP_ParserBase::buildConditionedNetlist START ENDIF");
          // this means we have ".endif" and this should be digested !!!
          // read in next record , since the actual one should be ".endif"
          SIM_PRINT_L6(verb(), " IP_ParserBase::buildConditionedNetlist END ENDIF");
      }
  }

  SIM_PRINT_L6(verb(), " IP_ParserBase::buildConditionedNetlist END l = " << subcircuitLevel_);

  // return the resulting conditioned netlisting
  return ret;
}


//==================================== SUBCKT ==================================
TICTAC_SPTR<IP_SubcirDef>           IP_ParserBase::buildSubcirDef()
{
  // signal that we have non top-level after this
  bool isThisTopLevel = isParsingTopLevel_;
  int levelTmp = subcircuitLevel_;
  subcircuitLevel_++;
  // decide if we have top-level or not
  isParsingTopLevel_ = (subcircuitLevel_ > 0)?false:true;
  //
  TICTAC_SPTR<IP_SubcirDef> retSubCir;
  TICTAC_SPTR<IP_NetlistStructBase> test;
  std::string defName = "TOPLEVEL";
  std::vector < TICTAC_SPTR<IP_Param> > subCirParams(0);
  std::vector < IP_Token > subCircPorts(0);
  std::vector< std::string > endings(0);

  SIM_PRINT_L6(verb(), " IP_ParserBase::buildSubcirDef top = "
      << isParsingTopLevel_ << " subcircuitLevel_ = " << subcircuitLevel_);

  // create the subcircuit
  if (isParsingTopLevel_) // means we are on the toplevel
    { // toplevel
      std::vector<IP_Token> tokens(1);
      std::string tmp = ".TOPLEVEL";
      tokens[0].setValue(tmp);
      retSubCir = TICTAC_SPTR<IP_SubcirDef>(
          new IP_SubcirDef(tokens, defName, subCirParams, subCircPorts));
      retSubCir->setTopLevelFlag(true);
      // TOPLEVEL should end with ".end"
      endings.push_back(".end");
    }
  else
    { // non toplevel
      int n = (int) tokens_.size(), i = 2;
      // non TOPLEVEL should end with ".ends"
      endings.push_back(".ends");
      if (tokens_.size() < 3) {
        reportError("Not enough tokens for .SUBCKT declaration (subcircuit without pinns)",
            IP_Parser_NOFATAL, tokens_[0]);
      }

      // the subcircuit name also (as evrything) we store in lower case (if no case sensitive) format
      if (tokens_.size() > 1)
        defName = sim_toLowerString(tokens_[1].getValue());

      // now loop through each token and see which are pinns and which are params of the subckt
      while (i < n)
        {

          std::vector<IP_Token> tokensTmp(0); // required only for parameter declaration
          if (   (i + 1 < n)
              && (tokens_[i + 1].getTokenType() == TK_EQUAL)
              )
            { // here we just call the method to parse the parameter sequence
              subCirParams = buildParameterSequence( i, (int) tokens_.size(), tokens_);
              // break the loop, since we are finished
              break;
            }
          else
            { // here we have simple port/pin, and we simply add it to the list
              if (tokens_[i].getValue() != "params:")
                subCircPorts.push_back(tokens_[i]);
              i++;
            }
        }
      // create subcircuit header
      retSubCir = TICTAC_SPTR<IP_SubcirDef>(
                new IP_SubcirDef(tokens_, defName, subCirParams, subCircPorts));
    }

  SIM_PRINT_L6(verb(), " IP_ParserBase::buildSubcirDef START BODY subcircuitLevel_ = " << subcircuitLevel_);

  // get the body of the subcircuit
  TICTAC_SPTR<IP_NetlistStructSequence> subcktBody = buildStuff(endings, (isParsingTopLevel_ == false));
  retSubCir->setSubcktBody( subcktBody );

  SIM_PRINT_L6(verb(), " IP_ParserBase::buildSubcirDef END BODY subcircuitLevel_ = " << subcircuitLevel_);

  // reset the flags
  isParsingTopLevel_ = isThisTopLevel;
  subcircuitLevel_   = levelTmp;
  // return the subcircuit
  return retSubCir;
}
