/*
 * Resistor.cpp
 *
 *  Created on: Sep 12, 2013
 *      Author: benk
 */

#include "Resistor.hpp"
#include "AN_Analysis.hpp"
#include "AN_SimulationControllers.hpp"

Resistor::Resistor(MYINT p1, MYINT p2, MYREAL resistance,
    MYREAL mFact, std::string name):
Device(p1,p2, name)
{
  this->setMFactor(mFact);
  // Compute the conductance, incl. safeguards
  conductance_ = 1.0/SIMMAX(SIMABS(resistance),1E-10);
}

void Resistor::getAnalysisStamp(
    std::vector<MYINT>& stampColumn,
    std::vector<MYINT>& stampRow,
    std::vector<MatrixEntryType>& entryType,
    std::vector<MYREAL>& potentialEntryValues,
    MYINT &nrStampsElements,
    [[maybe_unused]] const AN_Analysis *analysis) const
{
  /*=========== START STUDENT PROJECT A1 ===================== */
  // return the local element stamp
  nrStampsElements = 4;
  if (nrStampsElements > (MYINT)stampColumn.size()) stampColumn.resize(nrStampsElements, -1);
  if (nrStampsElements > (MYINT)stampRow.size()) stampRow.resize(nrStampsElements, -1);
  if (nrStampsElements > (MYINT)entryType.size()) entryType.resize(nrStampsElements);
  if (nrStampsElements > (MYINT)potentialEntryValues.size()) potentialEntryValues.resize(nrStampsElements, 0.0);
  stampColumn[0] = 0; stampRow[0] = 0; entryType[0] = STATIC_ONLY; potentialEntryValues[0] = conductance_;
  stampColumn[1] = 1; stampRow[1] = 0; entryType[1] = STATIC_ONLY; potentialEntryValues[1] = conductance_;
  stampColumn[2] = 0; stampRow[2] = 1; entryType[2] = STATIC_ONLY; potentialEntryValues[2] = conductance_;
  stampColumn[3] = 1; stampRow[3] = 1; entryType[3] = STATIC_ONLY; potentialEntryValues[3] = conductance_;
 
/*
 *   stampColumn.push_back(0);
 *   stampRow.push_back(0);
 *   entryType.push_back(STATIC_ONLY);
 *   potentialEntryValues.push_back(conductance_);
 * 
 *   stampColumn.push_back(1);
 *   stampRow.push_back(0);
 *   entryType.push_back(STATIC_ONLY);
 *   potentialEntryValues.push_back(conductance_);
 * 
 *   stampColumn.push_back(0);
 *   stampRow.push_back(1);
 *   entryType.push_back(STATIC_ONLY);
 *   potentialEntryValues.push_back(conductance_);
 * 
 *   stampColumn.push_back(1);
 *   stampRow.push_back(1);
 *   entryType.push_back(STATIC_ONLY);
 *   potentialEntryValues.push_back(conductance_);
 */
  /*=========== END STUDENT PROJECT A1 ===================== */
}

void Resistor::getAnalysisStampRHS(
     std::vector<MYINT>& stampColumn,
     std::vector<MatrixEntryType>& entryType,
     std::vector<MYREAL>& potentialEntryValues,
     MYINT &nrRHSStampsElements,
     [[maybe_unused]] const AN_Analysis *analysis) const
{
 
  /*=========== START STUDENT PROJECT A1 ===================== */
  // return the local element stamp
  nrRHSStampsElements = 2;
  if (nrRHSStampsElements > (MYINT)stampColumn.size()) stampColumn.resize(nrRHSStampsElements, -1);
  if (nrRHSStampsElements > (MYINT)entryType.size()) entryType.resize(nrRHSStampsElements);
  if (nrRHSStampsElements > (MYINT)potentialEntryValues.size()) potentialEntryValues.resize(nrRHSStampsElements, 0.0);
  stampColumn[0] = 0; entryType[0] = STATIC_ONLY; potentialEntryValues[0] = conductance_;
  stampColumn[1] = 1; entryType[1] = STATIC_ONLY; potentialEntryValues[1] = conductance_;
  /*=========== END STUDENT PROJECT A1 ===================== */
}


void Resistor::evalDevice(
    // ---------- INPUTS --------
    [[maybe_unused]] const AN_Analysis *analysis,
    [[maybe_unused]] AN_SimulationControllers* simulationController,
    std::vector<MYREAL> &localVars,
    [[maybe_unused]] MYINT partiID,
    // ----------- OUTPUTS ----------
    MYINT &nrMatrixStamp,
    MYINT &nrRHSStamp,
    std::vector<MYREAL> &matrixValues,
    std::vector<MYREAL> &rhsValues,
    [[maybe_unused]] MYINT &flags
    )
{
 
  /*=========== START STUDENT PROJECT A1 ===================== */
  // this is the same stamp for DC and for TRAN analysis
  MYREAL mfact = (MYREAL)this->getMFactor();
  nrRHSStamp = 2;
  nrMatrixStamp = 4;
  matrixValues[0] =  conductance_ * mfact;
  matrixValues[1] = -conductance_ * mfact;
  matrixValues[2] = -conductance_ * mfact;
  matrixValues[3] =  conductance_ * mfact;
  rhsValues[0] =  conductance_*(localVars[0] - localVars[1]) * mfact;
  rhsValues[1] = -conductance_*(localVars[0] - localVars[1]) * mfact;
  /*=========== END STUDENT PROJECT A1 ===================== */
}
