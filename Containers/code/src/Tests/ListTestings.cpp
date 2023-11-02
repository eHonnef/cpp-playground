/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <doctest/doctest.h>
#include <Containers/List.hpp>
#include <algorithm>
#include <random>
#include <format>

TEST_SUITE("[]") {
    std::random_device RandomDevice;
    std::mt19937 Generator{RandomDevice()};

    class DefaultConstructor {
    public:
        DefaultConstructor() = default;

        DefaultConstructor(uint32_t i) : m_uInt{i} {}

        bool operator==(const DefaultConstructor &Other) const {
            return Other.Get() == this->Get();
        }

        [[maybe_unused]] uint32_t Get() const {
            return m_uInt;
        }

    private:
        uint32_t m_uInt;
    };

    class NonDefaultConstructor : public DefaultConstructor {
    public:
        NonDefaultConstructor(int a) : DefaultConstructor(a) {}
    };

    class NonCopyableClass : public DefaultConstructor {
    public:
        NonCopyableClass() = default;

        NonCopyableClass(int a) : DefaultConstructor(a) {}

        NonCopyableClass(const NonCopyableClass &) = delete;

        NonCopyableClass &operator=(const NonCopyableClass &) = delete;
    };

    class NonMovableClass : public DefaultConstructor {
    public:
        NonMovableClass() = default;

        NonMovableClass(int a) : DefaultConstructor(a) {}

        NonMovableClass(NonMovableClass &&) = delete;

        NonMovableClass &operator=(NonMovableClass &&) = delete;
    };

    class NonMovableNonCopiable : public DefaultConstructor {
    public:
        NonMovableNonCopiable() = default;

        NonMovableNonCopiable(int a) : DefaultConstructor(a) {}

        NonMovableNonCopiable(const NonMovableNonCopiable &) = delete;

        NonMovableNonCopiable &operator=(const NonMovableNonCopiable &) = delete;

        NonMovableNonCopiable(NonMovableNonCopiable &&) = delete;

        NonMovableNonCopiable &operator=(NonMovableNonCopiable &&) = delete;
    };

    template<typename t_tTestType>
    std::vector<t_tTestType> GetObjects() {
        if constexpr (std::is_same<t_tTestType, NonDefaultConstructor>::value) {
            return std::vector<t_tTestType>{
                    {static_cast<int>(Generator())},
                    {static_cast<int>(Generator())},
                    {static_cast<int>(Generator())},
                    {static_cast<int>(Generator())},
                    {static_cast<int>(Generator())},
                    {static_cast<int>(Generator())}
            };
        } else if constexpr (std::is_same<t_tTestType, std::string>::value) {
            return std::vector<t_tTestType>{
                    {"189c2nudh"},
                    {"static_cast<int>(Generator())"},
                    {"2390 lm,sdf"},
                    {"190-273 mlaskjd"},
                    {"8979812897389&*¨&*%&123"},
                    {"çasdauiqãoaisdu"}
            };
        } else {
            return std::vector<t_tTestType>{
                    t_tTestType{static_cast<t_tTestType>(Generator())},
                    t_tTestType{static_cast<t_tTestType>(Generator())},
                    t_tTestType{static_cast<t_tTestType>(Generator())},
                    t_tTestType{static_cast<t_tTestType>(Generator())}
            };
        }
    };

    template<typename t_tTestType>
    t_tTestType GetRandom() {
        if constexpr (std::is_same<t_tTestType, NonDefaultConstructor>::value) {
            return {static_cast<int>(Generator())};
        } else if constexpr (std::is_same<t_tTestType, std::string>::value) {
            return std::to_string(Generator());
        } else {
            return t_tTestType{static_cast<t_tTestType>(Generator())};
        }
    }

    TEST_CASE("Std lib testings") {
        eho::CListStatic<uint32_t, 128> lst{};
        for (auto &Item: lst) {
            Item = Generator();
        }

        REQUIRE_NOTHROW(std::ranges::shuffle(lst, Generator));
        REQUIRE_NOTHROW(std::ranges::stable_sort(lst));
    }

    TEST_CASE_TEMPLATE("Dynamic list - Non Amortized", t_tTestType, uint32_t, DefaultConstructor,
                       NonDefaultConstructor, std::string) {
        eho::CList<t_tTestType> lst{};
        CHECK(lst.size() == 0);

        SUBCASE("Resize") {
            constexpr size_t arResize[] = {15, 10, 2, 0, 30, 2, 0};
            for (auto i: arResize) {
                lst.resize(i);
                CHECK(lst.size() == 0);
                CHECK(lst.capacity() == i);
            }
            CHECK(lst.empty());

            lst.resize(100);
            CHECK(lst.size() == 0);
            CHECK(lst.capacity() == 100);
            CHECK(lst.empty());

            lst.clear();
            CHECK(lst.capacity() == 0);
            CHECK(lst.size() == 0);
            CHECK(lst.empty());
        }

        SUBCASE("Insertion") {
            SUBCASE("Copy insertion") {
                std::vector<t_tTestType> vecObjects{GetObjects<t_tTestType>()};

                SUBCASE("Begin insertions") {
                    for (const auto &i: vecObjects) {
                        lst.insert(0, i);
                    }

                    CHECK(lst.capacity() == vecObjects.size());
                    CHECK(lst.size() == vecObjects.size());
                    CHECK_FALSE(lst.empty());

                    size_t j = 0;
                    for (size_t i = vecObjects.size(); i-- > 0; ++j) {
                        CHECK(lst.at(j) == vecObjects[i]);
                    }
                }

                SUBCASE("End insertions") {
                    lst.clear();

                    for (const auto &i: vecObjects) {
                        lst.insert(i);
                    }

                    CHECK(lst.capacity() == vecObjects.size());
                    CHECK(lst.size() == vecObjects.size());
                    CHECK_FALSE(lst.empty());

                    for (size_t i = 0; i < vecObjects.size(); ++i) {
                        CHECK(lst.at(i) == vecObjects[i]);
                    }
                }

                SUBCASE("Random pos insertion") {
                    lst.clear();
                    // Insert some initial data
                    for (const auto &i: vecObjects) {
                        lst.insert(i);
                    }
                    constexpr size_t uNumInsertions = 1000;
                    for (size_t i = 0; i < uNumInsertions; ++i) {
                        size_t uIndex = Generator() % lst.size();
                        auto Value = GetRandom<t_tTestType>();

                        if (uIndex == 0) {
                            // We already checked for insertions in the beginning
                            uIndex += 1;
                        }

                        // I assume the std::vector is correct
                        // So we compare our implementation with the std::vector
                        vecObjects.insert(vecObjects.begin() + uIndex, Value);
                        lst.insert(uIndex, Value);

                        for (size_t j = 0; j < vecObjects.size(); ++j) {
                            CHECK(lst.at(j) == vecObjects[j]);
                        }
                    }
                }
            }
            SUBCASE("Move insertion") {
                // @todo
            }
            SUBCASE("Inplace build insertion") {
                // @todo
            }
        }
        SUBCASE("Access") {
            // Already tested with previous test cases
        }
        SUBCASE("Reassign values") {}
        SUBCASE("Remove") {}
    }

    TEST_CASE("Iterator") {
        SUBCASE("Forward") {}
    }
}
