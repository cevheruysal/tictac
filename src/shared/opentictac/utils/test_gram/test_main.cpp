#include "node_gram.hpp"
#include "pegtl.hpp"
#include "pegtl/contrib/tracer.hpp"


#include <iostream>
#include <sstream>

int
    main()
{
  std::cout << "bla" << std::endl;

  std::string outString;
  std::string dummy;

  std::string testString("(it1,    it#2 , asdfas)");
  std::ostringstream outStream;
  tao::pegtl::string_input<> stringInput(testString, dummy);

  // tao::pegtl::parse<grammar::parent,
  // actions::printAction>(tao::pegtl::string_input<>(testString,dummy));
  /*
   * tao::pegtl::parse<grammar::parent, actions::standard_action>(
   *     tao::pegtl::string_input<>(testString, dummy), outStream);
   */
  /*
   * tao::pegtl::parse<grammar::parent, actions::standard_action, controls::debugControl>(stringInput,
   *                                                                                       outStream);
   */
  // tao::pegtl::parse<grammar::parent, actions::standard_action, controls::debugControl>(stringInput);
  // tao::pegtl::parse<grammar::parent, actions::testAction,controls::debugControl>(stringInput);
  tao::pegtl::parse<grammar::parent, actions::testAction,tracer>(stringInput);
  // tao::pegtl::parse<grammar::parent, actions::standard_action>(stringInput);
  /*
   * tao::pegtl::parse<grammar::parent,tao::pegtl::nothing, controls::indentControl>(
   *     tao::pegtl::string_input<>(testString, dummy), outStream);
   */
  // tao::pegtl::parse<grammar::parent, actions::printAction, tao::pegtl::normal>(
      // tao::pegtl::string_input<>(testString, dummy), outStream);

  std::cout << outStream.str() << std::endl;

  // std::string testString("it1, it2");
  // tao::pegtl::string_input<> in(testString,dummy);
  // tao::pegtl::parse<gram::dev, gram::action>(in);
}
