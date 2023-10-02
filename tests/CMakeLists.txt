add_executable(Tests vector3_test.cpp)

find_package(Catch2 CONFIG REQUIRED)

target_link_libraries(Tests PRIVATE EnableAllWarningsAsErrors EnableAsanIfDebug Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(Tests)
