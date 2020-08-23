project(test_server)

file(GLOB SRC src/*.cpp)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} common ${CONAN_LIBS})
