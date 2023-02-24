cmake_minimum_required(VERSION 3.10)

project(TransportCataogue CXX)
set(CMAKE_CXX_STANDARD 17)

# Эта команда найдёт собранный нами пакет Protobuf.
# REQUIRED означает, что библиотека обязательна.
# Путь для поиска укажем в параметрах команды cmake.
find_package(Protobuf REQUIRED)
# Помимо Protobuf, понадобится библиотека Threads
find_package(Threads REQUIRED)

# Команда вызова protoc. 
# Ей переданы названия переменных, в которые будут сохранены 
# списки сгенерированных файлов, а также сам proto-файл.
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS transport_catalogue.proto map_renderer.proto svg.proto graph.proto)


set(CATALOG_FILES main.cpp 
			domain.h 
			geo.h geo.cpp 
			graph.h graph.proto
			json.h json.cpp 
			json_builder.h json_builder.cpp 
			json_reader.h json_reader.cpp 
			map_renderer.h map_renderer.cpp map_renderer.proto
			request_handler.h request_handler.cpp 
			router.h 
			serialization.h serialization.cpp 
			svg.h svg.cpp svg.proto
			transport_catalogue.h transport_catalogue.cpp transport_catalogue.proto
			transport_router.h transport_router.cpp)

# добавляем цель - transport_catalogue
add_executable(transport_catalogue ${PROTO_SRCS} ${PROTO_HDRS} ${CATALOG_FILES})

target_include_directories(transport_catalogue PUBLIC ${Protobuf_INCLUDE_DIRS})
target_include_directories(transport_catalogue PUBLIC ${CMAKE_CURRENT_BINARY_DIR})


target_link_libraries(transport_catalogue "$<IF:$<CONFIG:Debug>,${Protobuf_LIBRARY_DEBUG},${Protobuf_LIBRARY}>" Threads::Threads)
