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

 
 /* === HERE STARTS THE CODE OF ASSIGNMENT: 2 ==== */ 

 
 /* === HERE ENDS THE CODE OF ASSIGNMENT: 2 ==== */ 
}


void Inductor::getAnalysisStampRHS(
     std::vector<MYINT>& stampColumn,
     std::vector<MatrixEntryType>& entryType,
     std::vector<MYREAL>& potentialEntryValues,
     MYINT &nrRHSStampsElements,
     const AN_Analysis *analysis) const
{

 
 /* === HERE STARTS THE CODE OF ASSIGNMENT: 2 ==== */ 

 
 /* === HERE ENDS THE CODE OF ASSIGNMENT: 2 ==== */ 
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

 
 /* === HERE STARTS THE CODE OF ASSIGNMENT: 2 ==== */ 
 
 /* === HERE ENDS THE CODE OF ASSIGNMENT: 2 ==== */ 
}
