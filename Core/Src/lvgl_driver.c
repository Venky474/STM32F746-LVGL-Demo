///*
// * lvgl_driver.c
// */
//
//#include "lvgl_driver.h"
//#include <stdbool.h>
//#include "main.h"
//
//#include "stm32746g_discovery_lcd.h"
//#include "core_cm7.h"
//
///*********************
// *      DEFINES
// *********************/
//#define MY_DISP_HOR_RES    480
//#define MY_DISP_VER_RES    272
//
///**********************
// *  STATIC PROTOTYPES
// **********************/
//extern DMA2D_HandleTypeDef hdma2d;
//
//static void disp_init(void);
//static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
//static void DMA2D_CopyBuffer(uint32_t *pSrc, uint32_t *pDst, uint32_t xSize, uint32_t ySize);
//
///**********************
// *  STATIC VARIABLES
// **********************/
//static lv_disp_drv_t disp_drv;
//
///**********************
// *   GLOBAL FUNCTIONS
// **********************/
//
//void lv_port_disp_init(void)
//{
//    disp_init();
//
//    /* ---- SDRAM DRAW BUFFERS ---- */
//
//    static lv_disp_draw_buf_t draw_buf;
//
//    __attribute__((section(".sdram"), aligned(32)))
//    static lv_color_t buf1[MY_DISP_HOR_RES * 60];
//
//    __attribute__((section(".sdram"), aligned(32)))
//    static lv_color_t buf2[MY_DISP_HOR_RES * 60];
//
//    lv_disp_draw_buf_init(&draw_buf, buf1, buf2, MY_DISP_HOR_RES * 60);
//
//    /* ---- REGISTER DRIVER ---- */
//
//    lv_disp_drv_init(&disp_drv);
//
//    disp_drv.hor_res = MY_DISP_HOR_RES;
//    disp_drv.ver_res = MY_DISP_VER_RES;
//    disp_drv.flush_cb = disp_flush;
//    disp_drv.draw_buf = &draw_buf;
//
//    lv_disp_drv_register(&disp_drv);
//}
//
///**********************
// *   STATIC FUNCTIONS
// **********************/
//
//static void disp_init(void)
//{
//    BSP_LCD_Init();
//    BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);
//    BSP_LCD_SelectLayer(0);
//    BSP_LCD_Clear(LCD_COLOR_BLACK);
//    BSP_LCD_DisplayOn();
//}
//
///* ---------------- FLUSH ---------------- */
//
///* ---------------- FLUSH (USING DMA2D) ---------------- */
//
///* ---------------- FLUSH (USING DMA2D) ---------------- */
//
//static void disp_flush(lv_disp_drv_t * disp_drv,
//                       const lv_area_t * area,
//                       lv_color_t * color_p)
//{
//    uint32_t *fb = (uint32_t *)LCD_FB_START_ADDRESS;
//    uint32_t pitch = MY_DISP_HOR_RES;
//    uint32_t dst_address = (uint32_t)&fb[area->y1 * pitch + area->x1];
//    uint32_t src_address = (uint32_t)color_p;
//    uint32_t width = (area->x2 - area->x1 + 1);
//    uint32_t height = (area->y2 - area->y1 + 1);
//
//    DMA2D_CopyBuffer((uint32_t *)src_address, (uint32_t *)dst_address, width, height);
//}
//
//
///**
//  * @brief  Starts a DMA2D transfer to copy a buffer area
//  */
//static void DMA2D_CopyBuffer(uint32_t *pSrc, uint32_t *pDst, uint32_t xSize, uint32_t ySize)
//{
//  // Reinitialize DMA2D in Memory-to-Memory (M2M) mode for this specific transfer
//  hdma2d.Init.Mode = DMA2D_M2M;
//  hdma2d.Init.ColorMode = DMA2D_OUTPUT_ARGB8888;
//  hdma2d.Init.OutputOffset = 0;
//  HAL_DMA2D_Init(&hdma2d);
//
//  // Configure the DMA2D Layer 1 (Source) parameters
//  hdma2d.LayerCfg[1].InputOffset = 0;
//  hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_ARGB8888;
//  hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
//  hdma2d.LayerCfg[1].InputAlpha = 0;
//  HAL_DMA2D_ConfigLayer(&hdma2d, 1);
//
//  // Start the DMA2D transfer (using interrupt mode)
//  HAL_DMA2D_Start_IT(&hdma2d, (uint32_t)pSrc, (uint32_t)pDst, xSize, ySize);
//}
//
///**
//  * @brief  DMA2D Transfer Complete Callback
//  *         Signals LVGL that the flush operation is complete.
//  */
//void HAL_DMA2D_XferCpltCallback(DMA2D_HandleTypeDef *hdma2d)
//{
//    lv_disp_flush_ready(&disp_drv);
//}
//
//











