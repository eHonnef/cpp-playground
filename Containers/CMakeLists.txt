cmake_minimum_required(VERSION 3.2)
project(eho_Containers
        LANGUAGES CXX
        VERSION 0.0.1
)

if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(CMAKE_GENERATOR "Unix Makefiles" CACHE INTERNAL "" FORCE)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    add_compile_options(-Wall -Wpedantic -Werror)
    add_compile_options(-fconcepts-diagnostics-depth=50)
    add_compile_options(-fsanitize=leak)
    set(${CMAKE_BINARY_DIR} "build")

    set(3rdParty_dir ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty)
    set(doctest_dir ${3rdParty_dir}/doctest)
    set(nanobench_dir ${3rdParty_dir}/nanobench)

    set(headers_dir ${CMAKE_CURRENT_LIST_DIR}/code/include)
    set(src_dir ${CMAKE_CURRENT_LIST_DIR}/code/src)

    include_directories(${headers_dir} ${3rdParty_dir})

    # Unit testing executable
    set(unit_test_bin "${PROJECT_NAME}_unit_test")
    file(GLOB_RECURSE tests_src_files CONFIGURE_DEPENDS ${src_dir}/Tests/*.cpp)
    add_executable(${unit_test_bin} ${tests_src_files})

    # Benchmarks executable
    set(benchmarks_bin "${PROJECT_NAME}_benchmarks")
    file(GLOB_RECURSE benchmarks_src_files CONFIGURE_DEPENDS ${src_dir}/Benchmarks/*.cpp)
    add_executable(${benchmarks_bin} ${benchmarks_src_files})
else ()
    add_library(${PROJECT_NAME} INTERFACE)
    target_compile_options(${PROJECT_NAME} INTERFACE -w)
    target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR}/code/include)
endif ()
