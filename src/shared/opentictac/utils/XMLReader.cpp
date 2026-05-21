/*
 * FitobXMLReader.cpp
 *
 *  Created on: Feb 26, 2010
 *      Author: benk
 */

#include "XMLReader.hpp"

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <boost/lexical_cast.hpp>

using namespace std;

XMLReader::XMLReader(const string& configFileName) {
  // Load the XML file into the property tree. If reading fails
  // (cannot open file, parse error), an exception is thrown.
  read_xml(configFileName, pt_ , 0x2 ); // means no comment

}

// return the number of children
int XMLReader::nrXMLNodes(const string& pathInTree) const{
  int counter = 0;
  // there could be even non nodes
  boost::optional< const ptree& > child = pt_.get_child_optional( pathInTree );
  if( !child ) {
    return 0; // this node is non existent
  }

  BOOST_FOREACH([[maybe_unused]] const ptree::value_type &v , pt_.get_child(pathInTree)) {
    counter++;
  }
  return counter;
}

// return the number of children
int XMLReader::nrXMLNodes(ptree &pt, const string& pathInTree)
{
  int counter = 0;
  // there could be even non nodes
  boost::optional< ptree& > child = pt.get_child_optional( pathInTree );
  if( !child ) {
    return 0; // this node is non existent
  }

  BOOST_FOREACH([[maybe_unused]] const ptree::value_type &v , pt.get_child(pathInTree)) {
    counter++;
  }
  return counter;
}


int XMLReader::getIntConfiguration(
		                const string& pathInTree_first,
		                const int indexChild ,
		                const string& pathInTree_second) const
{
  int counter = 0;
  BOOST_FOREACH(const ptree::value_type &v , pt_.get_child(pathInTree_first))
  {
    if (indexChild <= counter)
      {
	// take the tree , and ask for the second path inside the tree
	const ptree p = v.second;
	return p.get(pathInTree_second, -1);
	break;
      }
    else
      {
	counter++;
      }
  }
  return -1; //Wall
}

int XMLReader::getIntConfiguration(
    ptree &pt,
    const string& pathInTree_first,
    const int indexChild ,
    const string& pathInTree_second)
{
  int counter = 0;
  BOOST_FOREACH(const ptree::value_type &v , pt.get_child(pathInTree_first))
  {
    if (indexChild <= counter)
      {
	// take the tree , and ask for the second path inside the tree
	const ptree p = v.second;
	return p.get(pathInTree_second, -1);
	break;
      }
    else
      {
	counter++;
      }
  }
  return -1; //Wall
}

double XMLReader::getDoubleConfiguration(
		                       const string& pathInTree_first,
		                       const int indexChild ,
		                       const string& pathInTree_second) const
{
  int counter = 0;
  BOOST_FOREACH(const ptree::value_type &v , pt_.get_child(pathInTree_first))
  {
    if (indexChild <= counter)
      {
	// take the tree , and ask for the second path inside the tree
	const ptree p = v.second;
	return p.get(pathInTree_second, -1.0);
	break;
      }
    else
      {
	counter++;
      }
  }
  return -1.0; //Wall
}

double XMLReader::getDoubleConfiguration(ptree &pt,
				       const string& pathInTree_first,
		                       const int indexChild ,
		                       const string& pathInTree_second)
{
  int counter = 0;
  BOOST_FOREACH(const ptree::value_type &v , pt.get_child(pathInTree_first))
  {
    if (indexChild <= counter)
      {
	// take the tree , and ask for the second path inside the tree
	const ptree p = v.second;
	return p.get(pathInTree_second, -1.0);
	break;
      }
    else
      {
	counter++;
      }
  }
  return -1.0; //Wall
}

const string XMLReader::getStringConfiguration(
		                            const string& pathInTree_first,
		                            const int indexChild ,
		                            const string& pathInTree_second) const
{
  int counter = 0;
  BOOST_FOREACH(const ptree::value_type &v , pt_.get_child(pathInTree_first))
  {
    if (indexChild <= counter)
      {
	// take the tree , and ask for the second path inside the tree
	const ptree p = v.second;
	return p.get(pathInTree_second, "");
	break;
      }
    else
      {
	counter++;
      }
  }
  return ""; //Wall
}

const string XMLReader::getStringConfiguration(ptree &pt,
					const string& pathInTree_first,
					const int indexChild ,
					const string& pathInTree_second)
{
  int counter = 0;
  BOOST_FOREACH(const ptree::value_type &v , pt.get_child(pathInTree_first))
  {
    if (indexChild <= counter)
      {
	// take the tree , and ask for the second path inside the tree
	const ptree p = v.second;
	return p.get(pathInTree_second, "");
	break;
      }
    else
      {
	counter++;
      }
  }
  return ""; //Wall
}


/** splits one string in different substrings using the delimiter char "delim" */
void XMLReader::splitString(
		const std::string &s, char delim, std::vector<std::string> &elems) const {
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

/** converts one array of strings into numbers (Integers)*/
void XMLReader::vectStringToIArray(
	   const std::vector<std::string> &elems , IVector& intArr) const {
	   using boost::lexical_cast;
	   using boost::bad_lexical_cast;
       for (unsigned int i = 0 ; i < elems.size() ; i++ ){
    	   intArr.push_back(lexical_cast<int>(elems[i]));
       }
}

/** converts one array of strings into numbers (Integers)*/
void XMLReader::vectStringToDArray(
		const std::vector<std::string> &elems , DVector& doubleArr) const {
	 using boost::lexical_cast;
	 using boost::bad_lexical_cast;
	 for (unsigned int i = 0 ; i < elems.size() ; i++ ){
		 doubleArr.push_back(lexical_cast<double>(elems[i]));
	 }
}

void XMLReader::getIntVectorConfiguration(
		                       const string& pathInTree,
		                       char delim,
		                       IVector& outVect) const {

	string tmp = getStringConfiguration(pathInTree);
	std::vector<std::string> tmp_strs;
	splitString(tmp, delim, tmp_strs);
	vectStringToIArray( tmp_strs , outVect );
}

void XMLReader::getDoubleVectorConfiguration(
		                          const string& pathInTree,
		                          char delim,
		                          DVector& outVect) const {

	string tmp = getStringConfiguration(pathInTree);
	std::vector<std::string> tmp_strs;
	splitString(tmp, delim, tmp_strs);
	vectStringToDArray( tmp_strs , outVect );

}

void  XMLReader::getIntVectorConfiguration(
		                       const string& pathInTree_first,
                               const int indexChild ,
                               const string& pathInTree_second,
		                       char delim,
		                       IVector& outVect) const {

	string tmp = getStringConfiguration(pathInTree_first,indexChild,pathInTree_second);
	std::vector<std::string> tmp_strs;
	splitString(tmp, delim, tmp_strs);
	vectStringToIArray( tmp_strs , outVect );

}

void  XMLReader::getDoubleVectorConfiguration(
		                          const string& pathInTree_first,
                                  const int indexChild ,
                                  const string& pathInTree_second,
		                          char delim,
		                          DVector& outVect) const {

	string tmp = getStringConfiguration(pathInTree_first,indexChild,pathInTree_second);
	std::vector<std::string> tmp_strs;
	splitString(tmp, delim, tmp_strs);
	vectStringToDArray( tmp_strs , outVect );

}
