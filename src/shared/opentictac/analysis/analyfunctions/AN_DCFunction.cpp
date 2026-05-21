/*
 * AN_DCFunction.cpp
 *
 *  Created on: Oct 23, 2013
 *      Author: benk
 */

#include "AN_DCFunction.hpp"
#include "LA_MatrixNetlistMap.hpp"

AN_DCFunction::AN_DCFunction(AN_DCAnalysis *dcAnalysis) :
AN_AnalysisFunctionBase(dcAnalysis), dcAnalysis_(dcAnalysis)
{
  //this->setVerb(6);
}

AN_DCFunction::~AN_DCFunction()
{
  // no dynamic allocation, nothing to solve here
}

void AN_DCFunction::prepareAnalysisRun()
{
  // set the in and output dimensions
  this->inDim_ = dcAnalysis_->getRealSolver(0)->getSolverMatrix()->getNrRows();
  this->outDim_ = dcAnalysis_->getRealSolver(0)->getSolverMatrix()->getNrColumns();

  // initialize the vector of tolerances
  this->absTol_.resize(this->outDim_, 0.0);
  this->relTol_.resize(this->outDim_, 0.0);
  const IN_SimOptions* simopts = dcAnalysis_->getSimulationOption();
  for (MYINT i = 0; i < this->outDim_; i++)
    {
      MYINT unknownId = dcAnalysis_->getRealSolver(0)->getSolverMatrix()->getMatrixMap()->getColIndexForUnkID(i);
      if (unknownId > dcAnalysis_->getGlobalNetlist()->getNrNodesNoGnd()) {
          // current variable
          absTol_[i] = simopts->getDCNewtonTols().absTolCurrents_;
          relTol_[i] = simopts->getDCNewtonTols().relTolCurrents_;
        }
      else { // voltage variable
          absTol_[i] = simopts->getDCNewtonTols().absTolVoltages_;
          relTol_[i] = simopts->getDCNewtonTols().relTolVoltages_;
      }
    }

  // call the base class init method
  this->prepareAnalysis();
}

void AN_DCFunction::eval(
    [[maybe_unused]] MYINT evalIndex,
    [[maybe_unused]] std::vector<MYREAL> &u,
    [[maybe_unused]] std::vector<MYREAL> &res,
    [[maybe_unused]] std::vector<MYREAL> &absContribs)
{
  // actual is not implemented and is also not used
  // later it could be
  SIM_ERROR_STOP("NS_Function::eval in AN_DCFunction is not implemented");
}


/** see NS_Function base class */
LA_MatrixBasis<MYREAL>* AN_DCFunction::evalWithDeriv(
    [[maybe_unused]] MYINT evalIndex,
    std::vector<MYREAL> &u,
    std::vector<MYREAL> &res,
    std::vector<MYREAL> &absContribs)
{

  LA_MatrixBasis<MYREAL>* dcMatrix = dcAnalysis_->getRealSolver(0)->getSolverMatrix();
  Netlist* netlist = dcAnalysis_->getGlobalNetlist();
  MYINT flags;

  // test if u, res, and absContribs are correctly sized
  SIM_ERROR_COND_STOP(u.size()<boost::numeric_cast<decltype(u.size())>(this->getInputDimension())         ,"DC eval u.size()=" << u.size() <<" inDim=" << getInputDimension());
  SIM_ERROR_COND_STOP(res.size()<boost::numeric_cast<decltype(res.size())>(this->getOutputDimension())         ,"DC eval res.size()=" << res.size() <<" outDim=" << getOutputDimension());
  SIM_ERROR_COND_STOP(absContribs.size()<boost::numeric_cast<decltype(absContribs.size())>(this->getOutputDimension()) ,"DC eval absContribs.size()=" << absContribs.size() <<" outDim=" << getOutputDimension());

  // =========== main assembly loop is given in the base class's method =================
  this->assemblyMatrixAndRHS(dcMatrix, netlist, u, res, absContribs, flags);

  // returns the DC matrix
  return dcMatrix;
}
