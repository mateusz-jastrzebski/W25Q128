add_library(W25Q128 W25Q128.cpp)

target_include_directories(W25Q128 PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(W25Q128 PUBLIC hardware_spi pico_stdlib)
