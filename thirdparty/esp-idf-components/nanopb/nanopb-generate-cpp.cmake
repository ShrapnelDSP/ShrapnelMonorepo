function(NANOPB_GENERATE_CPP SRCS HDRS)
  cmake_parse_arguments(NANOPB_GENERATE_CPP "" "RELPATH" "" ${ARGN})
  if(NOT NANOPB_GENERATE_CPP_UNPARSED_ARGUMENTS)
    return()
  endif()
  set(NANOPB_OPTIONS_DIRS)
  
  if(MSVC)
    set(CUSTOM_COMMAND_PREFIX call)
  endif()

  if(NANOPB_GENERATE_CPP_RELPATH)
	  list(APPEND _nanopb_include_path "-I${NANOPB_GENERATE_CPP_RELPATH}")
	  list(APPEND NANOPB_OPTIONS_DIRS ${NANOPB_GENERATE_CPP_RELPATH})
  endif()

  if(NANOPB_GENERATE_CPP_APPEND_PATH)
    # Create an include path for each file specified
    foreach(FIL ${NANOPB_GENERATE_CPP_UNPARSED_ARGUMENTS})
      get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
      get_filename_component(ABS_PATH ${ABS_FIL} PATH)
      list(APPEND _nanopb_include_path "-I${ABS_PATH}")
    endforeach()
  else()
    list(APPEND _nanopb_include_path "-I${CMAKE_CURRENT_SOURCE_DIR}")
  endif()

  if(DEFINED NANOPB_IMPORT_DIRS)
    foreach(DIR ${NANOPB_IMPORT_DIRS})
      get_filename_component(ABS_PATH ${DIR} ABSOLUTE)
      list(APPEND _nanopb_include_path "-I${ABS_PATH}")
    endforeach()
  endif()

  list(REMOVE_DUPLICATES _nanopb_include_path)

  set(GENERATOR_PATH ${CMAKE_CURRENT_BINARY_DIR}/nanopb/generator)

  set(NANOPB_GENERATOR_EXECUTABLE ${GENERATOR_PATH}/nanopb_generator.py)
  if (CMAKE_HOST_WIN32)
    set(NANOPB_GENERATOR_PLUGIN ${GENERATOR_PATH}/protoc-gen-nanopb.bat)
  else()
    set(NANOPB_GENERATOR_PLUGIN ${GENERATOR_PATH}/protoc-gen-nanopb)
  endif()

  set(GENERATOR_CORE_DIR ${GENERATOR_PATH}/proto)
  set(GENERATOR_CORE_SRC
      ${GENERATOR_CORE_DIR}/nanopb.proto)

  # Treat the source directory as immutable.
  #
  # Copy the generator directory to the build directory before
  # compiling python and proto files.  Fixes issues when using the
  # same build directory with different python/protobuf versions
  # as the binary build directory is discarded across builds.
  #
  # Notice: copy_directory does not copy the content if the directory already exists.
  # We therefore append '/' to specify that we want to copy the content of the folder. See #847
  #
  add_custom_command(
      OUTPUT ${NANOPB_GENERATOR_EXECUTABLE} ${GENERATOR_CORE_SRC}
      COMMAND ${CMAKE_COMMAND} -E copy_directory
      ARGS ${NANOPB_GENERATOR_SOURCE_DIR}/ ${GENERATOR_PATH}
      VERBATIM)

  set(GENERATOR_CORE_PYTHON_SRC)
  foreach(FIL ${GENERATOR_CORE_SRC})
      get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
      get_filename_component(FIL_WE ${FIL} NAME_WE)

      set(output "${GENERATOR_CORE_DIR}/${FIL_WE}_pb2.py")
      set(GENERATOR_CORE_PYTHON_SRC ${GENERATOR_CORE_PYTHON_SRC} ${output})
      add_custom_command(
        OUTPUT ${output}
        COMMAND ${CUSTOM_COMMAND_PREFIX} ${PROTOBUF_PROTOC_EXECUTABLE}
        ARGS -I${GENERATOR_PATH}/proto
          --python_out=${GENERATOR_CORE_DIR} ${ABS_FIL}
        DEPENDS ${ABS_FIL}
        VERBATIM)
  endforeach()

  if(NANOPB_GENERATE_CPP_RELPATH)
      get_filename_component(ABS_ROOT ${NANOPB_GENERATE_CPP_RELPATH} ABSOLUTE)
  endif()
  foreach(FIL ${NANOPB_GENERATE_CPP_UNPARSED_ARGUMENTS})
    get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
    get_filename_component(FIL_WE ${FIL} NAME_WLE)
    get_filename_component(FIL_DIR ${ABS_FIL} PATH)
    set(FIL_PATH_REL)
    if(ABS_ROOT)
      # Check that the file is under the given "RELPATH"
      string(FIND ${ABS_FIL} ${ABS_ROOT} LOC)
      if (${LOC} EQUAL 0)
        string(REPLACE "${ABS_ROOT}/" "" FIL_REL ${ABS_FIL})
        get_filename_component(FIL_PATH_REL ${FIL_REL} PATH)
        file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${FIL_PATH_REL})
      endif()
    endif()
    if(NOT FIL_PATH_REL)
      set(FIL_PATH_REL ".")
    endif()

    list(APPEND ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/${FIL_PATH_REL}/${FIL_WE}.pb.c")
    list(APPEND ${HDRS} "${CMAKE_CURRENT_BINARY_DIR}/${FIL_PATH_REL}/${FIL_WE}.pb.h")

    # If there an options file in the same working directory, set it as a dependency
    get_filename_component(ABS_OPT_FIL ${FIL_DIR}/${FIL_WE}.options ABSOLUTE)
    if(EXISTS ${ABS_OPT_FIL})
        # Get directory as lookups for dependency options fail if an options
        # file is used. The options is still set as a dependency of the
        # generated source and header.
        get_filename_component(options_dir ${ABS_OPT_FIL} DIRECTORY)
        list(APPEND NANOPB_OPTIONS_DIRS ${options_dir})
    else()
        set(ABS_OPT_FIL)
    endif()

    # If the dependencies are options files, we need to pass the directories
    # as arguments to nanopb
    foreach(depends_file ${NANOPB_DEPENDS})
        get_filename_component(ext ${depends_file} EXT)
        if(ext STREQUAL ".options")
            get_filename_component(depends_dir ${depends_file} DIRECTORY)
            list(APPEND NANOPB_OPTIONS_DIRS ${depends_dir})
        endif()
    endforeach()

    if(NANOPB_OPTIONS_DIRS)
        list(REMOVE_DUPLICATES NANOPB_OPTIONS_DIRS)
    endif()

    set(NANOPB_PLUGIN_OPTIONS)
    foreach(options_path ${NANOPB_OPTIONS_DIRS})
        set(NANOPB_PLUGIN_OPTIONS "${NANOPB_PLUGIN_OPTIONS} -I${options_path}")
    endforeach()

    if(NANOPB_OPTIONS)
        set(NANOPB_PLUGIN_OPTIONS "${NANOPB_PLUGIN_OPTIONS} ${NANOPB_OPTIONS}")
    endif()

    # based on the version of protoc it might be necessary to add "/${FIL_PATH_REL}" currently dealt with in #516
    set(NANOPB_OUT "${CMAKE_CURRENT_BINARY_DIR}")

    # We need to pass the path to the option files to the nanopb plugin. There are two ways to do it.
    # - An older hacky one using ':' as option separator in protoc args preventing the ':' to be used in path.
    # - Or a newer one, using --nanopb_opt which requires a version of protoc >= 3.6
    # Since nanopb 0.4.6, --nanopb_opt is the default.
    if(DEFINED NANOPB_PROTOC_OLDER_THAN_3_6_0)
      set(NANOPB_OPT_STRING "--nanopb_out=${NANOPB_PLUGIN_OPTIONS}:${NANOPB_OUT}")
    else()
      set(NANOPB_OPT_STRING "--nanopb_opt=${NANOPB_PLUGIN_OPTIONS}" "--nanopb_out=${NANOPB_OUT}")
    endif()

    add_custom_command(
      OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${FIL_PATH_REL}/${FIL_WE}.pb.c"
             "${CMAKE_CURRENT_BINARY_DIR}/${FIL_PATH_REL}/${FIL_WE}.pb.h"
      COMMAND ${CUSTOM_COMMAND_PREFIX} ${PROTOBUF_PROTOC_EXECUTABLE}
      ARGS ${_nanopb_include_path} -I${GENERATOR_PATH}
           -I${GENERATOR_CORE_DIR} -I${CMAKE_CURRENT_BINARY_DIR}
           --plugin=protoc-gen-nanopb=${NANOPB_GENERATOR_PLUGIN}
           ${NANOPB_OPT_STRING}
           ${PROTOC_OPTIONS}
           ${ABS_FIL}
      DEPENDS ${ABS_FIL} ${GENERATOR_CORE_PYTHON_SRC}
           ${ABS_OPT_FIL} ${NANOPB_DEPENDS}
      COMMENT "Running C++ protocol buffer compiler using nanopb plugin on ${FIL}"
      VERBATIM )

  endforeach()

  set_source_files_properties(${${SRCS}} ${${HDRS}} PROPERTIES GENERATED TRUE)

  if(NOT DEFINED NANOPB_GENERATE_CPP_STANDALONE)
    set(NANOPB_GENERATE_CPP_STANDALONE TRUE)
  endif()
  
  if(MSVC)
      unset(CUSTOM_COMMAND_PREFIX)
  endif()

  if (NANOPB_GENERATE_CPP_STANDALONE)
    set(${SRCS} ${${SRCS}} ${NANOPB_SRCS} PARENT_SCOPE)
    set(${HDRS} ${${HDRS}} ${NANOPB_HDRS} PARENT_SCOPE)
  else()
    set(${SRCS} ${${SRCS}} PARENT_SCOPE)
    set(${HDRS} ${${HDRS}} PARENT_SCOPE)
  endif()
endfunction()

