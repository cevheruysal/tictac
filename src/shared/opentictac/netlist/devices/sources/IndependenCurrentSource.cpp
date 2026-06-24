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
 
  /*=========== START STUDENT PROJECT A1 ===================== */
  nrStampsElements = 4;
  if (nrStampsElements > (MYINT)stampColumn.size()) stampColumn.resize(nrStampsElements, -1);
  if (nrStampsElements > (MYINT)stampRow.size()) stampRow.resize(nrStampsElements, -1);
  if (nrStampsElements > (MYINT)entryType.size()) entryType.resize(nrStampsElements);
  if (nrStampsElements > (MYINT)potentialEntryValues.size()) potentialEntryValues.resize(nrStampsElements, 0.0);
  stampColumn[0] = 0; stampRow[0] = 0; entryType[0] = STATIC_ONLY; potentialEntryValues[0] = 0.0;
  stampColumn[1] = 1; stampRow[1] = 0; entryType[1] = STATIC_ONLY; potentialEntryValues[1] = 0.0;
  stampColumn[2] = 0; stampRow[2] = 1; entryType[2] = STATIC_ONLY; potentialEntryValues[2] = 0.0;
  stampColumn[3] = 1; stampRow[3] = 1; entryType[3] = STATIC_ONLY; potentialEntryValues[3] = 0.0;
      /*
       * nrStampsElements = 0;
       * return;
       */
  /*=========== END STUDENT PROJECT A1 ===================== */
}

void
    IndependentCurrentSource::getAnalysisStampRHS(std::vector<MYINT> &          stampColumn,
                                                  std::vector<MatrixEntryType> &entryType,
                                                  std::vector<MYREAL> &potentialEntryValues,
                                                  MYINT &              nrRHSStampsElements,
                                                  [[maybe_unused]] const AN_Analysis *  analysis) const
{
 
  /*=========== START STUDENT PROJECT A1 ===================== */
  // return the local element stamp
  nrRHSStampsElements = 2;
  if (nrRHSStampsElements > (MYINT)stampColumn.size()) stampColumn.resize(nrRHSStampsElements, -1);
  if (nrRHSStampsElements > (MYINT)entryType.size()) entryType.resize(nrRHSStampsElements);
  if (nrRHSStampsElements > (MYINT)potentialEntryValues.size())
    potentialEntryValues.resize(nrRHSStampsElements, 0.0);
  stampColumn[0]          = 0;
  entryType[0]            = STATIC_ONLY;
  potentialEntryValues[0] = dcValue_;
  stampColumn[1]          = 1;
  entryType[1]            = STATIC_ONLY;
  potentialEntryValues[1] = dcValue_;
  /*
   *   stampColumn.push_back(0);
   *   entryType.push_back(STATIC_ONLY);
   *   potentialEntryValues.push_back(dcValue_);
   *
   *   stampColumn.push_back(1);
   *   entryType.push_back(STATIC_ONLY);
   *   potentialEntryValues.push_back(dcValue_);
   */
  /*=========== END STUDENT PROJECT A1 ===================== */
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
  /*=========== START STUDENT PROJECT A1 ===================== */
  MYREAL mfact  = this->getMFactor();
  nrRHSStamp    = 2;
  nrMatrixStamp = 4;
  // nrMatrixStamp = 0;
  rhsValues[0] = mfact * dcValue_;
  rhsValues[1] = -mfact * dcValue_;
 
  matrixValues[0] = 0.0;
  matrixValues[1] = 0.0;
  matrixValues[2] = 0.0;
  matrixValues[3] = 0.0;
  /*=========== END STUDENT PROJECT A1 ===================== */
}

