/*
 * lvgl_mem.c
 *
 *  Created on: Feb 2, 2026
 *      Author: PC
 */

#include <stdint.h>

/* Force symbol emission */
__attribute__((used, section(".lvgl_heap")))
uint8_t lvgl_heap[1 * 1024 * 1024];
