cmake_minimum_required(VERSION 3.26)
project(deck CXX)

add_subdirectory(deps/fmt)
add_executable(deck src/deck.cpp)

target_compile_features(deck PRIVATE cxx_std_20)
target_link_libraries(deck fmt::fmt)

target_include_directories(deck PUBLIC include)
target_include_directories(deck PUBLIC deps/fmt/include)

target_compile_options(deck PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:/W4>
	$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)