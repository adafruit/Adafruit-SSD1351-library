# Adafruit SSD1351 Library
# https://github.com/adafruit/Adafruit-SSD1351-Library
# BSD License

cmake_minimum_required(VERSION 3.5)

idf_component_register(SRCS "Adafruit_SSD1351.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES arduino Adafruit-GFX-Library)

project(Adafruit-SSD1351-library)
