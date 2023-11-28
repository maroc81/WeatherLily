//******************************************************************************
//! \file main.cpp
//!
//! Entry point for the weather app.
//!
//! Taken from https://github.com/lvgl/lv_port_esp32 and modified.
//!
//******************************************************************************


#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "driver/gpio.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "nvs_flash.h"

// LWIP includes
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/apps/sntp.h"

// LVGL includes
#include "lvgl.h"
#include "lvgl_helpers.h"
#include "lv_png.h"

// Weather app includes
#include "appConfig.h"
#include "weatherApp.hpp"
#include "net/wifi.h"
#include "ui/curConditionsView.hpp"
#include "weather/openWeather.hpp"


/*********************
 *      DEFINES
 *********************/
#define LV_TICK_PERIOD_MS 1

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_tick_task(void *arg);
static void guiTask(void *pvParameter);
static void weatherTask(void *pvParameter);

extern "C" {
    void app_main();
}

static const char * TAG = "main";

// Allocate weather app instance
static weatherAppT weatherAppLC;

/* Creates a semaphore to handle concurrent call to lvgl stuff
 * If you wish to call *any* lvgl function from other threads/tasks
 * you should lock on the very same semaphore! */
SemaphoreHandle_t xGuiSemaphore;


//******************************************************************************
//! app_main()
//
//! Entry point for the espidf framework.
//
//******************************************************************************
void app_main() {

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI("wifi station", "ESP_WIFI_MODE_STA");
    wifi_init_sta();

    // Set time zone and start simple network time
    setenv("TZ", APP_TZ, 1);
    tzset();
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();

    xGuiSemaphore = xSemaphoreCreateMutex();

    /* If you want to use a task to create the graphic, you NEED to create a Pinned task
     * Otherwise there can be problem such as memory corruption and so on.
     * NOTE: When not using Wi-Fi nor Bluetooth you can pin the guiTask to core 0 */

    // Create task for lvgl/GUI on core 1
    xTaskCreatePinnedToCore(guiTask, "gui", 4096*2, NULL, 0, NULL, 1);


    // Create task for fetching weather on core 0
    xTaskCreatePinnedToCore(weatherTask, "weather", 4096*2, NULL, 0, NULL, 0);
}


//******************************************************************************
//! weatherTask()
//
//! Task for periodically polling current weather and forecasts
//
//******************************************************************************
static void weatherTask(void *pvParameter)
{
    (void) pvParameter;

    // Update weather every 5 minutes
    const int weatherRateSecsC = 60 * 5;
    int secsSinceLastUpdate = weatherRateSecsC;

    while (1) {
        // Delay 1 second
        vTaskDelay(pdMS_TO_TICKS(1000));
        secsSinceLastUpdate++;

        if( secsSinceLastUpdate >= weatherRateSecsC )
        {
            ESP_LOGI(TAG, "Fetching weather...");
            // Fetch weather and save
            weatherAppLC.UpdateWeather();
            ESP_LOGI(TAG, "Done");
            secsSinceLastUpdate = 0;

            // Push weather to UI
            if( pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY) )
            {
                ESP_LOGI(TAG, "Updating UI with weather values");
                weatherAppLC.UpdateWeatherUI();
                xSemaphoreGive(xGuiSemaphore);
            }
        }

    }

    // A task should NEVER return
    vTaskDelete(NULL);
}


//******************************************************************************
//! guiTask()
//
//! Task for executing LVGL and periodic GUI updates
//
//******************************************************************************
static void guiTask(void *pvParameter)
{
    (void) pvParameter;

    lv_init();

    /* Initialize SPI or I2C bus used by the drivers */
    lvgl_driver_init();

    // Initialize the PNG decoder
    lv_png_init();

    lv_color_t* buf1 = (lv_color_t*)heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf1 != NULL);

    /* Use double buffered when not working with monochrome displays */
    lv_color_t* buf2 = (lv_color_t*)heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf2 != NULL);

    static lv_disp_buf_t disp_buf;

    uint32_t size_in_px = DISP_BUF_SIZE;

    /* Initialize the working buffer depending on the selected display.
     * NOTE: buf2 == NULL when using monochrome displays. */
    lv_disp_buf_init(&disp_buf, buf1, buf2, size_in_px);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = disp_driver_flush;

    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    /* Register an input device when enabled on the menuconfig */
#if CONFIG_LV_TOUCH_CONTROLLER != TOUCH_CONTROLLER_NONE
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.read_cb = touch_driver_read;
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);
#endif

    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

    weatherAppLC.InitUi(lv_scr_act());

    while (1) {
        /* Delay 1 tick (assumes FreeRTOS tick is 10ms */
        vTaskDelay(pdMS_TO_TICKS(10));

        /* Try to take the semaphore, call lvgl related function on success */
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY)) {
            lv_task_handler();
            weatherAppLC.UpdateUI();
            xSemaphoreGive(xGuiSemaphore);
       }
    }

    /* A task should NEVER return */
    free(buf1);
    free(buf2);
    vTaskDelete(NULL);
}


static void lv_tick_task(void *arg) {
    (void) arg;

    lv_tick_inc(LV_TICK_PERIOD_MS);
}
