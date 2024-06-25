add_library(kvd_plan OBJECT
  bucket.cpp
  logical_plan.cpp
  ops.cpp
  text_plan.cpp
)

if(NOT KVD_DISABLE_TESTS)
  find_package(GTest REQUIRED)

  add_executable(kvd-test
    bucket-test.cc
    logical_plan-test.cc
    ops-test.cc
    pooling-test.cc
    text_plan-test.cc
  )

  target_link_libraries(kvd-test GTest::gtest_main kvd_plan)
  enable_testing()
  gtest_discover_tests(kvd-test)
endif()
