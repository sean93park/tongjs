# - Try to find SDL
# Once done, this will define
#
#  SDL_FOUND - system has SDL
#  SDL_INCLUDE_DIRS - the SDL include directories
#  SDL_LIBRARIES - link these to use SDL

include(LibFindMacros)

# Dependencies
# libfind_package(SDL Freetype)

# Use pkg-config to get hints about paths
set (Module sdl)
if (SDL_FIND_VERSION)
  set (Module "${Module}>=${SDL_FIND_VERSION}")
endif ()

libfind_pkg_check_modules(SDL_PKGCONF ${Module})

# Include dir
find_path(SDL_INCLUDE_DIR
  NAMES SDL.h
  PATHS ${SDL_PKGCONF_INCLUDE_DIRS}
)

# Finally the library itself
find_library(SDL_LIBRARY
  NAMES SDL
  PATHS ${SDL_PKGCONF_LIBRARY_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(SDL_PROCESS_INCLUDES SDL_INCLUDE_DIR)
set(SDL_PROCESS_LIBS SDL_LIBRARY)
libfind_process(SDL)

