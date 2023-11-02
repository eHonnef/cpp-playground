/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#define ANKERL_NANOBENCH_IMPLEMENT

#include <Containers/List.hpp>
#include <nanobench/nanobench.h>
#include <doctest/doctest.h>


TEST_SUITE("") {
    class CBenchmark {
    public:
        CBenchmark(const std::string &strTitle) : m_Benchmark{} {
            m_Benchmark.relative(true);
            m_Benchmark.title(strTitle);
        }

        ankerl::nanobench::Bench &operator()() {
            return m_Benchmark;
        }

    private:
        ankerl::nanobench::Bench m_Benchmark;
    };

    TEST_CASE_TEMPLATE("List benchmark", t_tTestType, uint32_t, int64_t, float, double) {
        /**
         * The idea is to compare the performance of std::vector to the List implementation with different types.
         */

        std::vector<t_tTestType> lstVector;
        eho::CList<t_tTestType, true> myLst;

        SUBCASE("Populate") {
            CBenchmark BPopulate{std::string("Populate: ") + typeid(t_tTestType).name()};
            BPopulate().run("std::vector: Populate", [&]() {
//                ankerl::nanobench::doNotOptimizeAway([&]() {
                for (size_t i = 0; i < 10000; ++i) {
                    if constexpr (std::is_same_v<t_tTestType, std::string>)
                        lstVector.emplace_back("127893498ajkshdjkvxcjkb__89iow37e");
                    else
                        lstVector.emplace_back(1234);
                }
//                });
            });
            BPopulate().run("eho::CList: Populate", [&]() {
//                ankerl::nanobench::doNotOptimizeAway([&]() {
                for (size_t i = 0; i < 10000; ++i) {
                    if constexpr (std::is_same_v<t_tTestType, std::string>)
                        myLst.insert("127893498ajkshdjkvxcjkb__89iow37e");
                    else
                        myLst.insert(1234);
                }
//                });
            });
            // Random position insertion
        }

        SUBCASE("Iterate") {
            CBenchmark BIterate{std::string("Iterate: ") + typeid(t_tTestType).name()};
            std::random_device RandomDevice;
            std::mt19937 Generator{RandomDevice()};

            for (size_t i = 0; i < 10000; ++i) {
                if constexpr (std::is_same_v<t_tTestType, std::string>) {
                    myLst.insert("127893498ajkshdjkvxcjkb__89iow37e");
                    lstVector.emplace_back("127893498ajkshdjkvxcjkb__89iow37e");
                } else {
                    myLst.insert(Generator());
                    lstVector.emplace_back(Generator());
                }
            }

            BIterate().minEpochIterations(1000).run("std::vector: Iterate over", [&]() {
                for (size_t i = 0; i < lstVector.size(); ++i) {
                    [[maybe_unused]] t_tTestType &ref = lstVector.at(i);
                }
                for (size_t i = 0; i < lstVector.size(); ++i) {
                    [[maybe_unused]] t_tTestType &ref = lstVector[i];
                }
                for (auto it = lstVector.begin(); it != lstVector.end(); ++it) {
                    [[maybe_unused]] t_tTestType &ref = *it;
                }
                for (auto &Elem: lstVector) {
                    [[maybe_unused]] t_tTestType &ref = Elem;
                }
            });

            BIterate().minEpochIterations(1000).run("eho::CList: Iterate over", [&]() {
                for (size_t i = 0; i < myLst.size(); ++i) {
                    [[maybe_unused]] t_tTestType &ref = myLst.at(i);
                }
                for (size_t i = 0; i < myLst.size(); ++i) {
                    [[maybe_unused]] t_tTestType &ref = myLst[i];
                }
                for (auto it = myLst.begin(); it != myLst.end(); ++it) {
                    [[maybe_unused]] t_tTestType &ref = *it;
                }
                for (auto &Elem: myLst) {
                    [[maybe_unused]] t_tTestType &ref = Elem;
                }
            });

            // random access
        }

        SUBCASE("stdlib Algorithms") {
            CBenchmark BShuffle{std::string("Algorithm shuffle: ") + typeid(t_tTestType).name()};
            CBenchmark BStableSort{std::string("Algorithm stable sort: ") + typeid(t_tTestType).name()};
            CBenchmark BShuffleAndSort{std::string("Algorithm shuffle + stable sort: ") + typeid(t_tTestType).name()};

            std::random_device RandomDevice;
            std::mt19937 Generator{RandomDevice()};

            for (size_t i = 0; i < 10000; ++i) {
                if constexpr (std::is_same_v<t_tTestType, std::string>) {
                    myLst.insert("127893498ajkshdjkvxcjkb__89iow37e");
                    lstVector.emplace_back("127893498ajkshdjkvxcjkb__89iow37e");
                } else {
                    myLst.insert(Generator());
                    lstVector.emplace_back(Generator());
                }
            }

            BShuffle().minEpochIterations(1000).run("eho::CList: shuffle", [&]() {
                std::ranges::shuffle(myLst, Generator);
            });

            BShuffle().minEpochIterations(1000).run("std::vector: shuffle", [&]() {
                std::ranges::shuffle(lstVector, Generator);
            });

            BStableSort().run("eho::CList: sort", [&]() {
                std::ranges::stable_sort(myLst);
            });

            BStableSort().run("std::vector: sort", [&]() {
                std::ranges::stable_sort(lstVector);
            });

            BShuffleAndSort().minEpochIterations(5000).run("eho::CList: sort", [&]() {
                std::ranges::shuffle(myLst, Generator);
                std::ranges::stable_sort(myLst);
            });

            BShuffleAndSort().minEpochIterations(5000).run("std::vector: sort", [&]() {
                std::ranges::shuffle(lstVector, Generator);
                std::ranges::stable_sort(lstVector);
            });
        }

        // deletion
    }
}
