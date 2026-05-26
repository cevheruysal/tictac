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
  nrStampsElements = 0;
}

void
    IndependentCurrentSource::getAnalysisStampRHS(std::vector<MYINT> &          stampColumn,
                                                   std::vector<MatrixEntryType> &entryType,
                                                   std::vector<MYREAL> &potentialEntryValues,
                                                   MYINT &              nrRHSStampsElements,
                                                   [[maybe_unused]] const AN_Analysis *  analysis) const
{
  nrRHSStampsElements = 2;
  if (nrRHSStampsElements > (MYINT)stampColumn.size()) stampColumn.resize(nrRHSStampsElements, -1);
  if (nrRHSStampsElements > (MYINT)entryType.size()) entryType.resize(nrRHSStampsElements);
  if (nrRHSStampsElements > (MYINT)potentialEntryValues.size()) potentialEntryValues.resize(nrRHSStampsElements, 0.0);

  stampColumn[0] = 0; entryType[0] = STATIC_ONLY; potentialEntryValues[0] = 1.0;
  stampColumn[1] = 1; entryType[1] = STATIC_ONLY; potentialEntryValues[1] = 1.0;
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
  MYREAL mfact = (MYREAL)this->getMFactor();
  nrMatrixStamp = 0;
  nrRHSStamp = 2;

  rhsValues[0] = dcValue_ * mfact;
  rhsValues[1] = -dcValue_ * mfact;
}