//-------------------------------------------------------------------------------------------------------------
/*
 * lvgl_driver.c
 *
 *  Created on: Jan 21, 2026
 *      Author: PC
 */



/*********************
 *      INCLUDES
 *********************/
#include "lvgl_driver.h"
#include <stdbool.h>
#include "main.h"

#include "stm32746g_discovery_lcd.h"
#include "core_cm7.h"

/*********************
 *      DEFINES
 *********************/
#define MY_DISP_HOR_RES    480
#define MY_DISP_VER_RES    272


/**********************
 *      TYPEDEFS
 **********************/
/* ================= LVGL DRAW BUFFERS IN SDRAM ================= */

#define DRAW_BUF_LINES  120  /* you can change to 20 later */

__attribute__((section(".lvgl_draw_buf")))
lv_color_t lvgl_draw_buf1[MY_DISP_HOR_RES * DRAW_BUF_LINES];

__attribute__((section(".lvgl_draw_buf")))
lv_color_t lvgl_draw_buf2[MY_DISP_HOR_RES * DRAW_BUF_LINES];

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//        const lv_area_t * fill_area, lv_color_t color);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_disp_drv_t disp_drv;                         /*Descriptor of a display driver*/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/


void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/

    /**
     * LVGL requires a buffer where it internally draws the widgets.
     * Later this buffer will passed to your display driver's `flush_cb` to copy its content to your display.
     * The buffer has to be greater than 1 display row
     *
     * There are 3 buffering configurations:
     * 1. Create ONE buffer:
     *      LVGL will draw the display's content here and writes it to your display
     *
     * 2. Create TWO buffer:
     *      LVGL will draw the display's content to a buffer and writes it your display.
     *      You should use DMA to write the buffer's content to the display.
     *      It will enable LVGL to draw the next part of the screen to the other buffer while
     *      the data is being sent form the first buffer. It makes rendering and flushing parallel.
     *
     * 3. Double buffering
     *      Set 2 screens sized buffers and set disp_drv.full_refresh = 1.
     *      This way LVGL will always provide the whole rendered screen in `flush_cb`
     *      and you only need to change the frame buffer's address.
     */

    /* Example for 1) */
//    static lv_disp_draw_buf_t draw_buf_dsc_1;
//    static lv_color_t buf_1[MY_DISP_HOR_RES * 10];                          /*A buffer for 10 rows*/
//    lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, NULL, MY_DISP_HOR_RES * 10);   /*Initialize the display buffer*/

    /* Example for 2) */
//    static lv_disp_draw_buf_t draw_buf_dsc_2;
//    static lv_color_t buf_2_1[MY_DISP_HOR_RES * 60];
//    static lv_color_t buf_2_2[MY_DISP_HOR_RES * 60];              /*An other buffer for 10 rows*/
//    lv_disp_draw_buf_init(&draw_buf_dsc_2, buf_2_1, buf_2_2, MY_DISP_HOR_RES * 60);   /*Initialize the display buffer*/

    static lv_disp_draw_buf_t draw_buf_dsc_2;

    lv_disp_draw_buf_init(&draw_buf_dsc_2,
                          lvgl_draw_buf1,
                          lvgl_draw_buf2,
                          MY_DISP_HOR_RES * DRAW_BUF_LINES);

