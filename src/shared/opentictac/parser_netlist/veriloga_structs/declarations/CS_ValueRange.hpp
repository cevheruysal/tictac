/*
 * CS_ValueRange.hpp
 *
 *  Created on: Nov 28, 2012
 *      Author: benk
 */

#ifndef CS_VALUERANGE_HPP_
#define CS_VALUERANGE_HPP_

#include "verilog_comp.hpp"
#include "CS_ExpressionBasis.hpp"
#include "CS_CompilerTreeNode.hpp"
#include "CS_ErrorInfo.hpp"

/** two types of constraints */
typedef enum {
    constr_from,
    constr_from_set,
    constr_exclude,
    constr_exclude_set
} ConstraintType;

/** the class that contains the constraints for the parameters
 * see value_range in LRM*/
class CS_ValueRange : public CS_CompilerTreeNode, public CS_ErrorInfo
{
public:

  /** Ctor for Parameter constraint objects
   * tha values v1, v2 are the (min, max) values ,
   * but for exclude the v1 id the exclude value */
  CS_ValueRange( const CS_ModuleDeclaration* module, int startLine, int startColumn,
      CS_ExpressionBasis* v1, CS_ExpressionBasis* v2 = 0, ConstraintType ct = constr_from,
      VarType valType = REAL, bool strictLeft = true, bool strictRight = true);

  /** empty Dtor */
  virtual ~CS_ValueRange() {;}

  /** returns the left (minimum) expression */
  CS_ExpressionBasis* getLeftExpr() { return minVal_; }
  const CS_ExpressionBasis* getLeftExpr() const { return minVal_; }

  /** returns the right (maximum) expression */
  CS_ExpressionBasis* getRightExpr() { return maxVal_; }
  const CS_ExpressionBasis* getRightExpr() const { return maxVal_; }

  /** method for the tree traversal */
  virtual void Accept(CS_CompilerVisitorBasis &visitor) { visitor.Visit( (CS_ValueRange &)(*this)); }

  /** returns the module */
  const CS_ModuleDeclaration* getModule() const { return module_;}

  /** line nr in the Verilog file*/
  int getStartLine() const { return startLine_; }

  /** column number in the Verilog file */
  int getStartColumn() const { return startColumn_; }

  /** returns the type of the constraint */
  ConstraintType getConstrinatType() const { return ctType_; }

  /** @returns the type of the range (INTEGER or REAL)*/
  VarType getValueType() const { return valType_; }

  /** @returns true if the left limit is strict */
  bool isStrictLeft() const {return strictLeft_; }
  /** @returns true if the right limit is strict */
  bool isStrictRight() const { return strictRight_; }

private:

  /** start line of the expression */
  const int startLine_;

  /** start column */
  const int startColumn_;

  /** the module that contains this expression */
  const CS_ModuleDeclaration* module_;

  /** Type type of the constraint */
  ConstraintType ctType_;
  /** type of the value (INTEGER OR REAL, no arrays allowed)*/
  VarType valType_;
  /** the minimal value or the exclude value */
  CS_ExpressionBasis* minVal_;
  /** the maximum value of the constraint */
  CS_ExpressionBasis* maxVal_;

  /** the values that indicate if the constraint boundary is strict or not */
  bool strictLeft_;
  bool strictRight_;
};

#endif /* CS_VALUERANGE_HPP_ */
