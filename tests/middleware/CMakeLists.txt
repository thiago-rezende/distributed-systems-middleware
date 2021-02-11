set(MIDDLEWARE_TESTS_HEADERS
    ""
)

set(MIDDLEWARE_TESTS_SOURCES
    "never_fails.cpp"
)

add_executable(middleware_tests
    ${MIDDLEWARE_TESTS_SOURCES}
    ${MIDDLEWARE_TESTS_HEADERS}
)

add_test(NAME middleware_tests COMMAND middleware_tests)

target_link_libraries(middleware_tests PUBLIC Catch2WithMain)