//    /* Example for 3) also set disp_drv.full_refresh = 1 below*/
//    static lv_disp_draw_buf_t draw_buf_dsc_3;
//    static lv_color_t buf_3_1[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /*A screen sized buffer*/
//    static lv_color_t buf_3_2[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /*Another screen sized buffer*/
//    lv_disp_draw_buf_init(&draw_buf_dsc_3, buf_3_1, buf_3_2,
//                          MY_DISP_VER_RES * LV_VER_RES_MAX);   /*Initialize the display buffer*/

    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/


    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    disp_drv.hor_res = MY_DISP_HOR_RES;
    disp_drv.ver_res = MY_DISP_VER_RES;

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_flush;

    /*Set a display buffer*/
    disp_drv.draw_buf = &draw_buf_dsc_2;

    /*Required for Example 3)*/
    //disp_drv.full_refresh = 1;

    /* Fill a memory array with a color if you have GPU.
     * Note that, in lv_conf.h you can enable GPUs that has built-in support in LVGL.
     * But if you have a different GPU you can use with this callback.*/
    //disp_drv.gpu_fill_cb = gpu_fill;

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
	  BSP_LCD_Init();
	  BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);
	  BSP_LCD_SetLayerWindow(0, 0, 0, 480, 272);
	  BSP_LCD_SetTransparency(0, 255);
	  BSP_LCD_SelectLayer(0);
	  BSP_LCD_DisplayOn();
}

volatile bool disp_flush_enabled = true;

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void)
{
    disp_flush_enabled = true;
}

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void)
{
    disp_flush_enabled = false;
}

//------------------------------------------------------------------------------------------
/*Flush the content of the internal buffer the specific area on the display
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_disp_flush_ready()' has to be called when finished.*/
//static void disp_flush(lv_disp_drv_t * disp_drv,
//                       const lv_area_t * area,
//                       lv_color_t * color_p)
//{
//    uint32_t *fb = (uint32_t *)LCD_FB_START_ADDRESS;
//
//    for(int y = area->y1; y <= area->y2; y++)
//    {
//        uint32_t index = y * MY_DISP_HOR_RES + area->x1;
//
//        for(int x = area->x1; x <= area->x2; x++)
//        {
//            fb[index++] = color_p->full;   // 32-bit ARGB
//            color_p++;
//        }
//    }
//
//    /* Cortex-M7 cache clean — IMPORTANT */
////    SCB_CleanDCache();
//
//    lv_disp_flush_ready(disp_drv);
//}

static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    // 1. Clean the source buffer (LVGL's draw buffer in SDRAM)
    uint32_t src_addr = (uint32_t)color_p;
    uint32_t src_size = lv_area_get_size(area) * sizeof(lv_color_t);
    SCB_CleanDCache_by_Addr((uint32_t*)src_addr, src_size);

    uint32_t *fb = (uint32_t *)LCD_FB_START_ADDRESS;

    // 2. Perform the copy
    for(int y = area->y1; y <= area->y2; y++) {
        uint32_t index = y * MY_DISP_HOR_RES + area->x1;
        for(int x = area->x1; x <= area->x2; x++) {
            fb[index++] = color_p->full;
            color_p++;
        }
    }

    // 3. Clean the Frame Buffer (Destination)
    // We clean from the start of the first row to the end of the last row modified
    uint32_t fb_start = (uint32_t)&fb[area->y1 * MY_DISP_HOR_RES];
    uint32_t fb_rows  = (area->y2 - area->y1 + 1);
    uint32_t fb_size  = fb_rows * MY_DISP_HOR_RES * sizeof(uint32_t);

    SCB_CleanDCache_by_Addr((uint32_t*)fb_start, fb_size);

    lv_disp_flush_ready(disp_drv);
}

//------------------------------------------------------------------------------------------
/*OPTIONAL: GPU INTERFACE*/

/*If your MCU has hardware accelerator (GPU) then you can use it to fill a memory with a color*/
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//                    const lv_area_t * fill_area, lv_color_t color)
//{
//    /*It's an example code which should be done by your GPU*/
//    int32_t x, y;
//    dest_buf += dest_width * fill_area->y1; /*Go to the first line*/
//
//    for(y = fill_area->y1; y <= fill_area->y2; y++) {
//        for(x = fill_area->x1; x <= fill_area->x2; x++) {
//            dest_buf[x] = color;
//        }
//        dest_buf+=dest_width;    /*Go to the next line*/
//    }
//}

