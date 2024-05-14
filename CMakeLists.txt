cmake_minimum_required(VERSION 3.16)
project(ListfileDumper)

include(GNUInstallDirs)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)
set(INCLUDEDIR ${PROJECT_SOURCE_DIR}/include)


# c++20 need for using enum and enum class
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the CERN root components RIO (for file IO) and Tree (TTree/TNTuple)
find_package(ROOT REQUIRED COMPONENTS RIO Tree)

# Find the libzip library
# https://libzip.org/
# Need libzip development package
# Debian/Ubuntu: sudo apt install libzip-dev
find_library(LIBZIP_LIBRARY NAMES zip)

message(STATUS "Project source directory: ${PROJECT_SOURCE_DIR}")
message(STATUS "Current source directory: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "Project binary directory: ${CMAKE_INSTALL_LIBDIR}")
message(STATUS "Project library output directory: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

message(STATUS "Project include directory: ${INCLUDE_DIR}")




file(COPY "${PROJECT_SOURCE_DIR}/initDUMPER.sh" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

include_directories(${INCLUDEDIR} ${ROOT_INCLUDE_DIRS})
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.h)
file(GLOB sources ${PROJECT_SOURCE_DIR}/source/*.cpp)

add_compile_options(-Os -Wall -Wextra -pedantic)

add_library(listfile SHARED ${PROJECT_SOURCE_DIR}/source/listfile.cpp)
add_library(MVMEmodule SHARED ${PROJECT_SOURCE_DIR}/source/MVMEmodule.cpp)
add_library(modules SHARED ${PROJECT_SOURCE_DIR}/source/modules.cpp)
#add_library(modules_dict SHARED ${CMAKE_BINARY_DIR}/modules_dict.cxx)



add_executable(mvme-listfile-dumper listfile-dumper.cpp ${sources} ${headers})
target_link_libraries(mvme-listfile-dumper PUBLIC ROOT::RIO ROOT::Tree ${LIBZIP_LIBRARY} listfile MVMEmodule modules)

target_include_directories(mvme-listfile-dumper PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR})
ROOT_GENERATE_DICTIONARY(modules_dict ${INCLUDEDIR}/modules.h LINKDEF ${INCLUDEDIR}/linkdef.h MODULE modules)

#install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} DESTINATION ${CMAKE_INSTALL_BINDIR})
#execute_process(COMMAND /bin/chmod ARGS 0750 "${CMAKE_INSTALL_BINDIR}/mvme-listfile-dumper")
#file(CHMOD "${CMAKE_INSTALL_BINDIR}/mvme-listfile-dumper" PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ)
install(TARGETS mvme-listfile-dumper DESTINATION ${CMAKE_INSTALL_BINDIR} PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ)
install(FILES "${PROJECT_SOURCE_DIR}/initDUMPER.sh" DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} DESTINATION ${CMAKE_INSTALL_LIBDIR} FILES_MATCHING PATTERN "*.so" PATTERN "*.pcm")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/templates DESTINATION ${CMAKE_INSTALL_DATADIR})
install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})



