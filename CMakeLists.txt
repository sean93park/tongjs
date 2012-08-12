CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(bravo)

SET ( TARGET_NAME "bravo" )

SET ( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" )
SET ( CMAKE_CXX_FLAGS "-g -Wall" )

SET ( TARGET_INCLUDE_DIRS "" )
SET ( TARGET_LIBRARY_DIRS "" )
SET ( TARGET_LIBRARY_FILES "" )

SET ( SOURCES 
		# TARGET_NAME
		source/tong.cpp
		source/tongsysapi.cpp
		source/tongsdl.cpp
		source/tongsdlhelper.cpp
		source/tongdebug.cpp
		#
		source/tong.h
		source/tongversion.h
		source/tongsdl.h
		source/tongsdlhelper.h
		source/tongdebug.h
	)

ADD_EXECUTABLE( ${TARGET_NAME} ${SOURCES} )


#
FIND_PACKAGE ( SDL )

# for SDL_Image
SET ( SDL_image_INCLUDE_DIR "/usr/include/SDL" )
SET ( SDL_image_LIB_DIR "/usr/lib" )
SET ( SDL_image_LIBRARIES "-lSDL_image" )
# for SDL_ttf
SET ( SDL_ttf_INCLUDE_DIR "/usr/include/SDL" )
SET ( SDL_ttf_LIB_DIR "/usr/lib" )
SET ( SDL_ttf_LIBRARIES "-lSDL_ttf" )


# for V8, have to change to some flexble way
SET ( V8_INCLUDE_DIR "/usr/include" )
SET ( V8_LIB_DIR "/usr/lib" )
SET ( V8_LIBRARIES "-lv8" )


# http://stackoverflow.com/questions/697560/how-to-copy-directory-from-source-tree-to-binary-tree
function (cm_export_file FILE DEST)
  if (NOT TARGET export-files)
    add_custom_target(export-files ALL COMMENT "Exporting files into build tree")
  endif (NOT TARGET export-files)
  
  get_filename_component(FILENAME "${FILE}" NAME)
  add_custom_command(TARGET export-files COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/${FILE}" "${CMAKE_CURRENT_BINARY_DIR}/${DEST}/${FILENAME}")
  install(FILES "${FILE}" DESTINATION "${DEST}")
endfunction (cm_export_file)

#########################################################################################
# portion from WebKit 'WebKitFeatures.cmake' file
# 
SET (DEFAULT 0)
SET (DEPENDS 1)
SET (ALWAYS 2)
SET (TONGJS_FEATURES "")

MACRO (TONGJS_SEPARATE_FEATURES _name _value)
        FOREACH(f ${ARGN})
                LIST(APPEND ${f}_FEATURES "${_name}=${_value}")
        ENDFOREACH()
ENDMACRO (TONGJS_SEPARATE_FEATURES)

MACRO (TONGJS_FEATURE_CHECK_DEPS _deps _deps_ok)
        SET(${_deps_ok} TRUE)

	LIST(LENGTH ${_deps} ${_deps_ok}_LIST_SIZE)
	IF (${${_deps_ok}_LIST_SIZE} GREATER 1)
		FOREACH(_dep ${${_deps}})
			IF (NOT ${_dep})
				SET(${_deps_ok} FALSE)
				BREAK()
			ENDIF ()
		ENDFOREACH ()
	ELSE ()
		IF (NOT ${_deps})
			SET(${_deps_ok} FALSE)
		ENDIF ()
	ENDIF ()
ENDMACRO()

MACRO (TONGJS_FEATURE _feature _description _type _args)
	IF (${_type} EQUAL ${DEPENDS})
                TONGJS_FEATURE_CHECK_DEPS(${_args} DEPS_OK_${_feature})
		OPTION(${_feature} "${_description}" DEPS_OK_${_feature})
	ELSEIF (${_type} EQUAL ${DEFAULT})
		OPTION(${_feature} "${_description}" ${_args})
	ELSEIF (${_type} EQUAL ${ALWAYS})
		SET(${_feature} ${_args})
	ENDIF ()

	LIST(APPEND TONGJS_FEATURES ${_feature})

	IF (${_feature})
		SET(${_feature} 1)
		TONGJS_SEPARATE_FEATURES(${_feature} 1 ${ARGN})
		LIST(APPEND FEATURE_DEFINES ${_feature})
	ELSE ()
		SET(${_feature} 0)
		TONGJS_SEPARATE_FEATURES(${_feature} 0 ${ARGN})
	ENDIF ()
ENDMACRO()

#
#########################################################################################
# cmake configure options

TONGJS_FEATURE(ENABLE_DEBUG "Enable debug messages" DEFAULT ON)
TONGJS_FEATURE(ENABLE_SCRIPTMSG "Enable script print messages" DEFAULT ON)

#########################################################################################
#

# for debug
IF (ENABLE_DEBUG)
	ADD_DEFINITIONS(-DTONG_DEBUG=1)
	SET(TONG_DEBUG 1)
ENDIF ()

IF (ENABLE_SCRIPTMSG)
	ADD_DEFINITIONS(-DTONG_SCRIPTMSG=1)
	SET(TONG_SCRIPTMSG 1)
ENDIF ()

#
LIST ( APPEND TARGET_INCLUDE_DIRS
	${SDL_INCLUDE_DIRS} 
	${V8_INCLUDE_DIR}
	${SDL_image_INCLUDE_DIR}
	${SDL_ttf_INCLUDE_DIR}
)

LIST ( APPEND TARGET_LIBRARY_DIRS 
	${SDL_LIBRARY_DIRS} 
	${V8_LIB_DIR}
	${SDL_image_LIB_DIR}
	${SDL_ttf_LIB_DIR}
)

LIST ( APPEND TARGET_LIBRARY_FILES
	${SDL_LIBRARIES} 
	${V8_LIBRARIES}
	${SDL_image_LIBRARIES}
	${SDL_ttf_LIBRARIES}
)

#
INCLUDE_DIRECTORIES 	( ${TARGET_INCLUDE_DIRS} )
LINK_DIRECTORIES 		( ${TARGET_LIBRARY_DIRS} )

TARGET_LINK_LIBRARIES 	( ${TARGET_NAME} ${TARGET_LIBRARY_FILES} )

cm_export_file( "source/tong.js" "." )
cm_export_file( "source/tongsdl.js" "." )
cm_export_file( "test/tuxhalf.png" "." )
cm_export_file( "test/2012_moon.png" "." )
