/*
 * CS_VariableReference.cpp
 *
 *  Created on: Jan 10, 2013
 *      Author: benk
 */

#include "CS_VariableReference.hpp"
#include "CS_Number.hpp"

CS_VariableReference::CS_VariableReference(CS_VariableDeclaration *varObj,  CS_ExpressionBasis *indexExpr,
    const CS_ModuleDeclaration* module, int startLine, int startColumn, int derivativeIndex) :
    CS_IndexedIdentifier(varObj, indexExpr, module, startLine, startColumn),
    varObj_(varObj), derivativeIndex_(derivativeIndex)
{
  // nothing to do here
  VERILOG_ERROR_COND_STOP( varObj == 0 , " Identifier must be a variable ", this);
}


CS_AKDependency CS_VariableReference::propagateDependencies()
{
  // the dependencies
  CS_AKDependency ret_set;

  // we trance dependencies only for real variables
  if ( (varObj_->getType() != REAL) &&
       (varObj_->getType() != REAL_ARRAY))
    {
      // store the empty set and return
      setActVarDependency(ret_set);
      return ret_set;
    }

  if (this->getIndexerExpr() == 0)
    { // is not indexed, get the actual dependency list of this variable and return it
      ret_set.uniteDependencies( varObj_->getVariableScalarDependency() );
    }
  else
    { // is an indexed port expression
      if (this->isConstIndexingExpression() == false)
        { // the indexing expression contains variables/parameters/ports
          // therefore add all the ranges of possible dependencies
          // since the indexing is a general variable, we add all the dependencies from the array
          for (int i = 0; i < varObj_->getSize() ; i++ )
            ret_set.uniteDependencies( varObj_->getVariableArrayDependency(i) );
        }
      else
        { // eval in const mode the indexing expression
          [[maybe_unused]] int sysVarStart = varObj_->getAkSystemVarIndex();
          int indP = this->getConstIndexValue();
          VERILOG_ERROR_COND_STOP( indP - varObj_->getOffset() > varObj_->getSize() , "Index range exceeded" , this );
          ret_set.uniteDependencies( varObj_->getVariableArrayDependency(indP - varObj_->getOffset()) );
        }
    }
  // store it in the actual node
  setActVarDependency(ret_set);

  //TODO: debug
  //if (ret_set.getNrElements() > 0)
  //  std::cout << " var " << varObj_->getName() << " dep = " << ret_set.toString() << "\n";

  // we return the dependency set
  return ret_set;
}

double CS_VariableReference::evalConst() const
{
  // const evaluation can be done only for generate construct expansions
  // therefore we use the static methods of UT_GenVarHandle
  if ( UT_GenVarHandle::isActGenvarName( varObj_->getName() ) )
    // this is the
    return UT_GenVarHandle::getActGenvarValue();
  else
    { // throw an error, this case should not happen
      VERILOG_PRINT_L3(verb(),"Variable '" << varObj_->getName()
          << "' can not be evaluated statically ");
      VERILOG_ERROR_STOP( "Variables can not be evaluated statically", this );
      return 0.0;
    }
}

CS_ExpressionBasis* CS_VariableReference::doDifferentiate(int sysVarIndex, [[maybe_unused]] int *flag)
{
  // test if this dependent on the chosen system variable
  if (this->getActVarDependency().isSysVarIndexPresent(sysVarIndex) < 0)
    return 0;
  // create a variable expression that will return the derivative with respect to sysVarIndex
  CS_ExpressionBasis *retExpr = new CS_VariableReference( varObj_, this->getIndexerExpr(),
      getModule(), getStartLine(), getStartColumn(), sysVarIndex );
  // return the created object
  return retExpr;
}


CS_ExpressionBasis* CS_VariableReference::copy() {
  // copy the indexing
  CS_ExpressionBasis* indexer = (this->getIndexerExpr())?this->getIndexerExpr()->copy():0;

  if (  UT_GenVarHandle::isGenvarActive()
    && (varObj_->isVariableGenerate() )
    && (UT_GenVarHandle::isActGenvarName(varObj_->getName()))
    && (indexer == 0) )
    { // this means for the generate loops we replace the genvar variable references with concrete values
      return (new CS_Number( (int)verilog_nearestInt(UT_GenVarHandle::getActGenvarValue()), this->getModule(),
          this->getStartLine(), this->getStartColumn()));
    }
  else
    { // create new reference
      return (new CS_VariableReference(varObj_, indexer, this->getModule(),
          this->getStartLine(), this->getStartColumn()));
    }
}
