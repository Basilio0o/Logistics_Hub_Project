find_package(PostgreSQL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(logistics_hub ${SOURCES})

target_include_directories(logistics_hub PUBLIC include)
target_link_libraries(logistics_hub PRIVATE ${PQXX_LIBRARIES} PostgreSQL::PostgreSQL)
target_include_directories(logistics_hub PRIVATE ${PQXX_INCLUDE_DIRS})