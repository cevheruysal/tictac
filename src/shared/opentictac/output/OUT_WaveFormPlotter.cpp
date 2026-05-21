/*
 * OUT_WaveFormPlotter.cpp
 *
 *  Created on: Nov 8, 2013
 *      Author: benk
 */

#include "OUT_WaveFormPlotter.hpp"
#include "Netlist.hpp"
#include "LA_MatrixNetlistMap.hpp"
#include "AN_SimulationControllers.hpp"

OUT_WaveFormPlotter::OUT_WaveFormPlotter()
{
  fileName_ = "save";
  nrScalarPlots_ = 0;

  scalarPlotNames_.resize(0);
  scalarPlotUnits_.resize(0);
  scalarPlotTypes_.resize(0);
  scalarPlotIndex_.resize(0);
  scalarPlotInternalIndex_.resize(0);
  actualScalarValues_.resize(0);
  actualScalarXArg_ = 0.0;
  nrRecordedPoints_ = 0;

  // TODO: this might be set differently
  xargName_  = "TIME";
  xargUnits_ = "s";
}

OUT_WaveFormPlotter::~OUT_WaveFormPlotter()
{
  // nothing to do here, no delete needed
  // just close the file if it is not closed yet
  if (myfileData_.is_open())
    myfileData_.close();
  if (myfileHeader_.is_open())
    myfileHeader_.close();
}

/// important method for all wavefiles before start recording
void OUT_WaveFormPlotter::startRecordingInit()
{
  // IMPORTANT: resize the state vector
  actualScalarValues_.resize( nrScalarPlots_, 0.0);
}

void OUT_WaveFormPlotter::startRecording()
{
  // IMPORTANT: call this as first
  startRecordingInit();

  // write first the header
  MYINT i;
  std::string fullHeaderFileName = fileName_ + ".tr." + "ai_pl";
  std::string fullDataFileName = fileName_ + ".p2" + ".tr";

  myfileHeader_.open (fullHeaderFileName.c_str(), std::ios::out);
  myfileHeader_ << "# Scope control file for output file: " << fileName_ << "\n";
  myfileHeader_ << "Program Saber\n";
  myfileHeader_ << "# Important note: The generating program was TITAN, not Saber\n";
  myfileHeader_ << "# The clause \"Program Saber\" is only needed to support hierarchy features\n";
  myfileHeader_ << "Release TICTAC\n";
  myfileHeader_ << "data_file " << fullDataFileName << "\n";
  myfileHeader_ << "Created " << sim_currentDateTime() << "\n";
  myfileHeader_ << "Title " << fileName_ << "\n";
  myfileHeader_ << "Tag.lin_x\n";
  myfileHeader_ << "Xname  " << xargName_ << "\n";
  myfileHeader_ << "Xunits " << xargUnits_ << "\n";
  myfileHeader_ << "Ytype real\n";
  myfileHeader_ << "# Ynames_style SCOPE\n";
  myfileHeader_ << "Ynames\n";
  for (i = 0; i < nrScalarPlots_; i++) {
      myfileHeader_ << " " << scalarPlotNames_[i] << "\n";
  }
  myfileHeader_ << "Yunits\n";
  for (i = 0; i < nrScalarPlots_; i++) {
      myfileHeader_ << " " << scalarPlotUnits_[i] << "\n";
  }
  myfileHeader_ << "Yproperty.type\n";
  for (i = 0; i < nrScalarPlots_; i++) {
      myfileHeader_ << " a\n";
  }

  // then recreate the other file
 myfileData_.open (fullDataFileName.c_str(), std::ios::out);
 myfileData_ << "# " << fullHeaderFileName << "\n";
}


void OUT_WaveFormPlotter::actualizeValues(
    const Netlist* netlist,
    const std::vector<MYREAL>& actState,
    const LA_MatrixNetlistMap* map,
    [[maybe_unused]] const AN_SimulationControllers* simCtr,
    MYREAL actualScalarXArg)
{
  // update the "actualScalarValues_" vector
  MYINT i, tmpID;
  this->actualScalarXArg_ = actualScalarXArg;
  for (i = 0; i < nrScalarPlots_; i++)
    {
      switch (scalarPlotTypes_[i]) {
      case NODE_VOLTAGE_PLOT : {
        tmpID = scalarPlotIndex_[i]; // this is the netlit's node number
        break; }
      case INTERNAL_VOLTAGE_PLOT : {
        tmpID = netlist->getDevice(scalarPlotIndex_[i])->getPin(scalarPlotInternalIndex_[i],netlist->getPariID());
        break; }
      case CURRENT_PLOT : {
        tmpID = netlist->getDevice(scalarPlotIndex_[i])->getCurrentID(scalarPlotInternalIndex_[i],netlist->getPariID());
        break; }
      // default is an internal error
      default : { SIM_ERROR_STOP("Internal OUT_WaveFormPlotter"); break; }
      }
      // negative index means GROUND or zero
      if (tmpID >= 0)
        {
          tmpID = map->getColIndexForUnkID(tmpID);
          // std::cout << "SIZE: " << actState.size() << " SIZE1:" << actualScalarValues_.size() << "\n";
          actualScalarValues_[i] = actState[tmpID];
        }
      else
        actualScalarValues_[i] = 0.0;
    }
  nrRecordedPoints_++;
}


void OUT_WaveFormPlotter::flushActualLine()
{
  // just write out one line and make sure that it is written into a file
  myfileData_ << std::fixed;
  myfileData_ << std::setprecision(11) << std::scientific << actualScalarXArg_;
  for (MYINT i = 0; i < nrScalarPlots_; i++)
    myfileData_ << " " << std::setprecision(11) << std::scientific << actualScalarValues_[i];
  myfileData_ << std::endl;
  // myfileData_.flush(); //TODO: flush might be too slow
  // TODO: maybe we could always update the header file as well so that
  // during simulation results could be visualized
}


void OUT_WaveFormPlotter::endPlotting()
{
  myfileData_.close();
  // finish the header
  myfileHeader_ << "Npoints                     " << nrRecordedPoints_ << "\n";
  myfileHeader_.close();
}
