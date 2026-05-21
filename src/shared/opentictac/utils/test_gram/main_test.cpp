#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "node_gram.hpp"
#include "pegtl/contrib/tracer.hpp"

#include <vector>
#include <map>

namespace peg = tao::pegtl;

template <typename Rule>
// using myControl= tracer<Rule>;
using myControl= peg::normal<Rule>;

TEST_CASE("Read device string", "[devString]")
{

  std::string              dummy, out;
  std::vector<std::string> outContainer;

  SECTION("list item with space")
  {
    std::string         s("it 1");
    peg::string_input<> in(s, dummy);

    peg::parse<grammar::commaSeparatedList, actions::action, myControl>(in, outContainer);
    CHECK(outContainer == std::vector<std::string>{"it 1"});
  }
  SECTION("list item with space and padding")
  {
    std::string         s("it 1 ");
    peg::string_input<> in(s, dummy);

    peg::parse<grammar::commaSeparatedList, actions::action, myControl>(in, outContainer);
    CHECK(outContainer == std::vector<std::string>{"it 1"});
  }
  SECTION("list")
  {
    std::string         s("it1 , it2 , it 3");
    peg::string_input<> in(s, dummy);

    peg::parse<grammar::commaSeparatedList, actions::action, myControl>(in, outContainer);
    CHECK(outContainer == std::vector<std::string>{"it1", "it2", "it 3"});
  }

  SECTION("list items with multiple spaces")
  {
    peg::string_input<> in("it 1,it 2               3     , it3", dummy);
    peg::parse<grammar::commaSeparatedList, actions::action, myControl>(in, outContainer);

    CHECK(outContainer == std::vector<std::string>{"it 1", "it 2               3", "it3"});
  }

  /*
   *   SECTION("read single string with all kind of chars")
   *   {
   *     std::string s("a1#aasdf{}$ pa1#aasdf{}$");
   *     CAPTURE(s);
   *
   *     peg::string_input<> in(s, dummy);
   *
   *     SECTION("devs")
   *     {
   *             peg::parse<grammar::devString, actions::action>(in, out);
   *
   *             CHECK(out == "a1#aasdf{}$");
   *     }
   *
   *     // peg::parse<grammar::dev, actions::action>(in);
   *     SECTION("dev")
   *     {
   *             peg::parse<grammar::dev, actions::action>(in, out);
   *             CAPTURE(out);
   *             CHECK(out.empty());
   *     }
   *   }
   */
}

TEST_CASE("list in parenthesis", "[devString]")
{

  std::string              dummy;
  std::vector<std::string> out;

  SECTION("single item in parenthesis") {
    peg::string_input<> in("(it1)", dummy);
    peg::parse<grammar::commaSeparatedList, actions::action, myControl>(in, out);

    CHECK(out == std::vector<std::string>{"it1"});
  }
  SECTION("single item in parenthesis inner padding") {
    peg::string_input<> in("(it1 )", dummy);
    peg::parse<grammar::commaSeparatedList, actions::action, myControl>(in, out);

    CHECK(out == std::vector<std::string>{"it1"});
  }
  SECTION("single item in parenthesis inner padding") {
    peg::string_input<> in("( it1 )", dummy);
    peg::parse<grammar::commaSeparatedList, actions::action, myControl>(in, out);

    CHECK(out == std::vector<std::string>{"it1"});
  }
  SECTION("single item in parenthesis with blanks") {
    peg::string_input<> in("( i t 1)", dummy);
    peg::parse<grammar::commaSeparatedList, actions::action, myControl>(in, out);

    CHECK(out == std::vector<std::string>{"i t 1"});
  }
  SECTION("multiple items in parenthesis with blanks") {
    peg::string_input<> in("( i t 1, it2)", dummy);
    peg::parse<grammar::commaSeparatedList, actions::action, myControl>(in, out);

    CHECK(out == std::vector<std::string>{"i t 1","it2"});
  }
  SECTION("Nested parenthesis") {
    peg::string_input<> in("( i t 1, (it2))", dummy);
    peg::parse<grammar::commaSeparatedList, actions::action, myControl>(in, out);

    CHECK(out == std::vector<std::string>{"i t 1","it2"});
  }
  SECTION("Nested parenthesis") {
    peg::string_input<> in(" i t 1, (it2)", dummy);
    peg::parse<grammar::commaSeparatedList, actions::action, myControl>(in, out);

    CHECK(out == std::vector<std::string>{"i t 1","it2"});
  }
  SECTION("function calls"){
    peg::string_input<> in("V( i t 1, (it2))", dummy);
    peg::parse<grammar::commaSeparatedList, actions::action, myControl>(in, out);

    CHECK(out == std::vector<std::string>{"i t 1","it2"});
  }
  SECTION("multiple function calls"){
    peg::string_input<> in("V( i t 1, IT(it2)) , A( b)", dummy);
    peg::parse<grammar::commaSeparatedList, actions::action, myControl>(in, out);

    CHECK(out == std::vector<std::string>{"i t 1","it2","b"});
  }
}

TEST_CASE("Identify pins")
{
  std::string           dummy;
  std::map<std::string,std::string> out;
  SECTION("find pin")
  {
    peg::string_input<> in("dev#pin", dummy);
    peg::parse<grammar::commaSeparatedList, actions::pinAction, myControl>(in, out);

    CHECK(out == std::map<std::string,std::string>{{"dev","pin"}});
  }
  SECTION("find pin in list")
  {
    peg::string_input<> in("a, dev#pin", dummy);
    peg::parse<grammar::commaSeparatedList, actions::pinAction, myControl>(in, out);

    CHECK(out == std::map<std::string,std::string>{{"dev","pin"}});
  }
  SECTION("find pin in list and parent")
  {
    peg::string_input<> in("V(a, (dev#pin))", dummy);
    peg::parse<grammar::commaSeparatedList, actions::pinAction, myControl>(in, out);

    CHECK(out == std::map<std::string,std::string>{{"dev","pin"}});
  }
  SECTION("find pin in list and parent and func")
  {
    peg::string_input<> in("V(a, I(dev#pin))", dummy);
    peg::parse<grammar::commaSeparatedList, actions::pinAction, myControl>(in, out);

    CHECK(out == std::map<std::string,std::string>{{"dev","pin"}});
  }
}

TEST_CASE("Test operators")
{
  std::string dummy;
  std::map<std::string, std::string> out;
  std::vector<std::string> outs;
  SECTION("try single operator")
  {
    peg::string_input<> in("a + b", dummy);
    peg::parse<grammar::commaSeparatedList, actions::action, myControl>(in, outs);

    CHECK(outs == std::vector<std::string>{"a","b"});
  }
  SECTION("multiple operators")
  {
    peg::string_input<> in("V(a) - V(b-c,asdf), I(dev1#pin2)", dummy);
    SECTION("w/o pins")
    {
      peg::parse<grammar::commaSeparatedList, actions::action, myControl>(in, outs);
      CHECK(outs == std::vector<std::string>{"a", "b", "c", "asdf"});
    }

    SECTION("w/o pins")
    {
      BENCHMARK("test benchmark")
      {
        peg::parse<grammar::commaSeparatedList, actions::pinAction, myControl>(in, out);
      }
      CHECK(out == std::map<std::string, std::string>{{"dev1", "pin2"}});
    }
  }
}

