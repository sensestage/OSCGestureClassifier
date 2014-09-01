message( "=== GestureClassification ===" )

include_directories(
  ../utils/
  ../lbimproved/
  ../YIN/
)

set(gestureclassification_SRCS
  DtwPsClassifier.cpp
  RepClassifier.cpp
  Template.cpp
)


set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )

add_library( GestureClassification STATIC ${gestureclassification_SRCS} )

target_link_libraries(GestureClassification YIN)


message( "=== OSC GestureClassification ===" )

include(FindPkgConfig)
pkg_check_modules(liblo REQUIRED liblo)


include_directories(
  ${CMAKE_BINARY_DIR}
  ${liblo_INCLUDE_DIRS}
  ../YIN/
  ../lbimproved/
  ../utils/
  .
#   ../GestureClassification/
)

set(oscGestureclassification_SRCS
  OSCGestureClassifier.cpp
)

add_executable( OSCGestureClassifier ${oscGestureclassification_SRCS} )

target_link_libraries(OSCGestureClassifier ${liblo_LIBRARIES} GestureClassification)

install(TARGETS OSCGestureClassifier DESTINATION bin)
# main.cpp is a test program
