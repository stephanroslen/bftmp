// 2020 by Stephan Roslen

#ifndef _BFTMP__BFEXPRESSION_H_
#define _BFTMP__BFEXPRESSION_H_

#include "BFMachine.h"

namespace bf::expression {

template<typename... Exprs>
struct Expression {
  static void work(Machine &machine) {
    ((void) Exprs::work(machine), ...);
  }

  template<typename T>
  using AppendT = Expression<Exprs..., T>;
};

template<Machine::SignedElemType n>
struct Add {
  static void work(Machine &machine) {
    if constexpr (0 != n) {
      machine.add(n);
    }
  }
};

struct Read {
  static void work(Machine &machine) {
    machine.read();
  }
};

struct Write {
  static void work(Machine &machine) {
    machine.write();
  }
};

template<Machine::SignedIdxType n>
struct Move {
  static void work(Machine &machine) {
    if constexpr (0 != n) {
      machine.move(n);
    }
  }
};

template<typename Expr>
struct Loop {
  static void work(Machine &machine) {
    machine.loop([&]() { Expr::work(machine); });
  }
};

}// namespace bf::expression

#endif// _BFTMP__BFEXPRESSION_H_
