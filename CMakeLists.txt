project(LolCodeInterpreter)

cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 17)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

set(
    HEADERS
    ${LolCodeInterpreter_SOURCE_DIR}/driver.hh
    ${LolCodeInterpreter_SOURCE_DIR}/parser.hh
)

BISON_TARGET(
    LolParser
    parser.y
    ${LolCodeInterpreter_SOURCE_DIR}/lolparser.cc
    DEFINES_FILE ${LolCodeInterpreter_SOURCE_DIR}/lolparser.hh
)

FLEX_TARGET(
    LolScanner
    scanner.l
    ${LolCodeInterpreter_SOURCE_DIR}/lolscanner.cc
)

ADD_FLEX_BISON_DEPENDENCY(LolScanner LolParser)

add_executable(
    LolCodeInterpreter
    main.cc
    driver.cc
    scanner.cc
    ${BISON_LolParser_OUTPUTS}
    ${FLEX_LolScanner_OUTPUTS}
)