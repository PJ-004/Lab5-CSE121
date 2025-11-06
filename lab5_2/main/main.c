/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include <time.h>

const static char *TAG = "EXAMPLE";

/*---------------------------------------------------------------
        ADC General Macros
---------------------------------------------------------------*/
//ADC1 Channels
#if CONFIG_IDF_TARGET_ESP32
#define EXAMPLE_ADC1_CHAN0          ADC_CHANNEL_4
#define EXAMPLE_ADC1_CHAN1          ADC_CHANNEL_5
#else
#define EXAMPLE_ADC1_CHAN0          ADC_CHANNEL_2
#define EXAMPLE_ADC1_CHAN1          ADC_CHANNEL_3
#endif

#if (SOC_ADC_PERIPH_NUM >= 2) && !CONFIG_IDF_TARGET_ESP32C3
/**
 * On ESP32C3, ADC2 is no longer supported, due to its HW limitation.
 * Search for errata on espressif website for more details.
 */
#define EXAMPLE_USE_ADC2            1
#endif

#if EXAMPLE_USE_ADC2
//ADC2 Channels
#define EXAMPLE_ADC2_CHAN0          ADC_CHANNEL_0
#endif  //#if EXAMPLE_USE_ADC2

#define EXAMPLE_ADC_ATTEN           ADC_ATTEN_DB_12

/*
#define DOT_DURATION		1
#define	DASH_DURATION		4
#define CHAR_DURATION		2
#define WORD_DURATION		6
#define END_SENTENCE		8
*/

/*
#define DOT_DURATION		20
#define	DASH_DURATION		40
#define CHAR_DURATION		10
#define WORD_DURATION		60
#define END_SENTENCE		80
*/

#define DOT_DURATION		10
#define	DASH_DURATION		40
#define CHAR_DURATION		20
#define WORD_DURATION		60
#define END_SENTENCE		80

static int adc_raw[2][10];
static int voltage[2][10];
static bool example_adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle);
static void example_adc_calibration_deinit(adc_cali_handle_t handle);

typedef struct {
    char* morse;
    char* letter;
} translation;

translation morse_code_t[] = {
    {"|"    , " , "},
    {"/"    , " " },

    {".-"   , "A" },
    {"-..." , "B" },
    {"-.-." , "C" },
    {"-.."  , "D" },
    {"."    , "E" },
    {"..-." , "F" },
    {"--."  , "G" },
    {"...." , "H" },
    {".."   , "I" },
    {".---" , "J" },
    {"-.-"  , "K" },
    {".-.." , "L" },
    {"--"   , "M" },
    {"-."   , "N" },
    {"---"  , "O" },
    {".--." , "P" },
    {"--.-" , "Q" },
    {".-."  , "R" },
    {"..."  , "S" },
    {"-"    , "T" },
    {"..-"  , "U" },
    {"...-" , "V" },
    {".--"  , "W" },
    {"-..-" , "X" },
    {"-.--" , "Y" },
    {"--.." , "Z" },
    
    {"-----", "0" },
    {".----", "1" },
    {"..---", "2" },
    {"...--", "3" },
    {"....-", "4" },
    {".....", "5" },
    {"-....", "6" },
    {"--...", "7" },
    {"---..", "8" },
    {"----.", "9" }
};

void translate(char* string, char* morse) {
    for (int i = 0; i < 37; ++i) {
    	if (!strcmp(morse_code_t[i].morse, morse)) {
	    strcat(string, morse_code_t[i].letter);
	}
    }
}

