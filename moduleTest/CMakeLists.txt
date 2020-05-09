set(moduleTest moduleTest)

set(moduleTest_SRCS
    main.cpp)

include_directories("../ThirdParty/Catch" "../ThirdParty/FakeIt")


set(MODULES_GUI FALSE)
set(MODULES_CLI FALSE)
set(MODULES_CORE FALSE)
set(MODULES_CORE_TEST TRUE)
add_subdirectory(../modules modules)

add_executable(${moduleTest} ${moduleTest_SRCS})
target_link_libraries(${moduleTest} salalib genlib mgraph440 ${moduletest_modules})
