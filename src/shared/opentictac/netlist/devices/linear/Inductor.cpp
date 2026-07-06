/*
 * Inductor.cpp
 *
 *  Created on: Sep 12, 2013
 *      Author: benk
 */

#include "Inductor.hpp"
#include "AN_Analysis.hpp"
#include "AN_SimulationControllers.hpp"
#include "AN_DTController.hpp"

Inductor::Inductor(MYINT p1, MYINT p2, MYREAL inductance,
    MYREAL mFact, std::string name):
Device(p1,p2,name)
{
  this->setMFactor(mFact);
  inductance_ = inductance;
  currentID_.resize(1,-1);
  this->setNrDDTs(1); // we have one DDT
}

/** see base class */
void Inductor::getAnalysisStamp(
    std::vector<MYINT>& stampColumn,
    std::vector<MYINT>& stampRow,
    std::vector<MatrixEntryType>& entryType,
    std::vector<MYREAL>& potentialEntryValues,
    MYINT &nrStampsElements,
    const AN_Analysis *analysis) const
{
  /*=========== START STUDENT PROJECT A2 ===================== */
  // return the local element stamp
  // for DC we have less contribution
  if (analysis->getAnalysisType() == DC_TYPE)
    nrStampsElements = 4;
  else
    nrStampsElements = 5;

  // resize the vectors corectly
  if (nrStampsElements > (MYINT)stampColumn.size()) stampColumn.resize(nrStampsElements, -1);
  if (nrStampsElements > (MYINT)stampRow.size()) stampRow.resize(nrStampsElements, -1);
  if (nrStampsElements > (MYINT)entryType.size()) entryType.resize(nrStampsElements);
  if (nrStampsElements > (MYINT)potentialEntryValues.size()) potentialEntryValues.resize(nrStampsElements, 0.0);
 
  stampColumn[0] = 0; stampRow[0] = 2; entryType[0] = STATIC_ONLY; potentialEntryValues[0] = 1.0;
  stampColumn[1] = 1; stampRow[1] = 2; entryType[1] = STATIC_ONLY; potentialEntryValues[1] = 1.0;
  stampColumn[2] = 2; stampRow[2] = 0; entryType[2] = STATIC_ONLY; potentialEntryValues[2] = 1.0;
  stampColumn[3] = 2; stampRow[3] = 1; entryType[3] = STATIC_ONLY; potentialEntryValues[3] = 1.0;
  if (analysis->getAnalysisType() == DC_TYPE)
    {
      return;
    }
  if ((analysis->getAnalysisType() == TRAN_TYPE) && (this->getInductance() > 0.0))
    { // add this only when we have transient analysis or inductance is not zero
      stampColumn[4] = 2; stampRow[4] = 2; entryType[4] = DYNAMIC_ONLY;
      potentialEntryValues[4] = SIMABS(this->inductance_);
    }
  /*=========== END STUDENT PROJECT A2 ===================== */
}


void Inductor::getAnalysisStampRHS(
     std::vector<MYINT>& stampColumn,
     std::vector<MatrixEntryType>& entryType,
     std::vector<MYREAL>& potentialEntryValues,
     MYINT &nrRHSStampsElements,
     const AN_Analysis *analysis) const
{
  /*=========== START STUDENT PROJECT A2 ===================== */
  // for DC we have less contribution
  if (analysis->getAnalysisType() == DC_TYPE)
      nrRHSStampsElements = 2;
  else
    nrRHSStampsElements = 3;

  // resize the vector correctly
  if (nrRHSStampsElements > (MYINT)stampColumn.size()) stampColumn.resize(nrRHSStampsElements, -1);
  if (nrRHSStampsElements > (MYINT)entryType.size()) entryType.resize(nrRHSStampsElements);
  if (nrRHSStampsElements > (MYINT)potentialEntryValues.size()) potentialEntryValues.resize(nrRHSStampsElements, 0.0);
 
  // entry estimation for RHS is not important ...
  stampColumn[0] = 0; entryType[0] = STATIC_ONLY; potentialEntryValues[0] = 1.0;
  stampColumn[1] = 1; entryType[1] = STATIC_ONLY; potentialEntryValues[1] = 1.0;
  if (analysis->getAnalysisType() == TRAN_TYPE)
    { stampColumn[2] = 2; entryType[2] = DYNAMIC_ONLY; potentialEntryValues[2] = SIMABS(this->inductance_); }
  /*=========== END STUDENT PROJECT A2 ===================== */
}


void Inductor::evalDevice(
    // ---------- INPUTS --------
    const AN_Analysis *analysis,
    AN_SimulationControllers* simulationController,
    std::vector<MYREAL> &localVars,
    MYINT partiID,
    // ----------- OUTPUTS ----------
    MYINT &nrMatrixStamp,
    MYINT &nrRHSStamp,
    std::vector<MYREAL> &matrixValues,
    std::vector<MYREAL> &rhsValues,
    [[maybe_unused]] MYINT &flags
    )
{
  /*=========== START STUDENT PROJECT A2 ===================== */
  // DC Analysis Stamp is always there
  MYREAL mfact = (MYREAL)this->getMFactor();
  nrRHSStamp = 3;
  nrMatrixStamp = 4;
  matrixValues[0] =  1.0 * mfact;
  matrixValues[1] = -1.0 * mfact;
  matrixValues[2] =  1.0 * mfact;
  matrixValues[3] = -1.0 * mfact;
  rhsValues[0] =  localVars[2] * mfact;
  rhsValues[1] = -localVars[2] * mfact;
  rhsValues[2] = (localVars[0] - localVars[1]) * mfact; // TODO: this should be only in TRAN mode

 
  if (analysis->getAnalysisType() == TRAN_TYPE)
    {
      // for TRAN analysis there is one more entry
      nrMatrixStamp = 5;
      MYINT ddtIndex = this->getDDTIndex(0,partiID);
      // First, always set the argument of DDT
      simulationController->getDDTController()->setDDTX(ddtIndex, inductance_*(localVars[2]));
      // get the alpha coefficient
      MYREAL alpha = simulationController->getDDTController()->getAlpha(ddtIndex);
      MYREAL ddt   = simulationController->getDDTController()->getDDT(ddtIndex);
      // the derivative w.r.t to the current variable
      matrixValues[4] = -inductance_ * alpha * mfact;
      // add the dynamic current part ddt(L*I)
      rhsValues[2] +=  -ddt * mfact;
    }
  /*=========== END STUDENT PROJECT A2 ===================== */
}
