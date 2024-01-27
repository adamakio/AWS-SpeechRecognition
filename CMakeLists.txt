# CMakeList.txt : CMake project for AWS-SpeechRecognition, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_TOOLCHAIN_FILE "C:/Users/16478/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

project ("AWS-SpeechRecognition")

# Find packages
find_package(portaudio CONFIG REQUIRED)
find_package(CURL CONFIG REQUIRED)
# find_package(aws-cpp-sdk-core REQUIRED COMPONENTS transcribestreaming )
find_package(AWSSDK CONFIG COMPONENTS core dynamodb kinesis s3 transcribestreaming REQUIRED)

# Add source to this project's executable.
add_executable (AWS-SpeechRecognition "AWS-SpeechRecognition.cpp" "AWS-SpeechRecognition.h" "audio-capture.cpp")

# Link libraries to your executable
target_link_libraries(AWS-SpeechRecognition PRIVATE portaudio)
target_link_libraries(AWS-SpeechRecognition PRIVATE CURL::libcurl)
target_include_directories(AWS-SpeechRecognition PRIVATE ${AWSSDK_INCLUDE_DIRS})
target_link_libraries(AWS-SpeechRecognition PRIVATE ${AWSSDK_LINK_LIBRARIES})


