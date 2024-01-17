idf_component_register(SRCS 
    "app.cpp" "ca.cpp" "io.cpp" "main.cpp"
    INCLUDE_DIRS "."
    REQUIRES nvs_flash embr j1939 can esp_helper)
