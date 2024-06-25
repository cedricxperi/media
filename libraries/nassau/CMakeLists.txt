# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             nassau-lib
             # Sets the library as a shared library.
             SHARED
             # Provides a relative path to your source file(s).
             src/main/cpp/nassau/dts-nassau-wrapper.cpp)

add_library( # Sets the name of the library.
             libnassau-shared
             # Sets the library as a shared library.
             SHARED
             IMPORTED )

add_library( # Sets the name of the library.
              libc++_shared
              # Sets the library as a shared library.
              SHARED
              IMPORTED )

set_target_properties( # Specifies the target library.
                        libc++_shared
                        # Specifies the parameter you want to define.
                        PROPERTIES IMPORTED_LOCATION
                        # Provides the path to the library you want to import.
                        ${CMAKE_CURRENT_SOURCE_DIR}/lib/v8/libc++_shared.so)

set_target_properties( # Specifies the target library.
                       libnassau-shared
                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION
                       # Provides the path to the library you want to import.
                       ${CMAKE_CURRENT_SOURCE_DIR}/lib/v8/libnassau-shared.so)

include_directories( cpp/)


# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib
              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        nassau-lib
        libnassau-shared
        libc++_shared
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib} )

set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-shared,-Bsymbolic" )