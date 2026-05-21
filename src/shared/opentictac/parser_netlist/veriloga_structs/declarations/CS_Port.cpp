/*
 * CS_PORT_HPP_.cpp
 *
 *  Created on: Nov 30, 2012
 *      Author: benk
 */

#include "CS_Port.hpp"

#include <math.h>

CS_Port::CS_Port(const CS_ModuleDeclaration* module,
    int startLine, int startColumn,
    std::string name,
    int portIndex,
    MY_SMPO<const CS_DisciplineDeclaration> &discipline,
    CS_Range* range, PortType ty, bool isExternal): startLine_(startLine)
    , startColumn_(startColumn)
    , module_(module)
    , name_(name)
    , portIndex_(portIndex)
    , range_(range)
    , portType_(ty)
    , isReverseNumbered_(false)
    , isExternal_(isExternal)
    , discipline_(discipline)
{
  // nothing to do here
  //setVerb(6);
  VERILOG_PRINT_L3( verb(), "PN:" << name_  << " ty:" << portType_);
  if (range_) {
      int leftV = verilog_nearestInt( range_->getLeftExpr()->evalConst() );
      int rightV = verilog_nearestInt( range_->getRightExpr()->evalConst() );
      VERILOG_PRINT_L3( verb(), "leftV = " << range_->getLeftExpr()->evalConst() << " rightV=" << range_->getRightExpr()->evalConst());
      // swap values if left is grater
      if ( leftV > rightV ){
          isReverseNumbered_ = true;
          int tmpVal = leftV; leftV = rightV; rightV = tmpVal;
      }
      // left index can not be greater
      VERILOG_ERROR_COND_STOP(leftV > rightV, "In index range of the port '" << name_
          << "' left index can not be greater than the right limit", this);
      size_ = rightV - leftV + 1; // set this value correctly
      VERILOG_ERROR_COND_STOP(size_ <= 0, "In index range of the port '" << name_
          << "' size of the port must be at least one but it is " << size_ , this);
      offset_ = leftV;
      VERILOG_PRINT_L3( verb(), "size_ = " << size_ );
      VERILOG_PRINT_L3( verb(), "offset_ = " << offset_ );
      isArray_ = true;
  }
  else {
      size_ = 1;
      offset_ = 0;
      isArray_ = false;
  }
  this->setDiscipline(discipline_);
}

void CS_Port::setRange(CS_Range* range)
{
  range_ = range;
  // update the size of the port
  if (range_)
    {
      int leftV = (int) verilog_nearestInt( range_->getLeftExpr()->evalConst() );
      int rightV = (int)verilog_nearestInt( range_->getRightExpr()->evalConst() );
      // swap values if left is grater
      if ( leftV > rightV ){
          isReverseNumbered_ = true;
          int tmpVal = leftV; leftV = rightV; rightV = tmpVal;
      }
      // left index can not be greater
      VERILOG_ERROR_COND_STOP(leftV > rightV, "In index range of the port '" << name_
          << "' left index can not be greater than the right limit", this);
      size_ = rightV - leftV + 1; // set this value correctly
      VERILOG_ERROR_COND_STOP(size_ <= 0, "In index range of the port '" << name_
          << "' size of the port must be at least one but it is " << size_, this);
      offset_ = leftV;
      isArray_ = true;
    }
}
