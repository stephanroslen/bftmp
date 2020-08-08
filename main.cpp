/* 2020 by Stephan Roslen */

#include <boost/hana.hpp>

#include "BFExpression.h"
#include "BFMachine.h"
#include "BFParser.h"

int main(int, const char *[]) {
  constexpr auto rawProgram = BOOST_HANA_STRING(R"(
++++++++++
[>+++++++>++++++++++>++++>+++>+<<<<<-]
>++.>+.+++++++..+++.>++++.>++.<<<+++++++++++++++.>.+++.------.--------.>>>>+++
[->+++++++++++<]
>.>++
[->+++++<]
>.
  )");
  constexpr auto programV = bf::parser::parse(rawProgram);
  using Program = typename decltype(programV)::type;
  bf::Machine machine;
  Program::work(machine);
  return 0;
}
