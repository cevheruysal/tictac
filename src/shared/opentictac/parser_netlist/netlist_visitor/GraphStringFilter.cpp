//
// Created by kowitz on 8/6/18.
//

#include "GraphStringFilter.hpp"
#include <fstream>

using Gsv = std::vector<std::string>;

Gsv
    GraphStringFilterTool::findExprInParentheses(std::string const &in)
{
  std::string currentColorString;
  Gsv         coloredStrings;
  bool        is_leaf = true;
  for (auto &&letter : in)
    {
      if (letter == '(')
        {
          currentColorString.erase();
          is_leaf = true;
        }
      else
        {
          if (letter == ')')
            {
              if (is_leaf) coloredStrings.push_back(currentColorString);
              currentColorString.erase();
              is_leaf = false;
            }
          else
            {
              currentColorString.append(1, letter);
            }
        }
    }
  if (is_leaf)
    {
      coloredStrings.push_back(currentColorString);
    }

  return coloredStrings;
}

Gsv
    GraphStringFilterTool::splitAtOperators(Gsv const &in)
{
  Gsv list_of_split_expr;
  std::for_each(in.begin(), in.end(), [&list_of_split_expr](std::string const &in) {
    std::string::size_type npos   = 0;
    std::string::size_type oldPos = 0;
    while (npos != std::string::npos)
      {
        npos = in.find_first_of(" ,+-*/", oldPos);
        list_of_split_expr.emplace_back(in.substr(oldPos, npos - oldPos));
        oldPos = npos + 1;
      }
  });

  // remove trailing spaces
  list_of_split_expr.erase(
      std::remove_if(list_of_split_expr.begin(),
                     list_of_split_expr.end(),
                     [](std::string const &in) { return (in == " ") || (in == ""); }),
      list_of_split_expr.end());
  return list_of_split_expr;
}

std::pair<Gsv, Gsv>
    GraphStringFilterTool::separateNodesAndPins(Gsv const &in)
{
  Gsv nodes, pins, input(in);

  std::partition_copy(input.begin(),
                      input.end(),
                      std::back_inserter(nodes),
                      std::back_inserter(pins),
                      [](std::string const &i) { return std::string::npos == i.find("#"); });


  return {nodes, pins};
}

std::pair<std::vector<std::string>, std::vector<std::string>>
    GraphStringFilterTool::separateTwoPinDevicesAndNodes(
        GraphStringFilterTool::StringVec const &in,
        GraphStringFilterTool::StringVec const &twoPinDevices)
{
  Gsv nodes, devices;

  std::partition_copy(in.begin(),
                      in.end(),
                      std::back_inserter(devices),
                      std::back_inserter(nodes),
                      [&twoPinDevices](std::string const &i) {
                        return twoPinDevices.end()
                               == std::find(twoPinDevices.begin(), twoPinDevices.end(), i);
                      });

  return {nodes, devices};
}

Gsv
    GraphStringFilterTool::createPlotStatementsNodes(Gsv const &in)
{
  StringVec plot_statements;

  std::for_each(in.begin(), in.end(), [&plot_statements](std::string const &i) {
    plot_statements.push_back("V(" + i + ")");
  });
  return plot_statements;
}

Gsv
    GraphStringFilterTool::createPlotStatementsDevicesWithPins(
        std::vector<std::pair<std::string, std::string>> const &in)
{
  StringVec plot_statements;

  std::for_each(
      in.begin(), in.end(), [&plot_statements](std::pair<std::string, std::string> const &i) {
        plot_statements.push_back("I(" + i.first + ", " + i.second + ")");
      });

  return plot_statements;
}

Gsv
    GraphStringFilterTool::createPlotStatementsTwoPinDevices(
        GraphStringFilterTool::StringVec const &in)
{
  Gsv plot_statements;

  std::for_each(in.begin(), in.end(), [&plot_statements](std::string const &i) {
    plot_statements.push_back("I(" + i + ")");
  });

  return plot_statements;
}

std::vector<std::pair<std::string, std::string>>
    GraphStringFilterTool::splitDevWithPin(Gsv const &in)
{
  std::vector<std::pair<std::string, std::string>> nodes_and_pins;
  std::for_each(in.begin(), in.end(), [&nodes_and_pins](std::string const &i) {
    auto npos = i.find('#');
    if (npos == std::string::npos)
      {
        nodes_and_pins.emplace_back(i.substr(0, npos), "");
      }
    else
      {
        nodes_and_pins.emplace_back(i.substr(0, npos), i.substr(npos + 1, std::string::npos));
      }
  });
  return nodes_and_pins;
}

std::vector<std::string>::const_iterator
    GraphStringFilterTool::isInDeviceVector(std::vector<std::string> const &device_list,
                                            std::string const &             device_name)
{
  std::vector<std::string>::const_iterator found =
      std::find_if(device_list.begin(), device_list.end(), [&device_name](std::string const &in) {
        std::string dev_name(in);
        auto        npos = in.find("$");
        if (npos != std::string::npos)
          {
            dev_name = in.substr(npos + 1, std::string::npos);
          }
        return dev_name == device_name;
      });
  return found;
}