void app_main(void)
{
    //-------------ADC1 Init---------------//
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    //-------------ADC1 Config---------------//
    adc_oneshot_chan_cfg_t config = {
        .atten = EXAMPLE_ADC_ATTEN,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, EXAMPLE_ADC1_CHAN0, &config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, EXAMPLE_ADC1_CHAN1, &config));

    //-------------ADC1 Calibration Init---------------//
    adc_cali_handle_t adc1_cali_chan0_handle = NULL;
    adc_cali_handle_t adc1_cali_chan1_handle = NULL;
    bool do_calibration1_chan0 = example_adc_calibration_init(ADC_UNIT_1, EXAMPLE_ADC1_CHAN0, EXAMPLE_ADC_ATTEN, &adc1_cali_chan0_handle);
    bool do_calibration1_chan1 = example_adc_calibration_init(ADC_UNIT_1, EXAMPLE_ADC1_CHAN1, EXAMPLE_ADC_ATTEN, &adc1_cali_chan1_handle);

    int time = 0;
    bool led = false;
    char morse_char[128] = "";
    char translation[128] = "";
    while (true) {
	ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, EXAMPLE_ADC1_CHAN1, &adc_raw[0][1]));
        
	if (do_calibration1_chan1) {
            ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc1_cali_chan1_handle, adc_raw[0][1], &voltage[0][1]));

	    if (voltage[0][1] >= 310)
	        led = true;
	    else
	        led = false;
        }

	if (led) {
	    ++time;
	} else {
	    //if (time > 0)
	    //    ESP_LOGI(TAG, "LED was high for %d", time);

	    switch(time) {
		case DOT_DURATION:
		    //ESP_LOGI(TAG, "Symbol Found: '.'");
		    strcat(morse_char, ".");
		    break;
		case DASH_DURATION:
		    //ESP_LOGI(TAG, "Symbol Found: '-'");
		    strcat(morse_char, "-");
		    break;
		case WORD_DURATION:
		    //ESP_LOGI(TAG, "Symbol Found: '/'");
		    strcat(morse_char, "/");
		    break;
		case END_SENTENCE:
		    translate(translation, morse_char);
		    memset(morse_char, 0, 128);
		    translate(translation, "|");
		    ESP_LOGI(TAG, "Sentence so far: %s", translation);
		    break;
                case CHAR_DURATION:
		    //ESP_LOGI(TAG, "Symbol Found: ' '");
		    translate(translation, morse_char);
		    memset(morse_char, 0, 128);
		    ESP_LOGI(TAG, "Sentence so far: %s", translation);
		    break;
		default:
		   break; 
	    }
	    
	    //if (time > 0)
	    //	ESP_LOGI(TAG, "Sentence so far: %s | Current Letter in Morse: %s", translation, morse_char);
	    time = 0;
	}

        //vTaskDelay(pdMS_TO_TICKS(1));
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    //Tear Down
    ESP_ERROR_CHECK(adc_oneshot_del_unit(adc1_handle));
    if (do_calibration1_chan0) {
        example_adc_calibration_deinit(adc1_cali_chan0_handle);
    }
    if (do_calibration1_chan1) {
        example_adc_calibration_deinit(adc1_cali_chan1_handle);
    }
}

/*---------------------------------------------------------------
        ADC Calibration
---------------------------------------------------------------*/
static bool example_adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle)
{
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    if (!calibrated) {
        ESP_LOGI(TAG, "calibration scheme version is %s", "Curve Fitting");
        adc_cali_curve_fitting_config_t cali_config = {
            .unit_id = unit,
            .chan = channel,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    if (!calibrated) {
        ESP_LOGI(TAG, "calibration scheme version is %s", "Line Fitting");
        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

    *out_handle = handle;
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Calibration Success");
    } else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    } else {
        ESP_LOGE(TAG, "Invalid arg or no memory");
    }

    return calibrated;
}

static void example_adc_calibration_deinit(adc_cali_handle_t handle)
{
#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    ESP_LOGI(TAG, "deregister %s calibration scheme", "Curve Fitting");
    ESP_ERROR_CHECK(adc_cali_delete_scheme_curve_fitting(handle));

#elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    ESP_LOGI(TAG, "deregister %s calibration scheme", "Line Fitting");
    ESP_ERROR_CHECK(adc_cali_delete_scheme_line_fitting(handle));
#endif
}
