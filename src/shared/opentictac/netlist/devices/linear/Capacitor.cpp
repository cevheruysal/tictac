/*
 * Capacitor.cpp
 *
 *  Created on: Sep 12, 2013
 *      Author: benk
 */

#include "Capacitor.hpp"
#include "AN_Analysis.hpp"
#include "AN_SimulationControllers.hpp"
#include "AN_DTController.hpp"

Capacitor::Capacitor(MYINT p1, MYINT p2, MYREAL capacity,
    MYREAL mFact, std::string name):
Device(p1,p2, name)
{
  this->setMFactor(mFact);
  // Compute the conductance, incl. safeguards
  capacity_ = capacity;
  this->setNrDDTs(1); // we have one DDT
}

void Capacitor::getAnalysisStamp(
    std::vector<MYINT>& stampColumn,
    std::vector<MYINT>& stampRow,
    std::vector<MatrixEntryType>& entryType,
    std::vector<MYREAL>& potentialEntryValues,
    MYINT &nrStampsElements,
    const AN_Analysis *analysis) const
{
  // for DC we have no contribution
  if (analysis->getAnalysisType() == DC_TYPE)
    {
      nrStampsElements = 0;
      return;
    }
  /*=========== START STUDENT PROJECT A2 ===================== */
  nrStampsElements = 4;
  if (nrStampsElements > (MYINT)stampColumn.size()) stampColumn.resize(nrStampsElements, -1);
  if (nrStampsElements > (MYINT)stampRow.size()) stampRow.resize(nrStampsElements, -1);
  if (nrStampsElements > (MYINT)entryType.size()) entryType.resize(nrStampsElements);
  if (nrStampsElements > (MYINT)potentialEntryValues.size()) potentialEntryValues.resize(nrStampsElements, 0.0);
  // TODO: the entry estimation could have been done otherwise ....
  stampColumn[0] = 0; stampRow[0] = 0; entryType[0] = DYNAMIC_ONLY; potentialEntryValues[0] = 1E+6*this->capacity_;
  stampColumn[1] = 1; stampRow[1] = 0; entryType[1] = DYNAMIC_ONLY; potentialEntryValues[1] = 1E+6*this->capacity_;
  stampColumn[2] = 0; stampRow[2] = 1; entryType[2] = DYNAMIC_ONLY; potentialEntryValues[2] = 1E+6*this->capacity_;
  stampColumn[3] = 1; stampRow[3] = 1; entryType[3] = DYNAMIC_ONLY; potentialEntryValues[3] = 1E+6*this->capacity_;
  /*=========== END STUDENT PROJECT A2 ===================== */
}


void Capacitor::getAnalysisStampRHS(
     std::vector<MYINT>& stampColumn,
     std::vector<MatrixEntryType>& entryType,
     std::vector<MYREAL>& potentialEntryValues,
     MYINT &nrRHSStampsElements,
     const AN_Analysis *analysis) const
{
  // for DC we have no contribution
  if (analysis->getAnalysisType() == DC_TYPE)
    {
      nrRHSStampsElements = 0;
      return;
    }
  /*=========== START STUDENT PROJECT A2 ===================== */
  nrRHSStampsElements = 2;
  if (nrRHSStampsElements > (MYINT)stampColumn.size()) stampColumn.resize(nrRHSStampsElements, -1);
  if (nrRHSStampsElements > (MYINT)entryType.size()) entryType.resize(nrRHSStampsElements);
  if (nrRHSStampsElements > (MYINT)potentialEntryValues.size())
    potentialEntryValues.resize(nrRHSStampsElements, 0.0);
  // entry estimation for RHS is not important ...
  stampColumn[0]          = 0;
  entryType[0]            = DYNAMIC_ONLY;
  potentialEntryValues[0] = 1E+6 * this->capacity_;
  stampColumn[1]          = 1;
  entryType[1]            = DYNAMIC_ONLY;
  potentialEntryValues[1] = 1E+6 * this->capacity_;
  /*=========== END STUDENT PROJECT A2 ===================== */
}


void Capacitor::evalDevice(
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
  // for DC analysis no stamping
  if (analysis->getAnalysisType() == DC_TYPE)
    {
      nrMatrixStamp = 0;
      nrRHSStamp = 0;
      return;
    }

  // transient analysis stamping
  if (analysis->getAnalysisType() == TRAN_TYPE)
    {
      nrMatrixStamp = 4;      nrRHSStamp = 2;
      MYINT ddtIndex = this->getDDTIndex(0,partiID);
      MYREAL mfact = (MYREAL)this->getMFactor();
      // First, always set the argument of DDT
      simulationController->getDDTController()->setDDTX(ddtIndex, capacity_*(localVars[0]-localVars[1]));
      // get the alpha coefficient
      MYREAL alpha = simulationController->getDDTController()->getAlpha(ddtIndex);
      MYREAL ddt   = simulationController->getDDTController()->getDDT(ddtIndex);

      /*=========== START STUDENT PROJECT A2 ===================== */

      //std::cout << " c stamp=" << (mfact * alpha * capacity_) << " capacity_=" << capacity_ << " alpha=" << alpha << " mfact=" << mfact << "\n";
      // fill the matrix
      matrixValues[0] =  mfact * alpha * capacity_;
      matrixValues[1] = -mfact * alpha * capacity_;
      matrixValues[2] = -mfact * alpha * capacity_;
      matrixValues[3] =  mfact * alpha * capacity_;
      // fill in RHS
      rhsValues[0] =  mfact * ddt;
      rhsValues[1] = -mfact * ddt;

      /*=========== END STUDENT PROJECT A2 ===================== */

      return;
    }

}
