#include "IndependenCurrentSource.hpp"

IndependentCurrentSource::IndependentCurrentSource(MYINT       p1,
                                                   MYINT       p2,
                                                   MYREAL      dcValue,
                                                   std::string name,
                                                   MYREAL      mfact = 1.0)
    : Device(p1, p2, name), dcValue_(dcValue)
{
  this->setMFactor(mfact);
}


void
    IndependentCurrentSource::getAnalysisStamp(std::vector<MYINT> &          stampColumn,
                                               std::vector<MYINT> &          stampRow,
                                               std::vector<MatrixEntryType> &entryType,
                                               std::vector<MYREAL> &         potentialEntryValues,
                                               MYINT &                       nrStampsElements,
                                               [[maybe_unused]] const AN_Analysis *           analysis) const
{
 
 /* === HERE STARTS THE CODE OF ASSIGNMENT: 1 ==== */ 
 
 /* === HERE ENDS THE CODE OF ASSIGNMENT: 1 ==== */ 
}

void
    IndependentCurrentSource::getAnalysisStampRHS(std::vector<MYINT> &          stampColumn,
                                                  std::vector<MatrixEntryType> &entryType,
                                                  std::vector<MYREAL> &potentialEntryValues,
                                                  MYINT &              nrRHSStampsElements,
                                                  [[maybe_unused]] const AN_Analysis *  analysis) const
{
 
 /* === HERE STARTS THE CODE OF ASSIGNMENT: 1 ==== */ 
 
 /* === HERE ENDS THE CODE OF ASSIGNMENT: 1 ==== */ 
}

void
    IndependentCurrentSource::evalDevice([[maybe_unused]] const AN_Analysis *       analysis,
                                         [[maybe_unused]] AN_SimulationControllers *simulationController,
                                         [[maybe_unused]] std::vector<MYREAL> &     localVars,
                                         [[maybe_unused]] MYINT                     partiID,
                                         MYINT &                   nrMatrixStamp,
                                         MYINT &                   nrRHSStamp,
                                         std::vector<MYREAL> &     matrixValues,
                                         std::vector<MYREAL> &     rhsValues,
                                         [[maybe_unused]] MYINT &                   flags)
{
 
 /* === HERE STARTS THE CODE OF ASSIGNMENT: 1 ==== */ 
 
 /* === HERE ENDS THE CODE OF ASSIGNMENT: 1 ==== */ 
}

