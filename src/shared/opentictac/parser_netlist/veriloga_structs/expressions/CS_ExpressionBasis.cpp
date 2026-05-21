/*
 * CS_ExpressionBasis.cpp
 *
 *  Created on: Nov 28, 2012
 *      Author: benk
 */

#include "CS_ExpressionBasis.hpp"
#include "CS_ModuleDeclaration.hpp"

CS_ExpressionBasis::CS_ExpressionBasis(const CS_ModuleDeclaration* module, int startLine,
    int startColumn):
UT_VerboseClass("Expression"),
startLine_(startLine), startColumn_(startColumn), module_(module),
actType_(INTEGER), twin_(0)
{
  derivatives_.resize(0);
  derivSysVarIndex_.resize(0);
  nrDerivs_ = 0;
  twin_prefix_ = "";
  twinCount_ = -1;
}

void CS_ExpressionBasis::makeDiffentiation()
{
  // clear derivatives first
  derivatives_.clear();
  derivSysVarIndex_.clear();
  nrDerivs_ = 0;
  //
  int nrDeps = this->getActVarDependency().getNrElements();
  // for all system variables compute the derivatives and store them locally
  for (int i = 0; i < nrDeps; i++)
    {
      int sysVar = this->getActVarDependency().getSysVars()[i], tmpFlag;
      CS_ExpressionBasis *tmp_exp = this->doDifferentiate(sysVar, &tmpFlag);
      this->addDerivatives(tmp_exp, sysVar);
    }
}
