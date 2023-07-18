set(CTEST_CUSTOM_MAXIMUM_NUMBER_OF_WARNINGS "2000")
set(CTEST_CUSTOM_MAXIMUM_NUMBER_OF_ERRORS   "2000")
list(APPEND CTEST_CUSTOM_ERROR_EXCEPTION    )


file (STRINGS "@CMAKE_BINARY_DIR@/CTestTestfile.cmake" LINES)

# overwrite the file....
file(WRITE "@CMAKE_BINARY_DIR@/CTestTestfile.cmake" "")

# loop through the lines,
foreach(LINE IN LISTS LINES)
    # remove unwanted parts
    string(REGEX REPLACE ".*eigen3.*" "" STRIPPED "${LINE}")
    # and write the (changed) line ...
    file(APPEND "@CMAKE_BINARY_DIR@/CTestTestfile.cmake" "${STRIPPED}\n")
endforeach()