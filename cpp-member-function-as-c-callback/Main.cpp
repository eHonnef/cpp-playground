#include <functional>

// Extern C library code that you cannot modify
extern "C" {
#include <stdio.h>

typedef int (*callback_t)(int *, int *);

// This function receives a function as parameter.
// Very common for C libraries.
void register_with_library(callback_t func) {
  int x = 0, y = 1;
  int o = func(&x, &y);
  printf("Value: %i\n", o);
}
}

template <typename T> struct Callback;
template <typename Ret, typename... Params> struct Callback<Ret (*)(Params...)> {
  template <typename... Args> static Ret callback(Args... args) { return func(args...); }
  static std::function<Ret(Params...)> func;
};

template <typename Ret, typename... Params>
std::function<Ret(Params...)> Callback<Ret (*)(Params...)>::func;

class AClass {
private:
  int m_nA;
  int m_nB;

public:
  AClass(int nA, int nB) : m_nA(nA), m_nB(nB) {}

  int OnSum(int *nA, int *nB) { return (*nA) + (*nB) - m_nA - m_nB; }
};

template <typename T, typename C> auto reg(C *obj) -> T {
  // @todo: improve this wrapper

  Callback<T>::func = std::bind(&AClass::OnSum, obj, std::placeholders::_1, std::placeholders::_2);

  T func = static_cast<T>(Callback<T>::callback);

  return func;
}

int main() {
  // Expected result = -4
  AClass a(2, 3);
  register_with_library(reg<callback_t>(&a));

  return 0;
}
