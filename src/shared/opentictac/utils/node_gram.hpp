#ifndef NODE_GRAMMAR_HPP
#define NODE_GRAMMAR_HPP
#include "pegtl.hpp"
#include "pegtl/contrib/tracer.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>


namespace grammar_old
{
using namespace tao::pegtl;
using namespace tao::pegtl::ascii;

struct notAllowedChar : sor<space, one<',', '(', ')'>>
{
};
struct nameChar : minus<any, notAllowedChar>
{
};
struct devString : plus<nameChar>
{
};
struct devStringNoHash : plus<minus<nameChar, one<'#'>>>
{
};
struct dev : pad<devString, space>
{
};
struct devNoHash : pad<devStringNoHash, space>
{
};
struct list : list_must<dev, one<','>, space>
{
};
struct listNoHash : list_must<devNoHash, one<','>, space>
{
};
struct parent
    : sor<try_catch<if_must<one<'('>, listNoHash, one<')'>>>, if_must<one<'('>, list, one<')'>>>
{
};


}  // namespace grammar_old

namespace actions_old
{

using namespace tao::pegtl;
using namespace tao::pegtl::ascii;
template <typename Rule>
struct standard_action
{
  template <typename Input>
  static void apply(Input const& in, std::string& out)
  {
    out.append(in.string());
    std::cout << "found something string" << std::endl;
  }

  template <typename Input>
  static void apply(Input const& in, std::ostringstream& out)
  {
    out << in.string();
    std::cout << "found something" << std::endl;
  }

  template <typename Input>
  static void apply(Input const& in)
  {
    std::cout << "This is: |" << in.string() << '|' << std::endl;
  }
  template <typename Input, typename... States>
  static void apply(Input const& in, States&&...)
  {
    std::cout << "found something alg" << std::endl;
  }
};

template <>
struct standard_action<grammar_old::devString>
{
  template <typename Input>
  static void apply(Input const& in)
  {
    std::cout << "!!!!! found dev " << in.string() << std::endl;
  }
};

template <typename Rule>
// struct action : nothing<Rule>
struct action : standard_action<Rule>
{
};

template <>
struct action<grammar_old::parent>
{
  template <typename Input>
  static void apply(Input const& in)
  {
    std::cout << "Found paren " << in.string() << std::endl;
  }
};

template <>
struct action<grammar_old::devString>
{
  template <typename Input>
  static void apply(Input const& in)
  {
    std::cout << "This is the item: " << in.string() << std::endl;
  }

  template <typename Input>
  static void apply(Input const& in, std::string& out)
  {
    out.append(in.string());
  }
};

template <>
struct action<grammar_old::dev>
{
};


template <typename Rule>
struct printAction
{
  template <typename Input>
  static void apply(Input const& in)
  {
    std::cout << in.string() << std::endl;
  }
};

template <typename Rule>
struct testAction : nothing<Rule>
{
};

template <>
struct testAction<grammar_old::devString>
{
  template <typename Input>
  static void apply(Input const& in)
  {
    std::cout << "Found general devString " << in.string() << std::endl;
  }
};
}  // namespace actions_old

namespace controls_old
{
using namespace tao::pegtl;
using namespace tao::pegtl::ascii;
template <typename Rule>
struct debugControl : normal<Rule>
{
  template <typename Input, typename... States>
  static void start(Input const& in, States&&...)
  {
    // std::cout << in.string() << std::endl;
    std::cout << "start " << internal::demangle<Rule>() << std::endl << "  ";
    // std::cout << "start " << typeid(Input).name() << std::endl;
    // std::cout << in.buffer()<< std::endl;
  }
  template <typename Input, typename... States>
  static void failure(Input const& in, States&&...)
  {
    std::cout << "failure " << internal::demangle<Rule>() << std::endl;
  }

  template <typename Input, typename... States>
  static void success(Input const& in, States&&...)
  {
    std::cout << "success " << internal::demangle<Rule>() << std::endl;
  }


  template <typename Input>
  static void start(Input const& in, std::ostringstream& out)
  {
    out << "  ";
  }
  template <typename Input>
  static void success(Input const& in, std::ostringstream& out)
  {
    out << " successful" << std::endl;
  }
  template <typename Input>
  static void failure(Input const& in, std::ostringstream& out)
  {
    out << " failed" << std::endl;
  }
};

}  // namespace controls_old


namespace grammar
{
using namespace tao::pegtl;
using namespace tao::pegtl::ascii;
/*
 * template <int Char>
 * struct stringWithout : star<not_one<Char, 10, 13>>
 * {
 * };
 */
template <int... Chars>
struct stringWithout : plus<not_one<10, 13, Chars...>>
{
};

struct operators : one<'+', '-', '*', '/'>
{
};

struct pinDivider : one<'#'>
{
};

struct listDivider : one<','>
{
};

struct braces : one<'(', ')'>
{
};

struct blanks : one<' '>
{
};

template <typename... Rules>
struct stringWO : plus<minus<any, sor<Rules...>>>
{
};

struct stringWOany : stringWO<braces, blanks, operators, pinDivider, listDivider>
{
};

struct stringWOanyWithPin : stringWO<braces, blanks, operators, listDivider>
{
};

struct commaSeparatedListItemSplit : stringWOanyWithPin
{
};
struct deviceAndPin : seq<stringWOany, pinDivider, stringWOany>
{
};

struct commaSeparatedListItem : list<commaSeparatedListItemSplit, plus<space>>
{
};
struct listItem : sor<deviceAndPin, commaSeparatedListItem>
{
};

// forward declaration
struct parenthesis;

struct commaSeparatedList
    : pad<list<sor<parenthesis, listItem>, sor<operators, listDivider>, space>, space>
{
};
struct parenthesis
    : seq<opt<stringWOany>, if_must<one<'('>, until<one<')'>, pad<commaSeparatedList, space>>>>
{
};
}  // namespace grammar



namespace actions
{
using namespace tao::pegtl;
using namespace tao::pegtl::ascii;
/*
 * template <typename Rule>
 * struct action
 * {
 *   template <typename Input, typename States...>
 *   static void apply(Input const& in, States&&...)
 *   {
 *     // std::cout << "found something alg" << std::endl;
 *   }
 * };
 */
template <typename Rule>
struct action : nothing<Rule>
{
};
template <typename Rule>
struct pinAction : nothing<Rule>
{
};


template <>
struct action<grammar::commaSeparatedListItem>
// struct action<grammar::commaSeparatedListItemSplit>
{
  template <typename Input, typename stringContainer>
  static void apply(Input const& in, stringContainer& out)
  {
    out.emplace_back(in.string());
  }
};

template <>
struct pinAction<grammar::deviceAndPin>
{
  template <typename Input, typename stringContainer>
  static void apply(Input const& in, stringContainer& out)
  {
    auto s    = in.string();
    auto npos = s.rfind('#');
    out.emplace(s.substr(0, npos), s.substr(npos + 1, std::string::npos));
  }
};

}  // namespace actions

#endif
