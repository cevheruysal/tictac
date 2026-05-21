/*
 * CS_ModuleInstantiation.cpp
 *
 *  Created on: Feb 1, 2013
 *      Author: benk
 */

#include "CS_ModuleInstantiation.hpp"
#include "CS_ExpressionBasis.hpp"
#include "CS_ModuleDeclaration.hpp"

CS_ModuleInstantiation::CS_ModuleInstantiation(ModuleInstanceType instType,
    std::string instName, std::string moduleName,
    std::vector<std::string>& paramNames,
    std::vector<CS_ExpressionBasis*>& parameValues,
    std::vector<CS_ExpressionBasis*>& portConnects,
    const CS_ModuleDeclaration* module, int startLine, int startColumn ) :
instType_(instType), instName_(instName), moduleName_(moduleName), paramNames_(paramNames),
parameValues_(parameValues), portConnects_(portConnects),
module_(module), startLine_(startLine), startColumn_(startColumn)
{
  // make some checks
  switch (instType_)
  {
    case VERILOG_SPICE_RES: {
      VERILOG_ERROR_COND_STOP( ((instName[0] != 'r')&&(instName[0] != 'R')),
          "First letter of a resistor instance must be r/R", this );
      for (int i = 0; i < (int)paramNames.size(); i++){
          VERILOG_ERROR_COND_STOP(
           (((paramNames[i] != "r") && (paramNames[i] != "R")) || (paramNames.size() > 1)),
           "At the moment resistor can have only one r/R parameter", this );
      }
      break;
    }
    case VERILOG_SPICE_CAP:{
      VERILOG_ERROR_COND_STOP( ((instName[0] != 'c')&&(instName[0] != 'C')),
          "First letter of a capacitor instance must be c/C", this );
      for (int i = 0; i < (int)paramNames.size(); i++){
          VERILOG_ERROR_COND_STOP(
           (((paramNames[i] != "c") && (paramNames[i] != "C")) || (paramNames.size() > 1)),
           "At the moment capacitors can have only one c/C parameter", this );
      }
      break;
    }
    case VERILOG_SPICE_IND:{
      VERILOG_ERROR_COND_STOP( ((instName[0] != 'l')&&(instName[0] != 'L')),
          "First letter of an inductor instance must be l/L", this );
      for (int i = 0; i < (int)paramNames.size(); i++){
          VERILOG_ERROR_COND_STOP(
           (((paramNames[i] != "l") && (paramNames[i] != "L")) || (paramNames.size() > 1)),
           "At the moment inductors can have only one l/L parameter", this );
      }
      break;
    }
    case VERILOG_MODULE_INSTANCE: {
      // each Verilog instance must be an X call
      instName_ = "x" + instName_;
      // no checks, this should be done at the creation of the object
      break;
    }
    default: {
      VERILOG_ERROR_STOP("Unsupported instantiation (or Spice primitive) type", this); break;
    }
  }

  //TODO: test the parameter names
}
