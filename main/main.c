#include <string.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_adc/adc_oneshot.h"
#include "hal/adc_types.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_cali.h"

TaskHandle_t ADCTaskHandle = NULL;

void ADCTask(void *arg) {
    int adc_raw_1, adc_raw_2; // Lưu giá trị ADC

    /* Khoi tao ADC*/
    adc_oneshot_unit_handle_t handle = NULL;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &handle));

    /* Cấu hình ADC */
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_12, // Độ phân giải 12-bit
        .atten = ADC_ATTEN_DB_12, // Mở rộng dải điện áp đầu vào (0-3.9V)
    };

    ESP_ERROR_CHECK(adc_oneshot_config_channel(handle, ADC_CHANNEL_4, &config)); // GPIO 32
    ESP_ERROR_CHECK(adc_oneshot_config_channel(handle, ADC_CHANNEL_6, &config)); // GPIO 34


    /* Đọc dữ liệu ADC*/
    while(1) {
        ESP_ERROR_CHECK(adc_oneshot_read(handle, ADC_CHANNEL_4, &adc_raw_1)); // Đọc kênh 4
        ESP_ERROR_CHECK(adc_oneshot_read(handle, ADC_CHANNEL_6, &adc_raw_2)); // Đọc kênh 6
        
        printf(" ADC_CHANNEL_4 (GPIO 32): %d\n", adc_raw_1);
        printf(" ADC_CHANNEL_6 (GPIO 34): %d\n", adc_raw_2);
        printf("\n---------------------\n");

        vTaskDelay(pdMS_TO_TICKS(100)); // Đọc ADC mỗi 500ms
    }

    /* 5️⃣ GIẢI PHÓNG TÀI NGUYÊN */
    adc_oneshot_del_unit(handle);
    vTaskDelete(NULL);
}

void app_main(void) {
    xTaskCreatePinnedToCore(ADCTask, "ADC Task", 4096, NULL, 10, &ADCTaskHandle, 0);
}
