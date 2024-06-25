
# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.

# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)

# Add your static library (assuming it's in the same directory).
add_library(mylibrary STATIC IMPORTED)
set_target_properties(mylibrary PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/libDTSHeadphoneXMobileSDK.a)

# Add your native source files.
add_library(native-lib SHARED ${CMAKE_SOURCE_DIR}/DTSHeadphoneXMobileInterface.cpp)

# Specify the libraries CMake should link against your native library.
target_link_libraries(native-lib mylibrary log)