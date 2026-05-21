//
// Created by kowitz on 8/6/18.
//
#include "catch.hpp"
#include "trompeloeil.hpp"

#include "GraphStringFilter.hpp"

namespace trompeloeil
{
template <>
void
    reporter<specialized>::send(severity s, const char *file, unsigned long line, const char *msg)
{
  std::ostringstream os;
  if (line) os << file << ':' << line << '\n';
  os << msg;
  auto failure = os.str();
  if (s == severity::fatal)
    {
      FAIL(failure);
    }
  else
    {
      CAPTURE(failure);
      CHECK(failure.empty());
    }
}
}  // namespace trompeloeil

class IP_XCallMock
{
 public:
  MAKE_MOCK0(getPinNameBinding, std::vector<std::string>(void));
};

class ExpansionStackMock
{
 public:
  MAKE_MOCK1(getXCall, IP_XCallMock *(std::string const &));
};


TEST_CASE("epxressions in parentheses are correctly extracted", "[graph_string]")
{
  using Gsv     = std::vector<std::string>;
  namespace GSF = GraphStringFilterTool;
  //  using reporter_func =
  //      std::function<void(trompeloeil::severity, char const* file, unsigned long line,
  //      std::string const& msg)>;
  //  reporter_func trompeloeil::set_reporter(reporter_func new_reporter);

  SECTION("single string output")
  {
    Gsv expected{{"S1"}};
    SECTION("no parenthesis")
    {
      std::string s("S1");

      CHECK(GSF::findExprInParentheses(s) == expected);
    }

    SECTION("single parenthesis")
    {
      std::string s("V(S1)");
      CHECK(GSF::findExprInParentheses(s) == expected);
    }

    SECTION("with stuff after")
    {
      std::string s("V(S1)asdf");
      CHECK(GSF::findExprInParentheses(s) == expected);
    }
  }

  SECTION("Multiple expr extraction")
  {
    Gsv expected{"S1", "V2#p"};
    SECTION("multiple parentheses")
    {
      std::string s("V(S1) - V(V2#p)");
      CHECK(GSF::findExprInParentheses(s) == expected);
    }
  }

  SECTION("Whitespace removal")
  {
    Gsv expected{"S1", "V2#p"};
    SECTION("with whitespaces")
    {
      Gsv s{{"S1 V2#p"}};
      CHECK(GSF::splitAtOperators(s) == expected);
    }
    SECTION("with operators and whitespaces")
    {
      Gsv s{{"S1 + V2#p"}};
      CHECK(GSF::splitAtOperators(s) == expected);
      s = {{"S1,V2#p"}};
      CHECK(GSF::splitAtOperators(s) == expected);
      s = {{"S1*V2#p"}};
      CHECK(GSF::splitAtOperators(s) == expected);
      s = {{"S1/V2#p"}};
      CHECK(GSF::splitAtOperators(s) == expected);
      s = {{"/S1 /V2#p/"}};
      CHECK(GSF::splitAtOperators(s) == expected);
    }
  }

  SECTION("Whitespaces, Operators and parenthesies")
  {
    Gsv expected{"S1", "V2#p"};

    std::string s{"V(S1) + V(V2#p)"};
    CHECK(GSF::splitAtOperators(GSF::findExprInParentheses(s)) == expected);
    s = "(V(S1) + V(V2#p))";
    CHECK(GSF::splitAtOperators(GSF::findExprInParentheses(s)) == expected);
    s = "(V(S1) + V(V2#p)V)";
    CHECK(GSF::splitAtOperators(GSF::findExprInParentheses(s)) == expected);
    s = "(V(S1, V2#p)V)";
    CHECK(GSF::splitAtOperators(GSF::findExprInParentheses(s)) == expected);
  }

  SECTION("check seperation of pins and nodes"){}
  {
    std::pair<Gsv, Gsv> expected{{"S1"}, {"V2#p"}};

    Gsv s{"S1", "V2#p"};
    CHECK(GSF::separateNodesAndPins(s) == expected);
  }

  SECTION("check separation of two pin devices and nodes")
  {
    std::pair<Gsv, Gsv> expected{{"S1"}, {"S2"}};
    Gsv                 s{"S1", "S2"};
    Gsv                 twoPinDev{"S1"};

    std::pair<Gsv, Gsv> result(GSF::separateTwoPinDevicesAndNodes(s, twoPinDev));
    CHECK(result.first == expected.first);
    CHECK(result.second == expected.second);
  }

  SECTION("check creation of plot statements for nodes"){}
  {
    Gsv expected{"V(S1)", "V(V2#p)"};
    Gsv s{"S1", "V2#p"};

    CHECK(GSF::createPlotStatementsNodes(s) == expected);
  }

  SECTION("check creation of plot statements for nodes")
  {
    Gsv expected{"I(S1)", "I(V2#p)"};
    Gsv s{"S1", "V2#p"};

    CHECK_THAT(GSF::createPlotStatementsTwoPinDevices(s), Catch::Matchers::Contains(expected));
  }

  SECTION("check creation of plot statements for pins of devices")
  {
    std::vector<std::pair<std::string, std::string>> s{{"V2", "p"}, {"S1", "1"}};
    Gsv                                              expected{"I(V2, p)", "I(S1, 1)"};

    CHECK_THAT(GSF::createPlotStatementsDevicesWithPins(s), Catch::Matchers::Contains(expected));
  }

  SECTION("check split for pins")
  {
    Gsv                                              s{"R2#d", "V2#p", "S1"};
    std::vector<std::pair<std::string, std::string>> expected{{"R2", "d"}, {"V2", "p"}, {"S1", ""}};

    auto result = GSF::splitDevWithPin(s);

    for (int i = 0; i < result.size(); ++i)
      {
        CAPTURE(result[i].first);
        CAPTURE(result[i].second);
        CHECK(result[i].first == expected[i].first);
        CHECK(result[i].second == expected[i].second);
      }
  }

  SECTION("check detection of pinnames")
  {
    ExpansionStackMock            m;
    std::shared_ptr<IP_XCallMock> xcall(new IP_XCallMock());
    IP_XCallMock *                xcall_raw = xcall.get();

    REQUIRE_CALL(m, getXCall(trompeloeil::_)).RETURN(xcall_raw);
    REQUIRE_CALL(*xcall, getPinNameBinding()).RETURN(Gsv{"d", "g", "s", "b"});

    Gsv expected{"d", "g", "s", "b"};

    CHECK(GSF::getPinnames<ExpansionStackMock, IP_XCallMock>("device_name", m) == expected);
  }

  SECTION("check the getters")
  {
    ExpansionStackMock            m;
    std::shared_ptr<IP_XCallMock> xcall(new IP_XCallMock());

    ALLOW_CALL(m, getXCall(trompeloeil::_)).RETURN(xcall.get());
    ALLOW_CALL(*xcall, getPinNameBinding()).RETURN(Gsv{"d", "g", "s", "b"});

    std::string input("I(R1#d) + V(node) + I(R2) + I(R2#n)");

    std::vector<std::string> twoPinDevs{"R2"};
    std::vector<std::string> threePinDevs{"Q2"};

    GraphStringFilter<ExpansionStackMock> f(input, m, threePinDevs, twoPinDevs);

    SECTION("check getters")
    {
      std::vector<std::string> expected{"I(R2)"};

      CHECK(f.getTwoPinDevicePlotStatements() == expected);

      expected = {"V(node)"};
      CHECK(f.getNodePlotStatements() == expected);

      expected = {"I(R1, 1)", "I(R2, 2)"};
      CHECK(f.getDevicePinPlotStatements<IP_XCallMock>() == expected);
    }

    SECTION("check adapted string"){}
    {
      std::string expected("I(R1#1) + V(node) + I(R2) + I(R2#n)");

      std::string result;
      result = f.getAdaptedGraphString<IP_XCallMock>();
      CHECK(result == expected);
    }


    SECTION("Invalid input")
    {
      std::string                           invalid_input("I(R1#d) + V(node) + I(R2) + I(R2#x)");
      GraphStringFilter<ExpansionStackMock> f_invalid(invalid_input, m, threePinDevs, twoPinDevs);
      CHECK_THROWS(f_invalid.getAdaptedGraphString<IP_XCallMock>());
    }
  }
}
