cmake_minimum_required(VERSION 3.10)
project(transport_routes)

# Вказуємо стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Додаємо виконуваний файл з вихідного коду
add_executable(transport_routes transport_routes.cpp)  # Замініть transport_routes.cpp на свій основний файл

# Додаємо шляхи до бібліотек
find_package(CURL REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)

# Лінкуємо бібліотеки з проектом
target_link_libraries(transport_routes PRIVATE CURL::libcurl jsoncpp_lib)
