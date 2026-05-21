/*
 * CS_RandomFunction.hpp
 *
 *  Created on: Aug 14, 2014
 *      Author: benk
 */

#ifndef CS_RANDOMFUNCTION_HPP_
#define CS_RANDOMFUNCTION_HPP_

#include "CS_FunctionCall.hpp"

/** enum type for the different random and distribution function */
typedef enum {
  VERILOG_RANDOMFUNC_RANDOM          = 0,  // random
  VERILOG_RANDOMFUNC_ARANDOM         = 1,  // arandom
  VERILOG_RANDOMFUNC_RDISTNORMAL     = 2,  // $rdist_normal
  VERILOG_RANDOMFUNC_RDISTUNIFORM    = 3   // $rdist_uniform
} RandomFuncCallType;

/** enum type for the different random and distribution function */
typedef enum {
  VERILOG_RANDOMFUNC_GLOBAL        = 0,  // global
  VERILOG_RANDOMFUNC_INSTANCE      = 1,  // instance
  VERILOG_RANDOMFUNC_NOT_GIVEN     = 2,  // is not given
} RandomGlobalFunction;

class CS_VariableReference;
class CS_ParameterReference;

/**
 * This class represnets the random and distribution functions in the
 * compiler structure.
 * In LRM the correspondance is:
 *distribution_functions, random_function and analog_random_function
 *
 * */
class CS_RandomFunction : public CS_FunctionCall
{
public:

  /** Ctor of the random or distribution function */
  CS_RandomFunction(RandomFuncCallType ft,
      CS_ExpressionBasis** args, int nrArgs,
      const CS_ModuleDeclaration* module, int startLine, int startColumn);

  /** empty Dtor */
  virtual ~CS_RandomFunction() {;}

  /** returns the type of random or distribution function */
  RandomFuncCallType getFunctionType() const { return type_; }

  /** returns an enum type which shows if this random function is called with "instance"
   * or "global" or none */
  RandomGlobalFunction getInstanceOrGlobal() const { return isGlobal_or_Instance_; }

   /** returns the classes that can be found in an expression (constant, ports, parameters, variables)*/
   virtual int getClasses() const { return (int)CONSTANT; }

   /** @returns true if this is an integer random function call */
   bool isIntegerRandFunction() const {
     return
      (    (type_ == VERILOG_RANDOMFUNC_RANDOM)
        || (type_ == VERILOG_RANDOMFUNC_ARANDOM));
   };

   /** @returns true if the seed is either a variable or a parameter */
   bool hasVariableOrParamSeed() const {
     if ( (seed_var_ == NULL) && (seed_par_ == NULL))
       return false;
     else
       return true;
   }

   /** @returns the seed variable reference */
   const CS_VariableReference* getVariableSeed() const { return seed_var_; }
   CS_VariableReference* getVariableSeed() { return seed_var_; }

   /** @returns the sedd as parameter reference (if the seed is a parameter reference ) */
   const CS_ParameterReference* getParameterSeed() const { return seed_par_; }
   CS_ParameterReference* getParameterSeed() { return seed_par_; }

   /** @returns the sedd as expression */
   const CS_ExpressionBasis* getSeedExpression() const { return seed_expr_; }
   CS_ExpressionBasis* getSeedExpression() { return seed_expr_; }

   /** defines if two random function calls have the same seed and type
    * @param funct the function to compared with
    * @returns true if they have the same seed, false otherwise*/
   bool hasSameSeed(const CS_RandomFunction* func) const;

   /** returns the types (actual INTEGER or REAL)*/
   virtual int getTypes() const {
     if (   isIntegerRandFunction() )
       return INTEGER;
     else
       return REAL;
   }

   /** method for the tree traversal */
   virtual void Accept(CS_CompilerVisitorBasis &visitor) { visitor.Visit( (CS_RandomFunction &)(*this)); }

   /** propagateDependencies() call of CS_RandomFunction should not appear in composed expressions */
   virtual CS_AKDependency propagateDependencies()
     {
       VERILOG_PRINT_L3(verb(), "propagateDependencies() within a CS_RandomFunction should not be called ");
       return CS_AKDependency();
     }

   /** function that creates a deep copy of the expression itself */
   virtual CS_ExpressionBasis* copy() {
     // copy the arguments
     CS_ExpressionBasis *arg[30];
     for (int i = 0; i < this->getNrArg(); i++)
       arg[i] = this->getArg(i)->copy();

     //create random function
     return (new CS_RandomFunction(type_, arg,
         this->getNrArg(), this->getModule(),
         this->getStartLine(), this->getStartColumn()));
   }

private:

  /** type of the random or distribution function */
  RandomFuncCallType type_;

  /** this enum type shows if this function is instance or global or if it is not given*/
  RandomGlobalFunction isGlobal_or_Instance_;

  /** the seed variable */
  CS_VariableReference *seed_var_;

  /** seed parameter */
  CS_ParameterReference *seed_par_;

  /** the seed expression */
  CS_ExpressionBasis *seed_expr_;
};

#endif /* CS_RANDOMFUNCTION_HPP_ */
