cmake_minimum_required(VERSION 2.8.0)

project(producer)

aux_source_directory(./ SOURCES)

add_library(producer ${SOURCES})
