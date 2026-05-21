/*
 * CS_Parameter.cpp
 *
 *  Created on: Nov 29, 2012
 *      Author: benk
 */

#include "CS_Parameter.hpp"
#include "CS_ModuleDeclaration.hpp"

#include "CS_ParameterReference.hpp"
#include "CS_Number.hpp"

CS_Parameter::CS_Parameter(std::string name, const CS_ModuleDeclaration* module,
    int startLine,  int startColumn,
    int paramIndex, VarType type,
    bool isLocal, CS_Range* range, std::string aliasName):
 module_(module), startLine_(startLine), startColumn_(startColumn), name_(name),
index_(paramIndex), type_(type), isLocalParameter_(isLocal), aliasName_(aliasName),
isDevicePrameter_(false), localParamForVariableArraySize_(0)
{
  size_ = 1;
  offset_ = 0;
  isReverse_ = false;
  range_ = range;

  // if range is given means this must be a localparam and and array
  if (range_)
    {
      range_ = range;

      // for local parameters the size MUST be fixed during compilation
      if ( isLocalParameter_ )
        {
          int leftV = verilog_nearestInt( range_->getLeftExpr()->evalConst());
          int rightV = verilog_nearestInt( range_->getRightExpr()->evalConst());

          // swap values if left is grater
          if ( leftV > rightV ){
              isReverse_ = true;
              int tmpVal = leftV;
              leftV = rightV;
              rightV = tmpVal;
          }
          // left index can not be greater
          VERILOG_ERROR_COND_STOP(leftV > rightV, " In index range of the parameter '" << name_
              << "' left index can not be greater than the right limit", this);
          size_ = rightV - leftV + 1; // set this value correctly
          VERILOG_ERROR_COND_STOP(size_ <= 0, " In index range of the parameter '" << name_
              << "' size of the variable must be at least one but it is " << size_ , this);
          offset_ = leftV;
        }
      else
        {
          // for external parameters the size IS NOT FIXED!
          // we only accept [0:localParam] constructs
          // first number must be zero
          CS_Number             *num = dynamic_cast<CS_Number*>(range_->getLeftExpr());
          VERILOG_ERROR_COND_STOP(num == 0, " In index range of the parameter '" << name_
                        << "' left index MUST be the constant zero", this);
          if (num != 0) {
              VERILOG_ERROR_COND_STOP( num->evalConst() != 0, " In index range of the parameter '"
                  << name_ << "' left index MUST be the constant zero", this);
          }
          // second range MUST be a local param reference
          CS_ParameterReference *ptr = dynamic_cast<CS_ParameterReference*>(range_->getRightExpr());
          VERILOG_ERROR_COND_STOP(ptr == 0, " In index range of the parameter '" << name_
                        << "' right index MUST be a local parameter", this);
          if (ptr != 0)
            {
              CS_Parameter *p = ptr->getParamObject();
              VERILOG_ERROR_COND_STOP(p->isLocalParam() == false, " In index range of the parameter '"
                  << name_ << "' right index MUST be a local parameter" << p->getName(), this);
              localParamForVariableArraySize_ = p;
            }
        }
    }
}

