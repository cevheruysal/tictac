/*
 * TicTacComputer.cpp
 *
 *  Created on: Sep 30, 2013
 *      Author: benk
 */

#include "TicTacComputer.hpp"
#include "AN_DCAnalysis.hpp"
#include "AN_TranAnalysis.hpp"

TicTacComputer::TicTacComputer(
    TICTAC_SPTR<Netlist> IPNetlist,
    TICTAC_SPTR<IN_SimOptions> globalOptions):
UT_SIMVerboseClass("TicTacComputer"),
IPNetlist_(IPNetlist), globalOptions_(globalOptions)
{
  // TODO Auto-generated constructor stub
  nr_analysis_ = 0;
  analysis_.resize(0);
}

TicTacComputer::~TicTacComputer()
{

}

void TicTacComputer::addAnlysis(TICTAC_SPTR<AN_Analysis> newAnalysis)
{
  if ((nr_analysis_ == 0) && (newAnalysis->getAnalysisType() == TRAN_TYPE))
    { // in case the first analysis is a transient one we should add one
      // DC analysis in front
      TICTAC_SPTR<AN_Analysis> dcanaly =
          TICTAC_SPTR<AN_Analysis> (new AN_DCAnalysis(newAnalysis->getGlobalNetlist(),
              globalOptions_.get(), newAnalysis->getAnalysisPlotterPtr()));
      analysis_.push_back(dcanaly);
      nr_analysis_++;
      newAnalysis->setPreviousAnalysis(dcanaly.get());
      analysis_.push_back(newAnalysis);
      nr_analysis_++;
    }
  else
    {
      // just add directly the analysis
      if (nr_analysis_ > 0) newAnalysis->setPreviousAnalysis(analysis_[nr_analysis_-1].get());
      analysis_.push_back(newAnalysis);
      nr_analysis_++;
    }
}

void TicTacComputer::executeRequiredAnalyses()
{
  // test if we have at least one analysis
  SIM_ERROR_COND_STOP(nr_analysis_ <= 0, "No analysis specified ");
  // inout for all analysis, which later one might reuse for the next analysis
  std::vector<MYREAL> stateVector;
  // this a loop over the sequence of analysis
  for (MYINT i = 0; i < nr_analysis_; i++)
    {
      // just execute analysis (states will be copied inside the method)
      analysis_[i]->executeAnalysis(stateVector);
    }
}
