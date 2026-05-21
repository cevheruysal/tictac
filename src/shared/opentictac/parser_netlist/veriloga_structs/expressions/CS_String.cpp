/*
 * CS_String.cpp
 *
 *  Created on: Jan 2, 2013
 *      Author: benk
 */

#include "CS_String.hpp"

std::vector<char> CS_String::specialChars_;
std::vector<std::string> CS_String::specialCharsString_;

CS_String::CS_String(std::string &value,
    const CS_ModuleDeclaration* module, int startLine, int startColumn) :CS_ExpressionBasis(module, startLine, startColumn), value_(value)
{
  setupSpecialChars();

  // at all places where we find "\n" we should add "\\n" and so on
  //std::cout << "&SDTOR1&" << value_ << ",\n";
  int i,j;
  for (i = 0; i < (int)value_.size(); i++)
    {
      for (j = 0; j < (int)specialChars_.size(); j++)
      if (value_[i] == specialChars_[j])
        {
          for (decltype(specialCharsString_[j].size()) k = 0; k < specialCharsString_[j].size()-1; k++)
            value_ = value_.insert(i," ");
          // insert twice the character
          value_.replace(i, specialCharsString_[j].size(), specialCharsString_[j] );
          i = i + specialCharsString_[j].size();
          //std::cout << "&" << value_ << "&\n";
        }
    }
  //std::cout << "&SDTOR2&" << value_ << ",\n";
}

void CS_String::setupSpecialChars()
 {
  // do it only once
   if (specialChars_.size() > 0)
     return;

   // add the special characters
   specialChars_.resize(0);
   specialCharsString_.resize(0);
   specialChars_.push_back('\n'); specialCharsString_.push_back("\\n");
   specialChars_.push_back('\t'); specialCharsString_.push_back("\\t");
   specialChars_.push_back('"');  specialCharsString_.push_back("\\\"");
   specialChars_.push_back('\\');  specialCharsString_.push_back("\\\\");


   // add special characters
   int i = 0, itmp, d1 = 0, d2 = 0, d3 = 0;
   std::string ret = "";
   for (i = 0; i < 256; i++) {

       // TODO: EXTEND here the list of special characters
       if (i == 8+5  // carrige return
           // (i == 8*4)  // space
          //(!isalpha(char(i)))
           )
         {
           specialChars_.push_back((char)(i));
           itmp = i;
           d1 = itmp / 64;
           itmp = itmp % 64;
           d2 = itmp / 8;
           d3 = itmp % 8;

           ret = "\\" + verilog_integerToString(d1)
                      + verilog_integerToString(d2)
                      + verilog_integerToString(d3);
           specialCharsString_.push_back(ret);
         }
   }

 }

