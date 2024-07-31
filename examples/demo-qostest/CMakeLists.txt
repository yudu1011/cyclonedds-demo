cmake_minimum_required(VERSION 3.16)
project(demo-localcom LANGUAGES C)

if(NOT TARGET CycloneDDS::ddsc)
  # Find the CycloneDDS package.
  find_package(CycloneDDS REQUIRED)
endif()

idlc_generate(TARGET TestData_lib FILES "TestData.idl" WARNINGS no-implicit-extensibility)

add_executable(Pub-QoSTest pub.c)
add_executable(Sub-QoStest sub.c)

target_link_libraries(Pub-QoSTest TestData_lib CycloneDDS::ddsc)
target_link_libraries(Sub-QoStest TestData_lib CycloneDDS::ddsc)


