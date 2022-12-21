# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/Espressif/frameworks/esp-idf-v4.4.3/components/bootloader/subproject"
  "E:/Git/ESP32-S3/software/lesson5_blink/p1_blink/build/bootloader"
  "E:/Git/ESP32-S3/software/lesson5_blink/p1_blink/build/bootloader-prefix"
  "E:/Git/ESP32-S3/software/lesson5_blink/p1_blink/build/bootloader-prefix/tmp"
  "E:/Git/ESP32-S3/software/lesson5_blink/p1_blink/build/bootloader-prefix/src/bootloader-stamp"
  "E:/Git/ESP32-S3/software/lesson5_blink/p1_blink/build/bootloader-prefix/src"
  "E:/Git/ESP32-S3/software/lesson5_blink/p1_blink/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "E:/Git/ESP32-S3/software/lesson5_blink/p1_blink/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
