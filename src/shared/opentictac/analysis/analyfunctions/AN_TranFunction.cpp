/*
 * AN_TranFunction.cpp
 *
 *  Created on: Oct 30, 2013
 *      Author: benk
 */

#include "AN_TranFunction.hpp"

AN_TranFunction::AN_TranFunction(AN_TranAnalysis *tranAnalysis):
AN_AnalysisFunctionBase(tranAnalysis), tranAnalysis_(tranAnalysis)
{
  // Initialize the fields

}

AN_TranFunction::~AN_TranFunction() {
  // TODO: eventually deallocate dynamically allocated memory
}

void AN_TranFunction::prepareAnalysisRun()
{
  // set the in and output dimensions
  this->inDim_ = tranAnalysis_->getRealSolver(0)->getSolverMatrix()->getNrRows();
  this->outDim_ = tranAnalysis_->getRealSolver(0)->getSolverMatrix()->getNrColumns();

  // initialize the vector of tolerances
  this->absTol_.resize(this->outDim_, 0.0);
  this->relTol_.resize(this->outDim_, 0.0);
  const IN_SimOptions* simopts = tranAnalysis_->getSimulationOption();
  for (MYINT i = 0; i < this->outDim_; i++)
    {
      MYINT unknownId = tranAnalysis_->getRealSolver(0)->getSolverMatrix()->getMatrixMap()->getColIndexForUnkID(i);
      if (unknownId > tranAnalysis_->getGlobalNetlist()->getNrNodesNoGnd()) {
          // current variable
          absTol_[i] = simopts->getTranNewtonTols().absTolCurrents_;
          relTol_[i] = simopts->getTranNewtonTols().relTolCurrents_;
        }
      else { // voltage variable
          absTol_[i] = simopts->getTranNewtonTols().absTolVoltages_;
          relTol_[i] = simopts->getTranNewtonTols().relTolVoltages_;
      }
    }

  // call the method from the super class
  this->prepareAnalysis();
}


void AN_TranFunction::eval(
    [[maybe_unused]] MYINT evalIndex,
    [[maybe_unused]] std::vector<MYREAL> &u,
    [[maybe_unused]] std::vector<MYREAL> &res,
    [[maybe_unused]] std::vector<MYREAL> &absContribs)
{
  // actual is not implemented and is also not used
  // later it could be
  SIM_ERROR_STOP("NS_Function::eval in AN_TranFunction is not implemented");
}

LA_MatrixBasis<MYREAL>* AN_TranFunction::evalWithDeriv(
    [[maybe_unused]] MYINT evalIndex,
    std::vector<MYREAL> &u,
    std::vector<MYREAL> &res,
    std::vector<MYREAL> &absContribs)
{
   // here we always work with the global solver ... index "0"
   LA_MatrixBasis<MYREAL>* tranMatrix = tranAnalysis_->getRealSolver(0)->getSolverMatrix();
   Netlist* netlist = tranAnalysis_->getGlobalNetlist();
   MYINT flags;

   // test if u, res, and absContribs are correctly sized
   SIM_ERROR_COND_STOP(u.size()< boost::numeric_cast<decltype(u.size())>(this->getInputDimension())    ,"TRAN eval u.size()=" << u.size() <<" inDim=" << getInputDimension());
   SIM_ERROR_COND_STOP(res.size()<boost::numeric_cast<decltype(res.size())>(this->getOutputDimension())   ,"TRAN eval res.size()=" << res.size() <<" outDim=" << getOutputDimension());
   SIM_ERROR_COND_STOP(absContribs.size()< boost::numeric_cast<decltype(absContribs.size())>(getOutputDimension()) ,"TRAN eval absContribs.size()=" << absContribs.size() <<" outDim=" << getOutputDimension());

   // =========== main assembly loop is given in the base class's method =================
   this->assemblyMatrixAndRHS(tranMatrix, netlist, u, res, absContribs, flags);

   // return the transient's analysis matrix
   return tranMatrix;
}


MYINT AN_TranFunction::minimumNrIteration() const
{
  return 1; // TODO: this could also be configurable to make at least one
            // time step
}
