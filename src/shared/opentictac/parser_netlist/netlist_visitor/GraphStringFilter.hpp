//
// Created by kowitz on 8/6/18.
//

#ifndef TICTAC_GRAPHSTRINGFILTER_HPP
#define TICTAC_GRAPHSTRINGFILTER_HPP


#include <IP_ExpansionStack.hpp>
#include <fstream>
#include <string>
#include <stdexcept>


class GraphStringFilterException : public std::runtime_error
{
 public:
  GraphStringFilterException(std::string const &in) : runtime_error(in) {}
};

template <typename ExpansionStackType>
class GraphStringFilter;

namespace GraphStringFilterTool
{
using StringVec = std::vector<std::string>;

StringVec
    findExprInParentheses(std::string const &in);

StringVec
    splitAtOperators(std::vector<std::string> const &in);

std::pair<StringVec, StringVec>
    separateNodesAndPins(StringVec const &in);

std::pair<StringVec, StringVec>
    separateTwoPinDevicesAndNodes(StringVec const &in, StringVec const &twoPinDevices);

StringVec
    createPlotStatementsNodes(StringVec const &in);

StringVec
    createPlotStatementsTwoPinDevices(StringVec const &stringVec);

StringVec
    createPlotStatementsDevicesWithPins(std::vector<std::pair<std::string, std::string>> const &in);

std::vector<std::pair<std::string, std::string>>
    splitDevWithPin(StringVec const &in);

std::vector<std::string>::const_iterator
    isInDeviceVector(std::vector<std::string> const &in, std::string const &device_name);

template <typename ExpansionStackType, typename IP_XCallType = IP_XCall>
std::vector<std::string>
    getPinnames(std::string const &device_in, ExpansionStackType &expansionStack)
{
  std::string combinedName(
      sim_toLowerString(GraphStringFilter<ExpansionStackType>::xCallPrefix_ + device_in));
  IP_XCallType *x_element(expansionStack.getXCall(combinedName));

  StringVec nameBinding;
  if (x_element != nullptr)
    {
      // found an x device of that name...yay
      nameBinding = x_element->getPinNameBinding();
    }

  return nameBinding;
}
}  // namespace GraphStringFilterTool

template <typename ExpansionStackType>
class GraphStringFilter
{
 public:
  using StringVec = std::vector<std::string>;
  using StringMap = std::map<std::string, std::string>;

 private:
  //! contains the origin string given to the class
  std::string original_graph_string_;

  //! contains the original definitions from the netlist
  ExpansionStackType &definitionTreeStuff_;

  StringVec const &                                twoPinDevices_;
  StringVec const &                                threePinDevices_;
  StringVec                                        devs_;
  StringVec                                        nodes_;
  std::vector<std::pair<std::string, std::string>> devWithPinsVec_;
  static std::map<std::string, std::string> const  titanTwoPinDevicePinNames_;
  static std::map<std::string, std::string> const  titanPlotStatementPinNames_;

 public:
  static std::string const xCallPrefix_;

  GraphStringFilter(std::string const &                 original_graph_string,
                    ExpansionStackType &                definitionTreeStuff,
                    GraphStringFilter::StringVec const &threePinDevices_,
                    GraphStringFilter::StringVec const &twoPinDevices_);


  StringVec getNodePlotStatements() const;
  StringVec getTwoPinDevicePlotStatements() const;

  template <typename IP_XCallType = IP_XCall>
  int getPort(std::pair<std::string, std::string> const &i) const;
  template <typename IP_XCallType = IP_XCall>
  StringVec getDevicePinPlotStatements() const;


  template <typename IP_XCallType = IP_XCall>
  std::string getAdaptedGraphString() const
  {
    // lets go over the original string and substitute the pins such that the devices can be read
    std::string graph_string(original_graph_string_);
    auto        writePins = devWithPinsVec_;

    for (auto &&it : writePins)
      {
        auto complete_name = it.first + "#" + it.second;
        bool isValid       = false;

        // substitute simtetrix pin name with pin number
        auto port = getPort<IP_XCallType>(it);
        if (port != -1)
          {
            it.second = sim_integerToString(port);
            isValid   = true;
          }

        // substitute pin numbers to titan pin names
        auto foundInTwoPinDeviceList =
            GraphStringFilterTool::isInDeviceVector(twoPinDevices_, it.first);
        if (foundInTwoPinDeviceList != twoPinDevices_.end())
          {
            try
              {
                it.second = titanTwoPinDevicePinNames_.at(it.second);
                isValid   = true;
              }
            catch (std::out_of_range const &e)
              {
              }
          }

        if (std::all_of(it.second.begin(), it.second.end(), ::isdigit))
          {
            isValid = true;
          }

        if (!isValid)
          {
            throw GraphStringFilterException(
                "There is a pin which is a string which could not recognized!");
          }

        auto npos = graph_string.find(complete_name);
        while (npos != std::string::npos)
          {
            graph_string.replace(npos, complete_name.size(), it.first + "#" + it.second);
            npos = graph_string.find(complete_name, npos + complete_name.size());
          }
      }

    return graph_string;
  }
};

