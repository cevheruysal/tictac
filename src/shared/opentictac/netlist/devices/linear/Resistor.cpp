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
 
 /* === HERE STARTS THE CODE OF ASSIGNMENT: 1 ==== */ 
 
 /* === HERE ENDS THE CODE OF ASSIGNMENT: 1 ==== */ 
}

void Resistor::getAnalysisStampRHS(
     std::vector<MYINT>& stampColumn,
     std::vector<MatrixEntryType>& entryType,
     std::vector<MYREAL>& potentialEntryValues,
     MYINT &nrRHSStampsElements,
     [[maybe_unused]] const AN_Analysis *analysis) const
{
 
 /* === HERE STARTS THE CODE OF ASSIGNMENT: 1 ==== */ 
 
 /* === HERE ENDS THE CODE OF ASSIGNMENT: 1 ==== */ 
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
 
 /* === HERE STARTS THE CODE OF ASSIGNMENT: 1 ==== */ 
 
 /* === HERE ENDS THE CODE OF ASSIGNMENT: 1 ==== */ 
}
