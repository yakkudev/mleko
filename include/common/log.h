#pragma once

#include <stdio.h>

#ifndef LOG_PREFIX
#define LOG_PREFIX
#endif // !LOG_PREFIX

#define LOG_COLOR_INFO "\x1B[32m"
#define LOG_COLOR_WARN "\x1B[33m"
#define LOG_COLOR_ERROR "\x1B[31m"
#define LOG_COLOR_RESET "\033[0m"

#define LOG(msg, ...) fprintf(stdout, LOG_PREFIX " [" LOG_COLOR_INFO "info" LOG_COLOR_RESET "] " msg "\n", ##__VA_ARGS__)
#define WARN(msg, ...) fprintf(stdout, LOG_PREFIX " [" LOG_COLOR_WARN "warn" LOG_COLOR_RESET "] " msg "\n", ##__VA_ARGS__)
#define ERROR(msg, ...) fprintf(stderr, LOG_PREFIX " [" LOG_COLOR_ERROR "error" LOG_COLOR_RESET "] " msg "\n", ##__VA_ARGS__)

