# main file for src

if(UNIT_TESTS)

include_directories(${PROJECT_SOURCE_DIR}/src "." ${GTEST_INCLUDE_DIRS})
include_directories(${HEADER_RAPIDJSON})


## alibxx


file(GLOB src_alibxx alibxx/*.cpp)
file(GLOB src_neon neon/*.cpp)
file(GLOB src_datetime datetime/*.cpp)
file(GLOB src_credential credential/*.cpp)
file(GLOB src_context context/*.cpp)
file(GLOB src_session session/*.cpp)
file(GLOB src_parser xmlparser/*.cpp)
file(GLOB src_files files/*.cpp)
file(GLOB src_auth  auth/*.cpp)
LIST(APPEND src_utils utils/utils_test.cpp ${SRC_STRING_UTILS_CPP})
LIST(APPEND src_parser parser/parser_test.cpp)
LIST(APPEND src_httprequest sessionfactory/test_factory.cpp)


add_definitions( -DTEST_VALID_CERT="${TEST_CRED_PATH}" -DTEST_VALID_CERT_PASS="${TEST_CRED_PASS}")
add_executable(g_test_unit_davix_exe ${src_context} ${src_session} ${src_credential}
                ${src_httprequest} ${src_datetime} ${src_neon} ${src_parser} ${src_auth} ${src_utils} ${src_parser} ${SRC_BASE64}
                ${SRC_HMACSHA1} ${davix_tool_common_src} ${src_files} ${src_alibxx})

target_link_libraries(g_test_unit_davix_exe alibxx_core davix  gtest gtest_main ${CMAKE_THREAD_LIBS_INIT} ${LIBSSL_PKG_LIBRARIES} ${Boost_LIBRARIES})

if(HAVE_CLOCK_GETTIME)
    target_link_libraries(g_test_unit_davix_exe rt)
endif(HAVE_CLOCK_GETTIME)

add_dependencies(g_test_unit_davix_exe libdavix)



add_test(g_test_unit_davix g_test_unit_davix_exe )

endif(UNIT_TESTS)
