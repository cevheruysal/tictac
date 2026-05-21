/*
 * IP_SimpleScanner.hpp
 *
 *  Created on: Jan 20, 2015
 *      Author: benk
 */

#ifndef IP_SIMPLESCANNER_HPP_
#define IP_SIMPLESCANNER_HPP_

#include "IP_ScannerBase.hpp"
#include "IP_DummyModule.hpp"

/** this is the "simple" scanner for the netlist structure */
class IP_SimpleScanner : public IP_ScannerBase
{
public:

  /** empty Ctor */
  IP_SimpleScanner();

  /** empty Dtor */
  virtual ~IP_SimpleScanner();

// ========== methods for the scanner interface ======
  /** with this method one starts to read a ntlist file (including runset)*/
  virtual void startReadingFile(std::string fileName);

  /** this method reads in a new record, a new record is
   * composed of several tokens
   * IMPORTANT: special instructions, which only represent for the scanner
   * are not here ! They are handled within */
  virtual void getNextRecord(IP_ScannerStatusInfo &status);

  /** in the actual record, returns a token*/
  virtual IP_Token& getToken(int index);

  /** @returns the number of tokens in the actual record*/
  virtual int getNrTokensInActualRecord();

  /** returns the actual file, where the scanner reads */
  virtual std::string& getActualFileName();

  /** if reading is finished then this returns true otherwise false */
  virtual bool finishedReading();
// ========== END interface methods ======

protected:

  /** internal method to report scanner errors */
  virtual void reportError(std::string errMsg);

private:

  /** internal method to process the actual line and also tell if next line should be read in
   * @returns true if next line should be read in to complete the record (e.g. // at the end of line )
   * false otherwise  */
  bool processActualLine();

  /** this is the central place to read in next line*/
  void getNextLineFromActualFile();

  /** auxiliary function to push the actual into the list of tokens.
   * This function is read during the line processing
   * after call value is set to ""!
   * @param actToken,   the actual value of the token
   * @param value       the value of this new "pushed" token
   * @param column */
  void pushToken(IP_Token &actToken, std::string &value, int column);

  /** pops one include file from the stack
   * @returns true if there was a stack entry, false if the stack was empty */
  bool popInclude();

  /** push one include file to the stack
   * @param filename the current file name
   * @returns true if we found and could open the file, false otherwise */
  bool pushInclude(std::string &filename);

  /** based on the actual file name we extract the actual directory */
  void extractActualDir();

  /// true if this file is given by absolute path
  bool isFileWithAbsPath(std::string file);

  /** the file where the parsing started */
  std::string startFile_;
  /** the filename which is actual parsed */
  std::string actualFileParsing_;
  /** the directory of the actual file, since for the next level include
   * we also have to look in the actual directory */
  std::string actualDirectory_;

  /** the actual include file stack */
  std::vector<std::string>    includeFileStack_;
  std::vector< TICTAC_SPTR<std::ifstream> >  fileHandleStack;
  std::vector<int>            columnIndStack;
  std::vector<int>            rowIndStack;

  /** we NEED to store each file which we read (this might get large, we might need to optimize it) */
  std::vector<std::string> allFiles_;
  /** for each file we generate a dummy module which can provide the file info
   * also for the Verilog-A structures */
  std::vector<IP_DummyModule> allFileDummyModules_;

  /** actual row number */
  int cRowInd_;
  /** actual column number */
  int cColInd_;

  /** the parsed tokens in the current record */
  std::vector<IP_Token> tokens_;

  /** actual file */
  TICTAC_SPTR<std::ifstream> actFileHandle_;

  /** actual line*/
  std::string actLine_;

  /// when reading is done (including includes) this is true, otherwise is false
  bool isFinished_;
  /// if there was a scanner error
  bool hadError_;
  /// the string containing the error message
  std::string actError_;

  /// actual processing token
  IP_Token actToken_;
};

#endif /* IP_SIMPLESCANNER_HPP_ */
