/*
 * CS_CompilerTreeNode.hpp
 *
 *  Created on: Jan 3, 2013
 *      Author: benk
 */

#ifndef COMPILERTREENODE_HPP_
#define COMPILERTREENODE_HPP_

#include "verilog_comp.hpp"
#include "CS_CompilerVisitorBasis.hpp"
#include "CS_AttributeInstance.hpp"

/** this class is the super class for all data structure entry, such that we
 * can traverse all the data structures created from a Verilog module with our Visitor (similar to Verific) concept.
 * This class has no direct reference in the LRM manual is mainly for the visitor concept
 * It contains a "attribute_instance" in LRM for each node in the date structure.
 *
 * In this class we also store informations regarding the C code generation */
class CS_CompilerTreeNode {

public :

  /** Ctor*/
  CS_CompilerTreeNode() {;}

  /** empty Dtor */
  virtual ~CS_CompilerTreeNode() {;}

  /** This is the key method for the traversal.
   * For each specific node this function calls the "Visit" method of the CompilerVisitorBasis.
   * In this function, each element of the tree (which are usually instructions, declarations, expressions, ...)
   * should call the "Visit" method of the visitor, e.g., visitor->Visit( (NodeType &)(*this)); */
  virtual void Accept(CS_CompilerVisitorBasis &visitor) = 0;

  /** returns the attribute map */
  CS_AttributeInstance& getAttributeMap() { return attributes_; }
  const CS_AttributeInstance& getAttributeMap() const { return attributes_; }

private:

  /** attribute map */
  CS_AttributeInstance attributes_;

};


#endif /* COMPILERTREENODE_HPP_ */
