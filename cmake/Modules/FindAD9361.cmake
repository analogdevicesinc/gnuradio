find_library(AD9361_LIBRARIES ad9361)
find_path(AD9361_INCLUDE_DIRS ad9361.h)
if(AD9361_LIBRARIES)
    set(LIBAD9361_FOUND TRUE)
endif(AD9361_LIBRARIES)
