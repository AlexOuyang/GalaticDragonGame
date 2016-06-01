IF(NOT EXISTS "/Users/chenxingouyang/Documents/Github/GameEngine/lib/assimp-3.2/install_manifest.txt")
  MESSAGE(FATAL_ERROR "Cannot find install manifest: \"/Users/chenxingouyang/Documents/Github/GameEngine/lib/assimp-3.2/install_manifest.txt\"")
ENDIF(NOT EXISTS "/Users/chenxingouyang/Documents/Github/GameEngine/lib/assimp-3.2/install_manifest.txt")

FILE(READ "/Users/chenxingouyang/Documents/Github/GameEngine/lib/assimp-3.2/install_manifest.txt" files)
STRING(REGEX REPLACE "\n" ";" files "${files}")
FOREACH(file ${files})
  MESSAGE(STATUS "Uninstalling \"$ENV{DESTDIR}${file}\"")
  EXEC_PROGRAM(
    "/usr/local/Cellar/cmake/3.5.2/bin/cmake" ARGS "-E remove \"$ENV{DESTDIR}${file}\""
    OUTPUT_VARIABLE rm_out
    RETURN_VALUE rm_retval
    )
  IF(NOT "${rm_retval}" STREQUAL 0)
    MESSAGE(FATAL_ERROR "Problem when removing \"$ENV{DESTDIR}${file}\"")
  ENDIF(NOT "${rm_retval}" STREQUAL 0)
ENDFOREACH(file)
