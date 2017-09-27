
message("CTEST_BUILD_OPTIONS = ${CTEST_BUILD_OPTIONS}")

# project and repository to clone from
SET(CTEST_PROJECT_NAME "Embree")
SET(TEST_REPOSITORY "https://github.com/embree/embree.git")

# set directories
set(CTEST_SOURCE_DIRECTORY "${CTEST_SCRIPT_DIRECTORY}/..")
set(CTEST_BINARY_DIRECTORY "${CTEST_SCRIPT_DIRECTORY}/../build")
SET(TEST_MODELS_PARENT_DIRECTORY $ENV{HOME})
SET(TEST_MODELS_DIRECTORY ${TEST_MODELS_PARENT_DIRECTORY}/embree-models)

# fix slashes under windows
IF (WIN32)
  STRING(REPLACE "\\" "/" CTEST_SOURCE_DIRECTORY "${CTEST_SOURCE_DIRECTORY}")
  STRING(REPLACE "\\" "/" CTEST_BINARY_DIRECTORY "${CTEST_BINARY_DIRECTORY}")
  STRING(REPLACE "\\" "/" TEST_MODELS_PARENT_DIRECTORY "${TEST_MODELS_PARENT_DIRECTORY}")
  STRING(REPLACE "\\" "/" TEST_MODELS_DIRECTORY "${TEST_MODELS_DIRECTORY}")
ENDIF()

MESSAGE("CTEST_SOURCE_DIRECTORY = ${CTEST_SOURCE_DIRECTORY}")
MESSAGE("CTEST_BINARY_DIRECTORY = ${CTEST_BINARY_DIRECTORY}")
MESSAGE("TEST_MODELS_DIRECTORY = ${TEST_MODELS_DIRECTORY}")
MESSAGE("http_proxy = $ENV{http_proxy}")
MESSAGE("https_proxy = $ENV{https_proxy}")
MESSAGE("no_proxy = $ENV{no_proxy}")
MESSAGE("PATH = $ENV{PATH}")

# update external model repository
FIND_PROGRAM(CTEST_GIT_COMMAND NAMES git)

MACRO(check_return_code)
  IF (NOT "${retcode}" STREQUAL "0")
    MESSAGE(FATAL_ERROR "error executing process")
  ENDIF()
ENDMACRO()

# macro that updates the test models
MACRO(update_test_models)
  IF(NOT EXISTS "${TEST_MODELS_DIRECTORY}")
    MESSAGE("cloning test models ...")
    EXECUTE_PROCESS(
      COMMAND ${CTEST_GIT_COMMAND} "clone" "git@git.sdvis.org:embree-models" embree-models
      WORKING_DIRECTORY ${TEST_MODELS_PARENT_DIRECTORY}
      RESULT_VARIABLE retcode)
    check_return_code()
  ELSE()
    MESSAGE("updating test models ...")
    EXECUTE_PROCESS(
      COMMAND ${CTEST_GIT_COMMAND} "fetch"
      WORKING_DIRECTORY ${TEST_MODELS_DIRECTORY}
      RESULT_VARIABLE retcode)
    check_return_code()
  ENDIF()
  IF (NOT TEST_MODELS_HASH)
    MESSAGE(FATAL_ERROR "no TEST_MODELS_HASH set")
  ENDIF()
  MESSAGE("checking out test models: ${TEST_MODELS_HASH}")
  EXECUTE_PROCESS(
    COMMAND ${CTEST_GIT_COMMAND} "checkout" ${TEST_MODELS_HASH}
    WORKING_DIRECTORY ${TEST_MODELS_DIRECTORY}
    RESULT_VARIABLE retcode)
  check_return_code()
ENDMACRO()

#EXECUTE_PROCESS(
#  COMMAND "wget" "http://10.123.110.90/CDash"
#  WORKING_DIRECTORY ${TEST_MODELS_DIRECTORY}
#  RESULT_VARIABLE retcode)
#check_return_code()


# increase default output sizes for test outputs
IF (NOT DEFINED CTEST_CUSTOM_MAXIMUM_PASSED_TEST_OUTPUT_SIZE)
  SET(CTEST_CUSTOM_MAXIMUM_PASSED_TEST_OUTPUT_SIZE 100000)
ENDIF()
IF(NOT DEFINED CTEST_CUSTOM_MAXIMUM_FAILED_TEST_OUTPUT_SIZE)
  SET(CTEST_CUSTOM_MAXIMUM_FAILED_TEST_OUTPUT_SIZE 800000)
ENDIF()

# enable testing in Embree
#STRING(REPLACE "\\" "\\\\" TEST_MODELS_DIRECTORY "${TEST_MODELS_DIRECTORY}")
SET (CTEST_BUILD_OPTIONS "${CTEST_BUILD_OPTIONS} -D BUILD_TESTING:BOOL=ON -D EMBREE_TESTING_MODEL_DIR:PATH=${TEST_MODELS_DIRECTORY}")
message("CTEST_BUILD_OPTIONS2 = ${CTEST_BUILD_OPTIONS}")

# set site based on this machine's hostname
SITE_NAME(HOSTNAME)
set(CTEST_SITE "${HOSTNAME}")

# drop location
set(CTEST_DROP_METHOD "http")
IF(NOT CTEST_DROP_SITE)
   set(CTEST_DROP_SITE "10.123.110.90")
ENDIF()
set(CTEST_DROP_LOCATION "/CDash/submit.php?project=${CTEST_PROJECT_NAME}")
set(CTEST_DROP_SITE_CDASH TRUE)

# get OS and CPU information
find_program(UNAME NAMES uname)
macro(getuname name flag)
  exec_program("${UNAME}" ARGS "${flag}" OUTPUT_VARIABLE "${name}")
endmacro(getuname)

getuname(osname -s)
getuname(osrel  -r)
getuname(cpu    -m)

# build using as many processes as we have processors
include(ProcessorCount)
ProcessorCount(numProcessors)
if(numProcessors EQUAL 0)
  SET(numProcessors 1)
endif()

# set build name
set(CTEST_BUILD_NAME "${TEST_NAME}-${osname}-${cpu}")
set(CTEST_CMAKE_GENERATOR "Unix Makefiles")
IF (WIN32)
  set(CTEST_BUILD_COMMAND "${CMAKE_COMMAND} --build . --config ${CTEST_CONFIGURATION_TYPE}")
ELSE()
  set(CTEST_BUILD_COMMAND "make -j ${numProcessors}")
ENDIF()

###################
# execute the test
###################

# requires CMake 2.8 or higher for git!!!
#set(CTEST_UPDATE_COMMAND "${CTEST_GIT_COMMAND}")

set(CTEST_CONFIGURE_COMMAND "${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE:STRING=${CTEST_BUILD_CONFIGURATION}")
set(CTEST_CONFIGURE_COMMAND "${CTEST_CONFIGURE_COMMAND} ${CTEST_BUILD_OPTIONS}")
set(CTEST_CONFIGURE_COMMAND "${CTEST_CONFIGURE_COMMAND} ..")

# start the test
ctest_empty_binary_directory(${CTEST_BINARY_DIRECTORY})
ctest_start("Continuous")
#ctest_update (RETURN_VALUE count)
update_test_models()
ctest_configure()
ctest_build()
#IF (NOT CTEST_SKIP_TESTING)
#  ctest_test()
#ENDIF()
#ctest_submit()
