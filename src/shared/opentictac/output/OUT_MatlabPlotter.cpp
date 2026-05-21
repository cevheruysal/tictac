/*
 * OUT_MatlabPlotter.cpp
 *
 *  Created on: May 13, 2015
 *      Author: benk
 */

#include "OUT_MatlabPlotter.hpp"

OUT_MatlabPlotter::OUT_MatlabPlotter() : OUT_WaveFormPlotter()
{
  //
}


void OUT_MatlabPlotter::startRecording()
{
  // IMPORTANT: call this as first
  startRecordingInit();

  MYINT i;
  std::string fullDataFileName = fileName_ + ".m";
  // we have here only one file
  myfileData_.open (fullDataFileName.c_str(), std::ios::out);
  myfileData_ << "plot_names = { ";
  for (i = 0; i < nrScalarPlots_; ++i)
    myfileData_ << "\'" << scalarPlotNames_[i] << "\' ";
  myfileData_ << " }; \n";
  myfileData_ << "plot_units = { ";
  for (i = 0; i < nrScalarPlots_; ++i)
    myfileData_ << "\'" << scalarPlotUnits_[i] << "\' ";
  myfileData_ << " }; \n";
  // initialize the time vector and the data vector
  myfileData_ << " t=[]; \n";
  myfileData_ << " d=[]; \n";
}


void OUT_MatlabPlotter::flushActualLine()
{
  myfileData_ << "t = [ t " << std::setprecision(11) << std::scientific << actualScalarXArg_ << "]; \n";
  myfileData_ << "d = [ d ; ";
   for (MYINT i = 0; i < nrScalarPlots_; i++)
     myfileData_ << " " << std::setprecision(11) << std::scientific << actualScalarValues_[i];
   myfileData_ << "];" << std::endl;
}

void OUT_MatlabPlotter::endPlotting()
{
  // TODO:
  for (MYINT i = 0; i < nrScalarPlots_; i++)
    {
      myfileData_ << "figure;\n";
      myfileData_ << "plot( t, d(:," << i+1 << "), \'-+\' );\n";
      myfileData_ << "xlabel(\'" << xargName_ << "[" << xargUnits_ << "]\');\n";
      myfileData_ << "ylabel(\'" << scalarPlotNames_[i] << "[" << scalarPlotUnits_[i] << "]\');\n";
    }
  myfileData_.close();
}
