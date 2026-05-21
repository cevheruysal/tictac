/*
 * IP_Token.cpp
 *
 *  Created on: Jan 20, 2015
 *      Author: benk
 */

#include "IP_Token.hpp"
#include "IP_DummyModule.hpp"

IP_Token::IP_Token(const std::string &value, IP_TokenType type,
                   std::string *filename, IP_DummyModule *dummyFileMode, int colInd,
                   int rowInd, int includeLevel): type_(type)
    , value_(value)
    , filename_(filename)
    , dummyFileMode_(dummyFileMode)
    , colInd_(colInd)
    , rowInd_(rowInd)
    , includeLevel_(includeLevel)
{
  // all fields are set
}

IP_Token::IP_Token(std::string &&value, IP_TokenType type,
                   std::string *filename, IP_DummyModule *dummyFileMode, int colInd,
                   int rowInd, int includeLevel): type_(type)
    , value_(value)
    , filename_(filename)
    , dummyFileMode_(dummyFileMode)
    , colInd_(colInd)
    , rowInd_(rowInd)
    , includeLevel_(includeLevel)
{
  // all fields are set
}

IP_Token::IP_Token(): type_(TK_NOTYPE)
    , value_(std::string("DUMMY"))
    , filename_(NULL)
    , dummyFileMode_(NULL)
    , colInd_(-1)
    , rowInd_(-1)
    , includeLevel_(-1)
{

}


IP_Token::IP_Token(const IP_Token &copy)
{
  // copy all fields
  value_ = copy.value_;
  type_ = copy.type_;
  filename_ = copy.filename_;
  dummyFileMode_ = copy.dummyFileMode_;
  colInd_ = copy.colInd_;
  rowInd_ = copy.rowInd_;
  includeLevel_ = copy.includeLevel_;
}

IP_Token& IP_Token::operator=(const IP_Token& from)
{
  if(&from == this)
    return *this;

  // copy all fields
  value_ = from.value_;
  type_ = from.type_;
  filename_ = from.filename_;
  dummyFileMode_ = from.dummyFileMode_;
  colInd_ = from.colInd_;
  rowInd_ = from.rowInd_;
  includeLevel_ = from.includeLevel_;
  return *this;
}