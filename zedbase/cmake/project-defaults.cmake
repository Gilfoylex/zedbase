set(CMAKE_CXX_STANDARD 20)

add_compile_definitions($<$<CONFIG:DEBUG>:DEBUG>)

if(MSVC)

add_compile_options(
    # use utf8 character set
    /utf-8
    )

add_compile_definitions("NOMINMAX" "UNICODE" "_UNICODE")

endif()

add_compile_definitions(
    $<$<PLATFORM_ID:Linux>:IS_LINUX>
    $<$<PLATFORM_ID:Windows>:IS_WINDOWS>
)