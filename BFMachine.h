/* 2020 by Stephan Roslen */

#ifndef BFTMP__BFMACHINE_H_
#define BFTMP__BFMACHINE_H_

#include <array>
#include <cstdint>
#include <cstdio>
#include <type_traits>
#include <utility>

namespace bf {

class Machine {
 public:
  using ElemType = unsigned char;
  using SignedElemType = std::make_signed_t<ElemType>;
  static_assert(std::is_unsigned_v<ElemType>);

  using IdxType = uint16_t;
  using SignedIdxType = std::make_signed_t<IdxType>;
  static_assert(std::is_unsigned_v<IdxType>);

 private:
  template<typename T, size_t n, size_t... idx>
  static constexpr std::array<T, n> makeFilledArrayHelper(T v, std::index_sequence<idx...>) {
    return std::array<T, n>{(static_cast<void>(idx), v)...};
  }

  template<typename T, size_t n>
  static constexpr std::array<T, n> makeFilledArray(T v) {
    return makeFilledArrayHelper<T, n>(v, std::make_index_sequence<n>{});
  }

  constexpr static size_t kCells{30000};

  std::array<ElemType, kCells> mData{makeFilledArray<ElemType, kCells>(0)};
  uint16_t mRef{0};  // index of current element in mData
  ElemType mValue{0};// cached value of current element in mData - synced by left/right calls

 public:
  void add(SignedElemType n) {
    mValue += static_cast<ElemType>(n);
  }

  void read() {
    mValue = std::getchar();
  }

  void write() const {
    std::putchar(mValue);
  }

  void move(SignedIdxType n) {
    mData[mRef] = mValue;
    mRef += static_cast<IdxType>(n);
    mValue = mData[mRef];
  };

  template<typename Morphism>
  void loop(Morphism &&morphism) const {
    while (0 != mValue)
      morphism();
  }
};

}// namespace bf

#endif// BFTMP__BFMACHINE_H_
