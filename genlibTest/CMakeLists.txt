set(genlibtest genlibTest)
set(genlibTest_SRCS
    testreadwritehelpers.cpp
    main.cpp
    testsimplematrix.cpp
    testbspnode.cpp
    teststringutils.cpp
    testcontainerutils.cpp)

set(LINK_LIBS
    genlib)

include_directories("../ThirdParty/Catch")

add_executable(${genlibtest} ${genlibTest_SRCS})

target_compile_options(${genlibtest} PRIVATE ${COMPILE_WARNINGS})

target_link_libraries(${genlibtest} ${LINK_LIBS})
    
