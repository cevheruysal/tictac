/*
 * CS_HierarchSysParamFunctions.hpp
 *
 *  Created on: Mar 5, 2013
 *      Author: benk
 */

#ifndef CS_HIERARCHSYSPARAMFUNCTIONS_HPP_
#define CS_HIERARCHSYSPARAMFUNCTIONS_HPP_

#include "CS_ExpressionBasis.hpp"

typedef enum {
  VERILOG_HierarchSysParamFunctions_mfactor = 0,
  VERILOG_HierarchSysParamFunctions_xposition = 1,
  VERILOG_HierarchSysParamFunctions_yposition = 2,
  VERILOG_HierarchSysParamFunctions_angle = 3,
  VERILOG_HierarchSysParamFunctions_hflip = 4,
  VERILOG_HierarchSysParamFunctions_vflip = 5
} HierarchSysParamFuncType;

/** Represents the hierarchical functions
 *
 * see hierarchical_system_parameter_functions in LRM */
class CS_HierarchSysParamFunctions : public CS_ExpressionBasis
{
public:

  CS_HierarchSysParamFunctions( HierarchSysParamFuncType ft,
      const CS_ModuleDeclaration* module,
      int startLine, int startColumn);

  virtual ~CS_HierarchSysParamFunctions() {;}

   /** returns the class of the function */
   virtual int getClasses() const { return (int)PARAMETER; }

   /** return the type of the parameter */
   virtual int getTypes() const { return (int)REAL;  }

   /** no constant evaluation */
   virtual double evalConst() const {
     VERILOG_PRINT_L3(verb()," CS_HierarchSysParamFunctions::evalConst should not be called");
     VERILOG_ERROR_STOP( "Internal Error", this );
     return 0.0;
    }

    /** method for the tree traversal */
    virtual void Accept(CS_CompilerVisitorBasis &visitor)
    {
      visitor.Visit( (CS_HierarchSysParamFunctions &)(*this));
    }

    /** no dependencies generated here  */
    virtual CS_AKDependency propagateDependencies()
      {
        // store and return empty dependencies
        CS_AKDependency ret_tmp;
        setActVarDependency(ret_tmp);
        return ret_tmp;
      }

    /** function that creates a deep copy of the expression itself */
    virtual CS_ExpressionBasis* copy() {
        // create new object
        return (new CS_HierarchSysParamFunctions(functionType_, this->getModule(),
          this->getStartLine(), this->getStartColumn()));
    }

    /** return the type of the function */
    HierarchSysParamFuncType getFunctionType() const { return functionType_; }

private :

  /** the type of the function */
  HierarchSysParamFuncType functionType_;

};

#endif /* CS_HIERARCHSYSPARAMFUNCTIONS_HPP_ */
