message( "=== YIN ===" )

include_directories(
  ../utils/
)
add_library( YIN STATIC YIN.cpp )

target_link_libraries(YIN GestureClassifierUtils)
