/*
 * CS_Number.cpp
 *
 *  Created on: Nov 30, 2012
 *      Author: benk
 */

#include "CS_Number.hpp"
#include "CS_ModuleDeclaration.hpp"

CS_Number::CS_Number(std::string value, const CS_ModuleDeclaration* module,
    int startLine, int startColumn, bool isOnlyString) :
    CS_ExpressionBasis(module, startLine, startColumn)
{
    // dummy initializations
    valD_ = 1.0;
    valI_ = 1.0;
    typeConst_ = REAL;
    valueS_ = std::string(value);
    // this is default since we might have constans such as "default"
    typeConst_ = NO_TYPE;
    setLocalType( typeConst_ );

    if (!isOnlyString)
      {
        // do this only if this is a real constant number (and not some string for defines or predefined stuff)
        std::stringstream str_tmp(value.c_str());
        // first try to convert to integer
        if( (str_tmp >> valI_).fail() )
          {
            char* end = 0 ;
            valD_ = strtod( value.c_str() , &end );
            // try to convert to REAL
            if ( *end == 0 )
              {
                typeConst_ = REAL;
                valI_ = (int)( valD_ );
              }
            else
              {
                // all conversions failed, so throw an error
                VERILOG_ERROR_STOP("Invalid constant number", this);
              }
          }
        else
          // converting to integer succeeded
          {
            typeConst_ = INTEGER;
            valD_ = (double) valI_;
          }
        // set the local type of the expression
        setLocalType( typeConst_ );
      }
}

CS_Number::CS_Number(int value, const CS_ModuleDeclaration* module,
    int startLine, int startColumn)  : CS_ExpressionBasis(module, startLine, startColumn)
{
  // initializations
  valD_ = 1.0;
  valI_ = value;
  typeConst_ = INTEGER;
  valueS_ = verilog_integerToString(valI_);
  // set the local type of the expression
  setLocalType( typeConst_ );
}

CS_Number::CS_Number(double value, const CS_ModuleDeclaration* module,
    int startLine, int startColumn) : CS_ExpressionBasis(module, startLine, startColumn)
{
  // initializations
  valD_ = value;
  valI_ = 1;
  typeConst_ = REAL;
  valueS_ = verilog_doubleToString(valD_);
  // set the local type of the expression
  setLocalType( typeConst_ );
}

