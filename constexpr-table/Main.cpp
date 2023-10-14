#include <iostream>

template <typename t_EEnum> class CA {
public:
  int m_nInt;
  t_EEnum m_eLookupValue;

  constexpr CA() : m_nInt(0), m_eLookupValue(t_EEnum::None) {}
  constexpr CA(int nInt, t_EEnum eLookup) : m_nInt(nInt), m_eLookupValue(eLookup) {}
  constexpr CA(const CA &cpy) : m_nInt(cpy.m_nInt), m_eLookupValue(cpy.m_eLookupValue) {}
};

template <typename t_EEnum> class CTable {
public:
  static constexpr unsigned m_uSize = static_cast<unsigned>(t_EEnum::None);
  CA<t_EEnum> m_arTable[m_uSize];

  template <unsigned n, class t_CFirst, class... t_COthers>
  inline constexpr void FillTable(const t_CFirst& First, const t_COthers&... Others) {
    FillTable<t_COthers...>(Others...);
    m_arTable[0] = First;
  }

  template <class t_CFirst, class... t_COthers>
  inline constexpr void FillTable(const t_CFirst& First, const t_COthers&... Others) {
    m_arTable[sizeof...(t_COthers)] = First;
    FillTable<sizeof...(t_COthers)-1, t_CFirst, t_COthers...>(First, Others...);
  }

  template <class t_CFirst, class... t_COthers>
  inline constexpr CTable(const t_CFirst &First, const t_COthers &...Others) {
    FillTable<sizeof...(t_COthers) - 1, t_CFirst, t_COthers...>(First, Others...);
  }
};

//------------------------------------------------

enum class ENUM { a, b, c, None };

int main() {
  CTable<ENUM> table(CA(1, ENUM::a), CA(2, ENUM::b), CA(3, ENUM::c));

  for (int i = 0; i < 3; ++i)
    std::printf("%d\n", table.m_arTable[i].m_nInt);

  return 0;
}
