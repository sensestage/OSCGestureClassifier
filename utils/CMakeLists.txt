message( "=== utils ===" )

# include_directories(
# )

set(GestureClassifierUtils_SRCS
  AlphaFilter.cpp
  HistogramBuffer.cpp
  utils.cpp
)


add_library( GestureClassifierUtils STATIC ${GestureClassifierUtils_SRCS} )
