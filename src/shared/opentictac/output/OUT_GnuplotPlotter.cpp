#include "OUT_GnuplotPlotter.hpp"


OUT_GnuplotPlotter::OUT_GnuplotPlotter():OUT_WaveFormPlotter() {}

void
    OUT_GnuplotPlotter::startRecording()
{
  startRecordingInit();

  std::string fullDataFileName = fileName_ + ".gp";
  myfileData_.open(fullDataFileName.c_str(), std::ios::out);
  myfileData_ << "# Gnuplot output file" << std::endl;
  myfileData_ << "# Columns" << std::endl << "# ";
  for (decltype(nrScalarPlots_) i = 0; i < nrScalarPlots_; ++i) myfileData_ << scalarPlotNames_[i] <<" ";
  myfileData_ << std::endl;
}

void
    OUT_GnuplotPlotter::flushActualLine()
{
  myfileData_ << std::setprecision(11) << std::scientific << actualScalarXArg_<<" ";
  for (decltype(nrScalarPlots_) i = 0; i < nrScalarPlots_; ++i)
    myfileData_ << std::setprecision(11) << std::scientific << actualScalarValues_[i]<<" ";
  myfileData_ << std::endl;
}

void
    OUT_GnuplotPlotter::endPlotting()
{
  myfileData_.close();
}

