/*
 * CS_Range.hpp
 *
 *  Created on: Dec 10, 2012
 *      Author: benk
 */

#ifndef CS_RANGE_HPP_
#define CS_RANGE_HPP_

#include "verilog_comp.hpp"
#include "CS_ExpressionBasis.hpp"
#include "CS_CompilerTreeNode.hpp"

/** the class that contains the index range declarations of a port and real
 * see range in LRM */
class CS_Range : public CS_CompilerTreeNode
{
public:

  /** Ctor for the Range object */
  CS_Range(CS_ExpressionBasis *exprL, CS_ExpressionBasis *exprR);

  /** empty Dtor */
  virtual ~CS_Range() {;}

  /** returns the left expression of the range */
  CS_ExpressionBasis* getLeftExpr() { return exprLeft_; }
  const CS_ExpressionBasis* getLeftExpr() const { return exprLeft_; }

  /** returns the right expression of the range */
  CS_ExpressionBasis* getRightExpr() { return exprRight_; }
  const CS_ExpressionBasis* getRightExpr() const { return exprRight_; }

  /** checks if the ranges are the same */
  bool isEqual(const CS_Range *other) const {
    return (    (exprLeft_->evalConst() == other->exprLeft_->evalConst())
             && (exprRight_->evalConst() == other->exprRight_->evalConst()) );
  }

  /** method for the tree traversal */
  virtual void Accept(CS_CompilerVisitorBasis &visitor) { visitor.Visit( (CS_Range &)(*this)); }

private:

  /** left expression of the range*/
  CS_ExpressionBasis *exprLeft_;

  /** right expression of the range */
  CS_ExpressionBasis *exprRight_;
};

#endif /* CS_RANGE_HPP_ */
