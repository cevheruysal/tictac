/* @HEADER@ */
// ************************************************************************
//
//                              Fitob
//            Copyright (2012) Janos Benk (benkjanos@gmail.com)
//
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1 of the
// License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
// Questions? Contact Janos Benk (benkjanos@gmail.com),
//
// ************************************************************************
/* @HEADER@ */

/*
 * XMLReader.hpp
 *
 *  Created on: Feb 26, 2010
 *      Author: benk
 */

#ifndef XMLREADER_HPP_
#define XMLREADER_HPP_

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#define IVector std::vector<int>
#define DVector std::vector<double>


using boost::property_tree::ptree;
using namespace std;

 /** This class should contain all the XML configuration
  *  xml configuration stores all the configuration information <br>
  *  The information from this class shold be used only in the setup face,
  *  since <br>
  *  XML attributes are placed under keys named <xmlattr>.  */
 class XMLReader {
 public:

    XMLReader(const string& configFileName);

    virtual ~XMLReader() {;}

	/** get one integer configuration in the configuration tree */
    int getIntConfiguration(const string& pathInTree) const { return pt_.get(pathInTree, -1); }
    static int getIntConfiguration(ptree &pt, const string& pathInTree) { return pt.get(pathInTree, -1); }

    /** get one double configuration in the configuration tree  */
    double getDoubleConfiguration(const string& pathInTree) const { return pt_.get(pathInTree, -1.0); }
    static double getDoubleConfiguration(ptree &pt, const string& pathInTree) { return pt.get(pathInTree, -1.0); }

	/** get one string configuration in the configuration tree  */
    const string getStringConfiguration(const string& pathInTree) const { return pt_.get(pathInTree, ""); }
    static string getStringConfiguration(ptree &pt,const string& pathInTree) { return pt.get(pathInTree, ""); }

	/** get one integer vector in the configuration tree  */
    void getIntVectorConfiguration(const string& pathInTree,
    		                       char delim,
    		                       IVector& outVect) const;

	/** get one double vector in the configuration tree  */
    void getDoubleVectorConfiguration(const string& pathInTree,
    		                          char delim,
    		                          DVector& outVect) const;

    /** number of children at the first level*/
    int nrXMLNodes(const string& pathInTree ) const;
    /// returns the number of nodes in one node
    static int nrXMLNodes(ptree &pt, const string& pathInTree);

    /** */
    int getIntConfiguration(const string& pathInTree_first,
    		                const int indexChild ,
    		                const string& pathInTree_second) const;

    static int getIntConfiguration(
	    ptree &pt,
	    const string& pathInTree_first,
	    const int indexChild ,
	    const string& pathInTree_second) ;

	/** */
    double getDoubleConfiguration(const string& pathInTree_first,
    		                       const int indexChild ,
    		                       const string& pathInTree_second) const;
    static double getDoubleConfiguration(ptree &pt,
				       const string& pathInTree_first,
    		                       const int indexChild ,
    		                       const string& pathInTree_second);

    /// returns a string configarion
    const string getStringConfiguration(const string& pathInTree_first,
					const int indexChild ,
					const string& pathInTree_second) const;
    static const string getStringConfiguration(ptree &pt,
					const string& pathInTree_first,
					const int indexChild ,
					const string& pathInTree_second);

	/** */
    void getIntVectorConfiguration(const string& pathInTree_first,
                                   const int indexChild ,
                                   const string& pathInTree_second,
    		                       char delim,
    		                       IVector& outVect) const;

	/**  */
    void getDoubleVectorConfiguration(const string& pathInTree_first,
                                      const int indexChild ,
                                      const string& pathInTree_second,
    		                          char delim,
    		                          DVector& outVect) const;

 private:

    /** splits one string in different substrings using the delimiter char "delim" */
    void splitString(const std::string &s, char delim, std::vector<std::string> &elems) const;

    /** converts one array of strings into numbers (Integers)*/
    void vectStringToIArray(const std::vector<std::string> &elems , IVector& intArr) const;

    /** converts one array of strings into numbers (Integers)*/
    void vectStringToDArray(const std::vector<std::string> &elems , DVector& doubleArr) const;

	/** The information tree which stores all the configuration info*/
    ptree pt_;

 };

#endif /* XMLREADER_HPP_ */
