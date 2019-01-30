# Terminal Color Codes
BLACK="\033[0;30m"
BLUE="\033[0;34m"
GREEN="\033[0;32m"
GREY="\033[01;30m"
CYAN="\033[0;36m"
RED="\033[01;31m"
PURPLE="\033[0;35m"
BROWN="\033[0;33m"
LGREY="\033[0;37m"
YELLOW="\033[0;33m"
WHITE="\033[0;37m"
NORMAL="\033[00m"

# Expect this script to be in the test subdirectory
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
pushd "${DIR}"


if [ ! -d build ]; then
	mkdir build
fi

pushd build
echo -e "\n\n${YELLOW}Running CMake.${NORMAL}"
cmake_opt=""
if test "$(uname)" == "Darwin"; then
    PATH="~/Applications/CMake.app/Contents/bin:${PATH}"
    cmake_opt="$cmake_opt -G 'Unix Makefiles'"
fi
eval "cmake $cmake_opt .."
if [ $? -ne 0 ]; then
    echo -e "${RED}CMake did not run correctly. Exiting.${NORMAL}"
    popd
    popd
    exit 1
fi

# For some reason the makefile is placed up one directory in some systems
if [ -a ../Makefile ]; then
    popd
fi

echo -e "\n\n${YELLOW}Running make.${NORMAL}"
cmake --build . --clean-first
if [ $? -ne 0 ]; then
    echo -e "${RED}make did not run correctly. Exiting.${NORMAL}"
    popd
    popd
    exit 1
fi

# This is to handle the case where the makefile was placed up one directory
if [ -d build ]; then
    pushd build
fi

pushd bin
echo -e "\n\n${YELLOW}Running Tests.${NORMAL}"
failedTests=0;

./tests
ec=$?
echo -e "\n"
if [ $ec -ne 0 ]; then
    echo -e "${PURPLE}$ec tests failed.${NORMAL}\n\n\n"
fi
failedTests=$(($failedTests+$ec));

echo -e "\n\n${YELLOW}${failedTests} tests failed.${NORMAL}"
popd
popd

exit $failedTests
