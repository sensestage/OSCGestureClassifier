message( "=== lbimproved ===" )

include_directories(
  ../utils/
)

set(lbimproved_SRCS
)

add_library( lbimproved STATIC ${lbimproved_SRCS} )
