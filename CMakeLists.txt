cmake_minimum_required(VERSION 2.8)

project(ligo C)

set(INSTALL_BIN_DIR "${CMAKE_INSTALL_PREFIX}/bin" CACHE PATH "Installation directory for executables")
set(INSTALL_LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib" CACHE PATH "Installation directory for libraries")
set(INSTALL_INC_DIR "${CMAKE_INSTALL_PREFIX}/include" CACHE PATH "Installation directory for headers")
set(INSTALL_PKGCONFIG_DIR "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" CACHE PATH "Installation directory for pkgconfig (.pc) files")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -ggdb")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBUSB REQUIRED libusb-1.0 )

find_package(Doxygen)
if(DOXYGEN_FOUND)

  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

  add_custom_target(doc
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM
    )

endif()

set(LIGO_HDRS
  ligo.h)

set(LIGO_SRCS
  ligo.c)

file(READ ${CMAKE_CURRENT_SOURCE_DIR}/ligo.h _ligo_h_content)
string(REGEX REPLACE ".*#define[ \t]+LIGO_VERSION[ \t]+\"([-0-9A-Za-z.]+)\".*"
  "\\1" LIGO_VERSION ${_ligo_h_content})

set(LIGO_PC ${CMAKE_CURRENT_BINARY_DIR}/ligo.pc)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/ligo.pc.cmakein
  ${LIGO_PC} @ONLY)

add_library(ligo SHARED ${LIGO_SRCS} ${LIGO_HDRS})

set_target_properties(ligo PROPERTIES
  SOVERSION ${LIGO_VERSION}
  VERSION   ${LIGO_VERSION}
  )

if(LIBUSB_FOUND)
  include_directories(${LIBUSB_INCLUDE_DIRS})
  target_link_libraries(ligo ${LIBUSB_LIBRARIES})
endif()


if(NOT SKIP_INSTALL_LIBRARIES AND NOT SKIP_INSTALL_ALL )
    install(TARGETS ligo
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}"
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}" )
endif()
if(NOT SKIP_INSTALL_HEADERS AND NOT SKIP_INSTALL_ALL )
    install(FILES ${LIGO_HDRS} DESTINATION "${INSTALL_INC_DIR}")
endif()
# if(NOT SKIP_INSTALL_FILES AND NOT SKIP_INSTALL_ALL )
#     install(FILES zlib.3 DESTINATION "${INSTALL_MAN_DIR}/man3")
# endif()
if(NOT SKIP_INSTALL_FILES AND NOT SKIP_INSTALL_ALL )
    install(FILES ${LIGO_PC} DESTINATION "${INSTALL_PKGCONFIG_DIR}")
endif()
