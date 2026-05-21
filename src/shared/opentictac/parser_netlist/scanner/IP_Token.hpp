/*
 * IP_Token.hpp
 *
 *  Created on: Jan 20, 2015
 *      Author: benk
 */

#ifndef IP_TOKEN_HPP_
#define IP_TOKEN_HPP_

#include <string>

/// forward declaration of
class IP_DummyModule;

/** the types of the tokens */
typedef enum {
    TK_NOTYPE = 0, /// general type
    TK_EQUAL = 1,  /// "=" since this might have special role
    TK_CONTROLL = 2, ///< if this represent a special control word, such as "."
    TK_EXPRESSION = 3, /// < this contains an expression as a string (without ' trailing)
    TK_CONTROL = 4, ///< e.g. when something starts with "."
    TK_FILENAME = 5, ///< when this token represents a filename (
                    //  important for no lower case (if no case sensitive) transformation)
    TK_OPEN_BR = 6,   /// "(" use as separate token
    TK_CLOSE_BR = 7,  /// ")" use as separate token
    TK_SEMICOLON = 8,  /// ";" use also to separate tokens (has neutral effect) */
    TK_PSPICE_EXPRESSION = 9 /// instead of ' ' we use {}. For some statements this is crucial
} IP_TokenType;


/** class which represent the tokens, which are aoutput of the scanner,
 * Tokens are the first stage in the parsing process, and they usually represent
 * a string, which later is resolved,
 * IMPORTANT: we have different layers for the netlist structure and expression
 * parsing.
 * In the first stage the expressions are read-in as pure string, and we only need
 * to know the delimiter for the strings */
class IP_Token
{
public:

  /** Dtor with 2 values*/
  IP_Token(const std::string &value, IP_TokenType type=TK_NOTYPE,
      std::string *filename=NULL, IP_DummyModule *dummyFileMode=NULL,
      int colInd = -1, int rowInd = -1, int includeLevel = -1);

  /** Ctor with rvalue string*/
  IP_Token(std::string &&value, IP_TokenType type=TK_NOTYPE,
      std::string *filename=NULL, IP_DummyModule *dummyFileMode=NULL,
      int colInd = -1, int rowInd = -1, int includeLevel = -1);

  /** empty Ctor, initializing fields with dummy values */
  IP_Token();

  /// copy Ctor
  IP_Token(const IP_Token &copy);

  /** Assignment Ctor */
  IP_Token& operator=(const IP_Token& from);

  /** empty Dtor*/
  virtual  ~IP_Token() {;}

  /** get the value */
  const std::string& getValue() const { return value_; }
  std::string& getValue()             { return value_; }

  /** returns true if the string match the token value */
  bool isIqual(const char* str) const { return (value_ == std::string(str)); }
  bool isIqual(const std::string &str) const { return (value_ == str); }
  //bool isIqualNoCase(const std::string &str) const { return (value_ == str); }

  /** returns true if this tokan is an empty one */
  bool hasEmptyValue() const { return (value_.size() > 1); }

  /** set value */
  //void setValue(std::string& value)  { value_ = value; }
  void setValue(std::string value)   { value_ = value; }

  /** get type of the token */
  IP_TokenType getTokenType() const { return type_; }

  /** set type of the token */
  void setTokenType(IP_TokenType t) { type_ = t; }

  /** NULL if the file name was not given */
  const std::string* getFileName() const { return filename_; }
  /** NULL if the file name was not given */
  const IP_DummyModule* getFileNameDummyMod() const { return dummyFileMode_; }

  int getColumnIndex() const { return colInd_; }
  int getRowIndex() const { return rowInd_; }

  void setColumnIndex(int c) { colInd_ = c; }
  void setRowIndex(int r) { rowInd_ = r; }

  /// retutrns the level of include
  int getIncludeLevel() const { return includeLevel_; }
  /// sets the level of includes
  void setIncludeLevel(int i) { includeLevel_ = i; }

private:
  /** type of the token */
  IP_TokenType type_;
  /** value of the token*/
  std::string value_;

  /** pointer to the filename (in order to save space)*/
  std::string *filename_;
  /** this information is needed such that expressions (from Verilog-A)
   * will also get the information about filename line col ...  */
  IP_DummyModule *dummyFileMode_;

  /** if the filename given than the column index */
  int colInd_;
  /** if the filename given than the row index */
  int rowInd_;
  /** this number tells the level of nestednes, toplevel is 0 */
  int includeLevel_;
};

#endif /* IP_TOKEN_HPP_ */
