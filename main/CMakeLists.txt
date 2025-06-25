file(GLOB_RECURSE pizda_sources
    "*.*"
)

idf_component_register(
    SRCS ${pizda_sources}
    PRIV_REQUIRES spi_flash
    INCLUDE_DIRS "."
    REQUIRES esp_timer esp_driver_ledc driver mbedtls nvs_flash esp_adc YOBA
)
