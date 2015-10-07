include(FindSubversion)

IF(Subversion_FOUND)

	if(EXISTS "${CMAKE_SOURCE_DIR}/.svn")

		Subversion_WC_INFO(${SOURCE_DIR} opncms)
		set (opncms_VERSION_MAJOR 0)
		set (opncms_VERSION_MINOR 1)
		set (opncms_VERSION_PATCH 0)
		file(WRITE version.h.in "#define opncms_VERSION_MAJOR ${opncms_VERSION_MAJOR}\n"
			"#define opncms_VERSION_MINOR ${opncms_VERSION_MINOR}\n"
			"#define opncms_VERSION_PATCH ${opncms_VERSION_PATCH}\n"
			"#define opncms_REVISION ${opncms_WC_REVISION}\n")
		execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different version.h.in version.h)
	else()

		file(WRITE version.h.in "#define opncms_VERSION_MAJOR 0\n"
			"#define opncms_VERSION_MINOR 0\n"
                        "#define opncms_VERSION_PATCH 0\n"
                        "#define opncms_REVISION -1\n")
                execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different version.h.in version.h)
	endif()

ENDIF(Subversion_FOUND)
