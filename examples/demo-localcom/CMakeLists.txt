cmake_minimum_required(VERSION 3.16)
project(demo-localcom LANGUAGES C)

if(NOT TARGET CycloneDDS::ddsc)
  # Find the CycloneDDS package.
  find_package(CycloneDDS REQUIRED)
endif()

idlc_generate(TARGET DemoData_lib FILES "DemoData.idl" WARNINGS no-implicit-extensibility)

add_executable(Demo-Pub pub.c)
add_executable(Demo-Sub sub.c)

target_link_libraries(Demo-Pub DemoData_lib CycloneDDS::ddsc)
target_link_libraries(Demo-Sub DemoData_lib CycloneDDS::ddsc)


