#include <array>
#include <random>

class CItem {
public:
    const int m_nVal;

//    constexpr CItem() : m_nVal(0) {}

    constexpr CItem(const int nVal) : m_nVal(nVal) {}
};

#ifdef __cplusplus
#if __cplusplus >= 201402L

// Using the std lib
template<typename t_EEnum, class t_cClass>
class LUT_StdLib {
private:
    using TPair = std::pair<t_EEnum, t_cClass>;

    static constexpr size_t m_uSize = static_cast<size_t>(t_EEnum::None);
    std::array<TPair, m_uSize> m_arItems;

//    template<std::array<TPair, m_uSize>::iterator q>
//    static constexpr void bleh() {
//        if constexpr (q->first == q->first) {}
//    }
//
//    template<size_t N, const std::initializer_list<TPair>::iterator q>
//    static constexpr void SanityCheck() {
//        []<std::size_t... Is>(std::index_sequence<Is...>) {
//            (bleh<q+Is, q+Is>(), ...);
//        }(std::make_index_sequence<N>{});
//    }

    template<class t_cIter, size_t... t_uI>
    constexpr LUT_StdLib(t_cIter Iter, std::index_sequence<t_uI...>)
            : m_arItems{((void) t_uI, *Iter++)...} {}

    template<size_t... t_uI>
    constexpr LUT_StdLib(const TPair &value, std::index_sequence<t_uI...>)
            : m_arItems{((void) t_uI, value)...} {}

public:
    constexpr LUT_StdLib(const std::initializer_list<TPair> InitList) : LUT_StdLib(InitList.begin(),
                                                                             std::make_index_sequence<m_uSize>()) {
        //@todo: find a way to check InitList in compile time
        //        SanityCheck<m_uSize, InitList.begin()>();
    }

    constexpr const t_cClass &at(const t_EEnum Index) const {
        return m_arItems[static_cast<size_t>(Index)].second;
    }

    template<t_EEnum t_eIndex>
    constexpr const t_cClass &at() const {
        static_assert(static_cast<size_t>(t_eIndex) < m_uSize, "Out of bounds");
        return m_arItems[static_cast<size_t>(t_eIndex)].second;
    }
};

#endif
#endif

//------------------------------------------------
/*
 * template <t_EEnum, t_cClass>
 * maps t_EEnum -> t_cClass
 */
//------------------------------------------------

enum class ENUM {
    a, b, c, d, None
};

int main() {
    int rand = std::rand() % 4;

    constexpr LUT_StdLib<ENUM, CItem> LUT_stdlib{
            {ENUM::a, CItem(0)},
            {ENUM::b, CItem(1)},
            {ENUM::c, CItem(2)},
            {ENUM::d, CItem(3)}
    };

    [[maybe_unused]] int x = 100;

    if (LUT_stdlib.at(ENUM::a).m_nVal == rand) {
        x += LUT_stdlib.at<ENUM::c>().m_nVal;
    }
    if (LUT_stdlib.at<ENUM::b>().m_nVal == rand) {
        x += LUT_stdlib.at<ENUM::c>().m_nVal;
    }

    static_assert(LUT_stdlib.at(ENUM::a).m_nVal == 0);
    static_assert(LUT_stdlib.at(ENUM::b).m_nVal != 0);
    static_assert(LUT_stdlib.at<ENUM::c>().m_nVal == 2);
    static_assert(LUT_stdlib.at<ENUM::d>().m_nVal != 2);

    return 0;
}
