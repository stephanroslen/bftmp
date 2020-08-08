// 2020 by Stephan Roslen

#ifndef _BFTMP__BFPARSER_H_
#define _BFTMP__BFPARSER_H_

#include <boost/hana.hpp>

#include "BFExpression.h"
#include "BFMachine.h"

namespace bf::parser {

template<typename TInput, typename TAdd>
static constexpr auto parseAddExpression(TInput input, TAdd add) {
  namespace hana = boost::hana;

  if constexpr (hana::is_empty(input)) {
    return hana::make_tuple(input, hana::type_c<bf::expression::Add<static_cast<Machine::SignedElemType>(add)>>);
  } else {
    auto input0 = hana::at_c<0>(input);
    [[maybe_unused]] auto inputResidual = hana::drop_front(input);

    if constexpr (input0 == hana::char_c<'+'>) {
      return parseAddExpression(inputResidual, add + hana::int_c<1>);
    } else if constexpr (input0 == hana::char_c<'-'>) {
      return parseAddExpression(inputResidual, add + hana::int_c<-1>);
    } else if constexpr (hana::contains(BOOST_HANA_STRING(".,<>[]"), input0)) {
      return hana::make_tuple(input, hana::type_c<bf::expression::Add<static_cast<Machine::SignedElemType>(add)>>);
    } else {
      return parseAddExpression(inputResidual, add);
    }
  }
}

template<typename TInput, typename TMove>
static constexpr auto parseMoveExpression(TInput input, TMove move) {
  namespace hana = boost::hana;

  if constexpr (hana::is_empty(input)) {
    return hana::make_tuple(input, hana::type_c<bf::expression::Move<static_cast<Machine::SignedIdxType>(move)>>);
  } else {
    auto input0 = hana::at_c<0>(input);
    [[maybe_unused]] auto inputResidual = hana::drop_front(input);

    if constexpr (input0 == hana::char_c<'<'>) {
      return parseMoveExpression(inputResidual, move + hana::int_c<-1>);
    } else if constexpr (input0 == hana::char_c<'>'>) {
      return parseMoveExpression(inputResidual, move + hana::int_c<1>);
    } else if constexpr (hana::contains(BOOST_HANA_STRING("+-.,[]"), input0)) {
      return hana::make_tuple(input, hana::type_c<bf::expression::Move<static_cast<Machine::SignedIdxType>(move)>>);
    } else {
      return parseMoveExpression(inputResidual, move);
    }
  }
}

template<typename TInput, typename TExpression>
static constexpr auto parseExpression(TInput input, TExpression expression) {
  namespace hana = boost::hana;

  if constexpr (hana::is_empty(input)) {
    return hana::make_tuple(input, expression);
  } else {
    auto input0 = hana::at_c<0>(input);
    auto inputResidual = hana::drop_front(input);

    if constexpr (input0 == hana::char_c<'+'>) {
      auto result = parseAddExpression(inputResidual, hana::int_c<1>);
      auto inputResidualResult = hana::at_c<0>(result);
      auto addExpressionResult = hana::at_c<1>(result);
      auto newExpression = hana::type_c<typename decltype(expression)::type::template AppendT<
          typename decltype(addExpressionResult)::type>>;
      return parseExpression(inputResidualResult, newExpression);
    } else if constexpr (input0 == hana::char_c<'-'>) {
      auto result = parseAddExpression(inputResidual, hana::int_c<-1>);
      auto inputResidualResult = hana::at_c<0>(result);
      auto addExpressionResult = hana::at_c<1>(result);
      auto newExpression = hana::type_c<typename decltype(expression)::type::template AppendT<
          typename decltype(addExpressionResult)::type>>;
      return parseExpression(inputResidualResult, newExpression);
    } else if constexpr (input0 == hana::char_c<','>) {
      auto newExpression = hana::type_c<typename decltype(expression)::type::template AppendT<bf::expression::Read>>;
      return parseExpression(inputResidual, newExpression);
    } else if constexpr (input0 == hana::char_c<'.'>) {
      auto newExpression = hana::type_c<typename decltype(expression)::type::template AppendT<bf::expression::Write>>;
      return parseExpression(inputResidual, newExpression);
    } else if constexpr (input0 == hana::char_c<'<'>) {
      auto result = parseMoveExpression(inputResidual, hana::int_c<-1>);
      auto inputResidualResult = hana::at_c<0>(result);
      auto moveExpressionResult = hana::at_c<1>(result);
      auto newExpression = hana::type_c<typename decltype(expression)::type::template AppendT<
          typename decltype(moveExpressionResult)::type>>;
      return parseExpression(inputResidualResult, newExpression);
    } else if constexpr (input0 == hana::char_c<'>'>) {
      auto result = parseMoveExpression(inputResidual, hana::int_c<1>);
      auto inputResidualResult = hana::at_c<0>(result);
      auto moveExpressionResult = hana::at_c<1>(result);
      auto newExpression = hana::type_c<typename decltype(expression)::type::template AppendT<
          typename decltype(moveExpressionResult)::type>>;
      return parseExpression(inputResidualResult, newExpression);
    } else if constexpr (input0 == hana::char_c<'['>) {
      // Parse the loop body into new expression
      auto emptyExpression = hana::type_c<bf::expression::Expression<>>;
      auto result = parseExpression(inputResidual, emptyExpression);
      auto inputResidualResult = hana::at_c<0>(result);
      auto expressionResult = hana::at_c<1>(result);
      // Wrap the expression
      auto loopExpressionResult = hana::type_c<bf::expression::Loop<typename decltype(expressionResult)::type>>;
      // Concat
      auto newExpression = hana::type_c<
          typename decltype(expression)::type::template AppendT<typename decltype(loopExpressionResult)::type>>;
      // Continue parsing
      return parseExpression(inputResidualResult, newExpression);
    } else if constexpr (input0 == hana::char_c<']'>) {
      // Return and don't continue parsing - '[' opened a nested parser which is closed here
      return hana::make_tuple(inputResidual, expression);
    } else {
      // Ignore everything else
      return parseExpression(inputResidual, expression);
    }
  }
};

template<typename TInput>
static constexpr auto parse(TInput &&input) {
  namespace hana = boost::hana;

  auto empty_expression = hana::type_c<bf::expression::Expression<>>;
  auto result = parseExpression(input, empty_expression);
  return hana::at_c<1>(result);
};

}// namespace bf::parser

#endif// _BFTMP__BFPARSER_H_