template <typename ExpansionStackType>
GraphStringFilter<ExpansionStackType>::GraphStringFilter(
    std::string const &                 original_graph_string,
    ExpansionStackType &                definitionTreeStuff,
    GraphStringFilter::StringVec const &threePinDevices_,
    GraphStringFilter::StringVec const &twoPinDevices_)
    : original_graph_string_(original_graph_string)
    , definitionTreeStuff_(definitionTreeStuff)
    //    , exprInParentheses_(findExprInParentheses(original_graph_string))
    , twoPinDevices_(twoPinDevices_)
    , threePinDevices_(threePinDevices_)
{
  auto exprInString = GraphStringFilterTool::findExprInParentheses(original_graph_string_);
  auto splitAtOp    = GraphStringFilterTool::splitAtOperators(exprInString);
  auto sepNodesPins = GraphStringFilterTool::separateNodesAndPins(splitAtOp);
  auto sepTwoPins =
      GraphStringFilterTool::separateTwoPinDevicesAndNodes(sepNodesPins.first, twoPinDevices_);

  devs_           = sepTwoPins.first;
  nodes_          = sepTwoPins.second;
  devWithPinsVec_ = GraphStringFilterTool::splitDevWithPin(sepNodesPins.second);
}

template <typename ExpansionStackType>
std::string const GraphStringFilter<ExpansionStackType>::xCallPrefix_{"x$"};

template <typename ExpansionStackType>
std::map<std::string, std::string> const
    GraphStringFilter<ExpansionStackType>::titanTwoPinDevicePinNames_{{"1", "p"},
                                                                      {"2", "n"},
                                                                      {"p", "p"},
                                                                      {"n", "n"},
                                                                      {"P", "p"},
                                                                      {"N", "n"}};

template <typename ExpansionStackType>
std::map<std::string, std::string> const
    GraphStringFilter<ExpansionStackType>::titanPlotStatementPinNames_{{"p", "1"},
                                                                       {"n", "2"},
                                                                       {"P", "1"},
                                                                       {"N", "2"}};

template <typename ExpansionStackType>
std::vector<std::string>
    GraphStringFilter<ExpansionStackType>::getNodePlotStatements() const
{
  return GraphStringFilterTool::createPlotStatementsNodes(nodes_);
}

template <typename ExpansionStackType>
std::vector<std::string>
    GraphStringFilter<ExpansionStackType>::getTwoPinDevicePlotStatements() const
{
  return GraphStringFilterTool::createPlotStatementsTwoPinDevices(devs_);
}

template <typename ExpansionStackType>
template <typename IP_XCallType>
std::vector<std::string>
    GraphStringFilter<ExpansionStackType>::getDevicePinPlotStatements() const
{
  StringVec plot_statements;
  for (auto &&i : devWithPinsVec_)
    {
      int port = getPort<IP_XCallType>(i);
      if (port != -1)
        {
          // even the device name changes since there could be a prefix
          plot_statements.emplace_back("I(" + xCallPrefix_ + i.first + ", "
                                       + sim_integerToString(port) + ")");
        }
      else
        {

          auto found = GraphStringFilterTool::isInDeviceVector(twoPinDevices_, i.first);
          if (found != twoPinDevices_.end())
            {
              // its a two pin device, lets transform the input
              try
                {
                  plot_statements.emplace_back("I(" + *found + ", "
                                               + titanPlotStatementPinNames_.at(i.second) + ")");
                }
              catch (std::out_of_range const &e)
                {
                  // in case there is no substitution, don't print anything....
                }
            }
        }
    }
  return plot_statements;
}

template <typename ExpansionStackType>
template <typename IP_XCallType>
int
    GraphStringFilter<ExpansionStackType>::getPort(
        std::pair<std::string, std::string> const &i) const
{
  auto pins = GraphStringFilterTool::getPinnames<ExpansionStackType, IP_XCallType>(
      i.first, definitionTreeStuff_);
  auto ind  = find(pins.begin(), pins.end(), i.second);
  int  port = -1;
  if (ind != pins.end())
    {
      port = distance(pins.begin(), ind) + 1;
    }

  return port;
}

#endif  // TICTAC_GRAPHSTRINGFILTER_HPP
