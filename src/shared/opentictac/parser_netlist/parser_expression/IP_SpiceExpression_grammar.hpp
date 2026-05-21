#ifndef FITOB_SCRIPT_GRAMMAR_HPP_
#define FITOB_SCRIPT_GRAMMAR_HPP_

#define BOOST_SPIRIT_DEBUG_OUT  //NDEBUG

#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_ast.hpp>
#include <boost/spirit/include/classic_tree_to_xml.hpp>

using namespace BOOST_SPIRIT_CLASSIC_NS;

namespace tictac{

  /** Here we specify the grammar of the Theta scipt
   * The result will be one XML tree which can later be processed
   * and the according object structure will be created */
  struct ThetaParser : public grammar<ThetaParser>
  {
    static const int constantID = 1;
    static const int factorID = 2;
    static const int termID = 3;
    static const int expressionID = 4;
    static const int variableID = 5;
    static const int endtermID = 6;
    static const int functionCallID = 7;

    static const int unaryopID = 20;
    static const int tenoropID = 21;
    static const int quoteStringID = 22;


    template <typename ScannerT>
    struct definition
    {
        definition(ThetaParser const& /*self*/)
        {
            //  Start grammar definition

            // ---------- Define Expressions ------------
            endterm     =   constant
                        |   functioncall
                        |   variable
                        |   inner_node_d[ch_p('(') >> expression >> ch_p(')')]
                        |   inner_node_d[ch_p('{') >> expression >> ch_p('}')]
                        |   strop
                        ;

            constant     =   discard_node_d[*(blank_p)] >>
                             leaf_node_d[
                               (discard_node_d[*(blank_p)] >>
                                lexeme_d[real_p >>
                                      !(
                                          // ----- here come the scaling factor in the constants
                                        (
                                          ch_p('T')
                                        | ch_p('G')
                                        | ch_p('K')
                                        | ch_p('k')
                                        | (ch_p('M') >> !(ch_p('e') >> ch_p('g')))
                                        | (ch_p('m') >> !(ch_p('e') >> ch_p('g')))
                                        | ch_p('u')
                                        | ch_p('n')
                                        | ch_p('p')
                                        | ch_p('f')
                                        )
                                       )
                                       // ------ here the possible physical units
                                   >> !(
                                          ch_p('V')
                                        | ch_p('v')
                                        | ch_p('F')
                                       )

                                       ]
                               >> discard_node_d[*(blank_p)])
                            ];

            functioncall =  (
                                 variable
                                 >> root_node_d[ch_p('(')]
                                 >> expression
                                 >>  *( discard_first_node_d[ch_p(',') >> expression] )
                                 >> discard_node_d[ch_p(')')]
                            );

            variable    =   leaf_node_d[
                              ( discard_node_d[*(blank_p)]
                              >> lexeme_d[ +( alnum_p
                                            | ch_p('_') //here we allow special characters in Names
                                            )
                                         ] >>
                              discard_node_d[*(blank_p)])
                            ];

            strop       = leaf_node_d[
                                      ch_p('\"')
                                   >> lexeme_d[ +(~ch_p('\"')) ]
                                   >> ch_p('\"')
                                    ];

            unaryop     =  discard_node_d[*(blank_p)] >>
                             ( endterm
                               |
                               *(
                                   (root_node_d[ch_p('-')] >> discard_node_d[*(blank_p)] >> endterm )
                                 | (root_node_d[ch_p('+')] >> discard_node_d[*(blank_p)] >> endterm )
                                 | (root_node_d[ch_p('~')] >> discard_node_d[*(blank_p)] >> endterm )
                                 | (root_node_d[ch_p('!')] >> discard_node_d[*(blank_p)] >> endterm )
                                )
                              );

            factor      =   unaryop >> discard_node_d[*(blank_p)] >>
                            *(   (root_node_d[lexeme_d[chseq_p("**")]] >> discard_node_d[*(blank_p)] >> unaryop )
                               | (root_node_d[lexeme_d[ch_p('^')]]    >> discard_node_d[*(blank_p)] >> unaryop )
                             );

            term        =   factor >> discard_node_d[*(blank_p)] >>
                            *(  (root_node_d[ch_p('*')] >> discard_node_d[*(blank_p)] >> factor)
                              | (root_node_d[ch_p('/')] >> discard_node_d[*(blank_p)] >> factor)
                             );

            tenorop     =   term >> discard_node_d[*(blank_p)] >>
                             *(
                                root_node_d[ch_p('?')] >> discard_node_d[*(blank_p)] >>
                                term >> discard_node_d[*(blank_p)] >>
                                discard_node_d[ch_p(':')] >> discard_node_d[*(blank_p)] >>
                                term
                              );


            expression  =   (tenorop >> discard_node_d[*(blank_p)] >>
                               *(
                                     (root_node_d[lexeme_d[chseq_p("<=")]] >> discard_node_d[*(blank_p)] >> tenorop)
                                   | (root_node_d[lexeme_d[chseq_p(">=")]] >> discard_node_d[*(blank_p)] >> tenorop)
                                   | (root_node_d[lexeme_d[chseq_p("==")]] >> discard_node_d[*(blank_p)] >> tenorop)
                                   | (root_node_d[lexeme_d[chseq_p("!=")]] >> discard_node_d[*(blank_p)] >> tenorop)
                                   | (root_node_d[lexeme_d[chseq_p("||")]] >> discard_node_d[*(blank_p)] >> tenorop)
                                   | (root_node_d[lexeme_d[chseq_p("&&")]] >> discard_node_d[*(blank_p)] >> tenorop)
                                   | (root_node_d[lexeme_d[chseq_p("|")]] >> discard_node_d[*(blank_p)] >> tenorop)
                                   | (root_node_d[lexeme_d[chseq_p("&")]] >> discard_node_d[*(blank_p)] >> tenorop)
                                   | (root_node_d[ch_p('>')] >> discard_node_d[*(blank_p)] >> tenorop)
                                   | (root_node_d[ch_p('<')] >> discard_node_d[*(blank_p)] >> tenorop)
                                   | (root_node_d[ch_p('+')] >> discard_node_d[*(blank_p)] >> tenorop)
                                   | (root_node_d[ch_p('-')] >> discard_node_d[*(blank_p)] >> tenorop)
                                )
                            );

            // ---------- END Define Expressions ------------


            // --------- End grammar definition ------------

            // turn on the debugging info.
            BOOST_SPIRIT_DEBUG_RULE(constant);
            BOOST_SPIRIT_DEBUG_RULE(factor);
            BOOST_SPIRIT_DEBUG_RULE(term);
            BOOST_SPIRIT_DEBUG_RULE(expression);
            BOOST_SPIRIT_DEBUG_RULE(variable);
            BOOST_SPIRIT_DEBUG_RULE(functioncall);

            BOOST_SPIRIT_DEBUG_RULE(unaryop);
            BOOST_SPIRIT_DEBUG_RULE(tenorop);
            BOOST_SPIRIT_DEBUG_RULE(strop);
        }

        rule<ScannerT, parser_context<>, parser_tag<expressionID> >   expression;
        rule<ScannerT, parser_context<>, parser_tag<termID> >         term;
        rule<ScannerT, parser_context<>, parser_tag<factorID> >       factor;
        rule<ScannerT, parser_context<>, parser_tag<constantID> >     constant;
        rule<ScannerT, parser_context<>, parser_tag<variableID> >     variable;
        rule<ScannerT, parser_context<>, parser_tag<endtermID> >      endterm;
        rule<ScannerT, parser_context<>, parser_tag<functionCallID> > functioncall;

        rule<ScannerT, parser_context<>, parser_tag<unaryopID> >      unaryop;
        rule<ScannerT, parser_context<>, parser_tag<tenoropID> >      tenorop;
        rule<ScannerT, parser_context<>, parser_tag<quoteStringID> >  strop;

        rule<ScannerT, parser_context<>, parser_tag<expressionID> > const&
        start() const { return expression; }
    };
  };
}

#endif

