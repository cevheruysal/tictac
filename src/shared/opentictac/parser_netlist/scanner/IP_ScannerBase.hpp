/*
 * IP_ScannerBase.hpp
 *
 *  Created on: Jan 20, 2015
 *      Author: benk
 */

#ifndef IP_SCANNERBASE_HPP_
#define IP_SCANNERBASE_HPP_

#include <vector>
#include <iostream>
#include <fstream>
#include <string>


#include "IP_Token.hpp"
#include "sim_utils.hpp"

/** simple class to represent the state of the scanner */
class IP_ScannerStatusInfo{
public:

  IP_ScannerStatusInfo() {isError_ = false; errorMessage_= "";}

  bool isError() const { return isError_; }

  std::string& errorMessage() { return errorMessage_; }

public:
  /** if there was an error*/
  bool isError_;
  /** the error message */
  std::string errorMessage_;
};

/** the base class for the scanner.
 * This is the scanner only for the netlist structure, and not for expression
 * The expressions should be read as a string, and later processed as */
class IP_ScannerBase : public UT_SIMVerboseClass
{
public:

  /** Ctor
   * @param scannerName the scanner name such that in a generic way one can see
   * which scanner is now in action */
  IP_ScannerBase(std::string scannerName):UT_SIMVerboseClass(std::string("scanner")), scannerName_(scannerName) {;}

  /** empty Dtor */
  virtual ~IP_ScannerBase() {;}

  /** with this method one starts to read a ntlist file (including runset)*/
  virtual void startReadingFile(std::string fileName) = 0;

  /** this method reads in a new record, a new record is
   * composed of several tokens
   * IMPORTANT: special instructions, which only represent for the scanner
   * are not here !*/
  virtual void getNextRecord(IP_ScannerStatusInfo &status) = 0;

  /** in the actual record, returns a token*/
  virtual IP_Token& getToken(int index) = 0;

  /** @returns the number of tokens in the actual record*/
  virtual int getNrTokensInActualRecord() = 0;

  /** returns the actual file, where the scanner read */
  virtual std::string& getActualFileName() = 0;

  /** if reading is finished then this returns true otherwise false */
  virtual bool finishedReading() = 0;

  /** adds a delimiter character for comments */
  virtual void addDelimiterForComment(char c) { delimiterForComments_.push_back(c); }

  /** removes one delimiter from the comment limiter list */
  virtual void removeDelimiterForComment(char c) {
    for (int i = 0; i < (int)delimiterForComments_.size(); ++i)
      if (delimiterForComments_[i] == c)
        delimiterForComments_.erase(delimiterForComments_.begin()+i);
  }

  /** adds a delimiter character for comments */
  virtual void addDelimiterForSoftComment(char c) { softDelimiterForComments_.push_back(c); }

  /** removes one delimiter from the soft delimiter list */
  virtual void removeDelimiterForSoftComment(char c) {
    for (int i = 0; i < (int)softDelimiterForComments_.size(); ++i)
      if (softDelimiterForComments_[i] == c)
        softDelimiterForComments_.erase(softDelimiterForComments_.begin()+i);
  }

  /** adds a delimiter character for comments */
  virtual void addDelimiterForStartLineComment(char c) { lineStartForComments_.push_back(c); }

  /** removes one delimiter from the soft delimiter list */
  virtual void removeDelimiterForStartLineComment(char c) {
    for (int i = 0; i < (int)lineStartForComments_.size(); ++i)
      if (lineStartForComments_[i] == c)
        lineStartForComments_.erase(lineStartForComments_.begin()+i);
  }

protected:
  /** internal method to report scanner errors */
  virtual void reportError(std::string errMsg) = 0;

public:
  /**
   * match one string to an array of strings (which represent key words, delimiters )
   * IMPORTANT: the delimiters are in lower case (if no case sensitive)!!!!*/
  static bool matchStringToken(std::string &tokenString,
                         std::vector<std::string> &delims,
                         int &ind);

  /**
   * match one string ending to an array of strings (which represent key words, delimiters )
   * IMPORTANT: the delimiters are in lower case (if no case sensitive)!!!!*/
  static bool matchEndStringToken(std::string &tokenString,
                         std::vector<std::string> &delims,
                         int &ind);

  /** match one char to the array of delimiters
   * IMPORTANT: no lower or upper case conversion!!! (should not be necessary)*/
  static bool matchCharToken(char tokenChar,
                         std::vector<char> &delims,
                         int &ind);

  /// method to transform to everything to lower case (if no case sensitive)
  static std::string toLower(const std::string &data);

protected:

  /** contains all the delimiters for expressions START (e.g '(')*/
  std::vector<char> delimiterStartExpressions_;
  /** contains all the delimiters for expressions END (e.g ')')*/
  std::vector<char> delimiterEndExpressions_;

  /** contains all the delimiters for comments (BUT NOT WITHIN EXPRESSIONS)*/
  std::vector<char> delimiterForComments_;
  /** these are interpreted as comments only when a token starts with */
  std::vector<char> softDelimiterForComments_;
  /** these are interpreted as comments only a new line is started */
  std::vector<char> lineStartForComments_;

  /** contains all the delimiters to show next line will continue this record*/
  std::vector<std::string> delimiterForRecordContinueEnd_;
  /** contains all the delimiters to show next line will continue this record*/
  std::vector<char> delimiterForRecordContinueStart_;

  /** characters as '=' should always be processed as separate tokens, except they are
   * within expressions */
  std::vector<char>         alwaysSeparateTokens_;

  /** the according token type to each entry of "alwaysSeparateTokens_"*/
  std::vector<IP_TokenType> tokenType_;

  /** the delmiter for tokens (the tokens, except expressions are not allowed
   * to contain such characters )*/
  std::vector<char> delimiterForTokens_;

  /** the form of the tokens which indicate an included file
   * IMPORTANT: all in lower case (if no case sensitive)! */
  std::vector<std::string> includeTokens_;

  /** scanner name */
  std::string scannerName_;
};



#endif /* IP_SCANNERBASE_HPP_ */
