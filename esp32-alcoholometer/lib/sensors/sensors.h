#ifndef __SENSORS_H__
#define __SENSORS_H__

#include "project_config.h"
#include "def_consts.h"
#include "esp_timer.h"
#include "reSensor.h" 
#include "reDS18x20.h"
#include "reBMP280.h"
#include "reLCD.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "reGpio.h"

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------- Сенсоры -------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

// Минимальный интервал чтения данных с сенсоров
#define SENSORS_MINIMAL_READ_INTERVAL 750

// DS18B20 : основной датчик температуры паров спирта в дефлегматоре (до конденсатора)
#define SENSOR_COLUMN_NAME            "DS18B20"
#define SENSOR_COLUMN_KEY             "col"
#define SENSOR_COLUMN_TOPIC           "ds18b20"
#define SENSOR_COLUMN_INDEX           1 
#define SENSOR_COLUMN_ADDR            ONEWIRE_NONE
#define SENSOR_COLUMN_PIN             CONFIG_GPIO_1WIRE
#define SENSOR_COLUMN_RESOLUTION      DS18x20_RESOLUTION_12_BIT
#define SENSOR_COLUMN_SAVE_SP         true
#define SENSOR_COLUMN_FILTER_MODE     SENSOR_FILTER_AVERAGE
#define SENSOR_COLUMN_FILTER_SIZE     16
#define SENSOR_COLUMN_ERRORS_LIMIT    3

static DS18x20 sensorColumn(1);

// BME280 : измерение давления для компенсации влияния колебаний атмосферного давления на температуру кипения воды
#define SENSOR_PRESSURE_NAME          "BMP280"
#define SENSOR_PRESSURE_KEY           "prs"
#define SENSOR_PRESSURE_BUS           I2C_NUM_0
#define SENSOR_PRESSURE_ADDRESS       BMP280_I2C_ADDR_PRIM
#define SENSOR_PRESSURE_MODE          BMP280_MODE_FORCED
#define SENSOR_PRESSURE_ODR           BMP280_STANDBY_1000ms
#define SENSOR_PRESSURE_FILTER        BMP280_FLT_NONE
#define SENSOR_PRESSURE_OSM           BMP280_OSM_X4
#define SENSOR_PRESSURE_TOPIC         "bmp280"
#define SENSOR_PRESSURE_FILTER_MODE   SENSOR_FILTER_RAW
#define SENSOR_PRESSURE_FILTER_SIZE   0
#define SENSOR_PRESSURE_ERRORS_LIMIT  16

static BMP280 sensorPressure(2);

// Шаблон отправки данных на MQTT
#define ALCOHOL_JSON_TOPIC            "alcohol"
#define ALCOHOL_JSON_QOS              1
#define ALCOHOL_JSON_RETAINED         true

// Период публикации данных с сенсоров на MQTT
static uint32_t iMqttPubInterval = CONFIG_MQTT_SENSORS_SEND_INTERVAL;
// Период публикации данных с сенсоров на OpenMon
#if CONFIG_OPENMON_ENABLE
static uint32_t iOpenMonInterval = CONFIG_OPENMON_SEND_INTERVAL;
#endif // CONFIG_OPENMON_ENABLE
// Период публикации данных с сенсоров на NarodMon
#if CONFIG_NARODMON_ENABLE
static uint32_t iNarodMonInterval = CONFIG_NARODMON_SEND_INTERVAL;
#endif // CONFIG_NARODMON_ENABLE
// Период публикации данных с сенсоров на ThingSpeak
#if CONFIG_THINGSPEAK_ENABLE
static uint32_t iThingSpeakInterval = CONFIG_THINGSPEAK_SEND_INTERVAL;
#endif // CONFIG_THINGSPEAK_ENABLE

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------- Кнопки --------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

static reGPIO btnMode(CONFIG_GPIO_BTN_MODE, CONFIG_GPIO_BTN_LEVEL, CONFIG_GPIO_BTN_PULLUP, true, CONFIG_BUTTON_DEBOUNCE_TIME_US, nullptr);
static reGPIO btnMenu(CONFIG_GPIO_BTN_MENU, CONFIG_GPIO_BTN_LEVEL, CONFIG_GPIO_BTN_PULLUP, true, CONFIG_BUTTON_DEBOUNCE_TIME_US, nullptr);
static reGPIO btnPlus(CONFIG_GPIO_BTN_PLUS, CONFIG_GPIO_BTN_LEVEL, CONFIG_GPIO_BTN_PULLUP, true, CONFIG_BUTTON_DEBOUNCE_TIME_US, nullptr);
static reGPIO btnMinus(CONFIG_GPIO_BTN_MINUS, CONFIG_GPIO_BTN_LEVEL, CONFIG_GPIO_BTN_PULLUP, true, CONFIG_BUTTON_DEBOUNCE_TIME_US, nullptr);

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------- Дисплей -------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

#define CONFIG_LCD_I2C_BUS      I2C_NUM_1
#define CONFIG_LCD_I2C_ADDRESS  0x27
#define CONFIG_LCD_I2C_TYPE     16, 2

#define CONFIG_LCD_MSG_LOADING  "Загрузка..."

static reLCD lcd(CONFIG_LCD_I2C_BUS, CONFIG_LCD_I2C_ADDRESS, CONFIG_LCD_I2C_TYPE);

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------- Зуммер --------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

#if defined(CONFIG_GPIO_BUZZER_ACTIVE)

#define CONFIG_BEEP_KEY_PRESS     lmFlash, 1, 100, 100
#define CONFIG_BEEP_KEY_SAVE      lmFlash, 2, 100, 100
#define CONFIG_BEEP_KEY_ERROR     lmFlash, 3, 100, 100
#define CONFIG_BEEP_MODE_CHANGE   lmFlash, 1, 250, 100
#define CONFIG_BEEP_RAPID_CHANGES lmFlash, 4, 100, 100
#define CONFIG_BEEP_TEMP_EXCESS   lmFlash, 50, 100, 100

#else

#define CONFIG_BEEP_KEY_PRESS     2350, 100, 1
#define CONFIG_BEEP_KEY_SAVE      2350, 100, 2
#define CONFIG_BEEP_KEY_ERROR     2350, 100, 3
#define CONFIG_BEEP_MODE_CHANGE   2350, 250, 1
#define CONFIG_BEEP_RAPID_CHANGES 2350, 100, 4
#define CONFIG_BEEP_TEMP_EXCESS   2350, 100, 50

#endif // CONFIG_GPIO_BUZZER_ACTIVE
// -----------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------- Общие параметры ---------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

// Уведомления в telegram
typedef enum {
  NOTIFY_OFF    = 0,
  NOTIFY_SILENT = 1,
  NOTIFY_SOUND  = 2
} notify_type_t;

static notify_type_t notify_tg = NOTIFY_SOUND;
static uint8_t notify_buzzer = 1;

#define CONFIG_ALCO_PGROUP_KEY                  "alc"
#define CONFIG_ALCO_PGROUP_TOPIC                "alco"
#define CONFIG_ALCO_PGROUP_FRIENDLY             "Алкогометр"

#define CONFIG_NOTIFY_TG_KEY                    "notify"
#define CONFIG_NOTIFY_TG_FRIENDLY               "Уведомления в telegram" 
#define CONFIG_NOTIFY_BUZZER_KEY                "buzzer"
#define CONFIG_NOTIFY_BUZZER_FRIENDLY           "Звуковые уведомления" 

// -----------------------------------------------------------------------------------------------------------------------
// -------------------------------------------- Общие константы и переменные ---------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

// Режим работы
typedef enum {
  MODE_NORMAL     = 0,    // Нормальный режим
  MODE_CORRECTION = 1,    // Режим установки температур корректировки
  MODE_WBOILING   = 2     // Режим установки температуры кипения чистой воды
} setup_mode_t;  
static setup_mode_t mode = MODE_NORMAL;

// Режим отображения температуры в обычном режиме
typedef enum {
  TM_RAW           = 0,   // Измеренное значение
  TM_COMPENSATED   = 1,   // Измеренное значение, скорректированное по давлению
  TM_CORRECTED     = 2,   // Полностью скорректированное значение
  TM_EXTENDED      = 3    // Расширенный вид 
} temp_show_t;
static temp_show_t temp_show = TM_CORRECTED;

// Время начала процесса перегонки
static time_t time_start = 0;
static time_t duration_sec = 0;
static uint16_t duration_h = 0;
static uint16_t duration_m = 0;
static uint16_t duration_s = 0;
// Температура считанная с датчика
static double temp_measured = NAN;
// Температура скомпенсированная по давлению
static double temp_pressure_compensated = NAN;
// Корректировка температуры в данном температурном дапазоне
static double temp_correction = NAN;
// Содержание спирта в парах (вычисляемое)
static double alcohol_content = NAN;
// Шаг изменения параметров с клавиатуры
static uint8_t step_changes = 10;

// -----------------------------------------------------------------------------------------------------------------------
// ---------------------------------------- Пересчет температуры в спиртуозность -----------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

// Размерность массива пересчета температуры в крепость спирта
// Источник исходной таблицы пересчета: https://forum.homedistiller.ru/index.php?topic=108287.0
// Расчет взят из версии 2.0 и немного изменен (так как в исходном коде присутствует ошибка, которая принципиально игнорирует t2s_strengths[0])
// Таблица пересчета (график) также немного "причесана", оригинал: {9688, 9513, 9366, 9274, 9181, 9087, 8981, 8887, 8783, 8716, 8661, 8604, 8562, 8523, 8479, 8436, 8391, 8344, 8293, 8264, 8236, 8213, 8189, 8167, 8150, 8128, 8109, 8097, 8081, 8054, 8037, 8023, 8007, 7993, 7978, 7963, 7951, 7938, 7921, 7904, 7887, 7872, 7847, 7829, 7812, 7797, 7780, 7761, 7739, 7716, 7694, 7671, 7644, 7617, 7592, 7568, 7541, 7517, 7489, 7460, 7434, 7407, 7379, 7352, 7325, 7297, 7270, 7245, 7219, 7193, 7168, 7141, 7116, 7088, 7063, 7035, 7005, 6973, 6943, 6914, 6888, 6859, 6831, 6802, 6769, 6737, 6709, 6682, 6654, 6626, 6598, 6571, 6544, 6513, 6486, 6449, 6413, 6377, 6339, 6301, 6267, 6233, 6203, 6168, 6132, 6097, 6065, 6028, 5996, 5958, 5922, 5889, 5858, 5822, 5791, 5758, 5727, 5690, 5657, 5627, 5595, 5560, 5527, 5500, 5465, 5431, 5397, 5366, 5317, 5287, 5265, 5232, 5200, 5176, 5142, 5106, 5070, 5034, 4998, 4970, 4921, 4861, 4800, 4738, 4672, 4632, 4618, 4597, 4576, 4554, 4527, 4478, 4442, 4393, 4345, 4296, 4246, 4195, 4143, 4104, 4052, 4000, 3946, 3892, 3836, 3796, 3740, 3681, 3623, 3565, 3507, 3444, 3395, 3329, 3263, 3196, 3129, 3061, 2991, 2940, 2869, 2798, 2705, 2588, 2472, 2354, 2225, 2067, 1933, 1792, 1647, 1470, 1316, 1166, 1014, 878, 788, 591, 0};

// Температуры от 78.2 до 99.7 с переменным шагом ~ 0.1°С 
const uint8_t t2s_size = 199;
// Массив температур для расчета крепости в дистилляторе
const uint16_t t2s_temps[t2s_size] = {782, 784, 786, 788, 790, 792, 794, 796, 798, 800, 802, 804, 806, 808, 810, 812, 814, 816, 818, 819, 820, 821, 822, 823, 824, 825, 826, 827, 828, 829, 830, 831, 832, 833, 834, 835, 836, 837, 838, 839, 840, 841, 842, 843, 844, 845, 846, 847, 848, 849, 850, 851, 852, 853, 854, 855, 856, 857, 858, 859, 860, 861, 862, 863, 864, 865, 866, 867, 868, 869, 870, 871, 872, 873, 874, 875, 876, 877, 878, 879, 880, 881, 882, 883, 884, 885, 886, 887, 888, 889, 890, 891, 892, 893, 894, 895, 896, 897, 898, 899, 900, 901, 902, 903, 904, 905, 906, 907, 908, 909, 910, 911, 912, 913, 914, 915, 916, 917, 918, 919, 920, 921, 922, 923, 924, 925, 926, 927, 928, 929, 930, 931, 932, 933, 934, 935, 936, 937, 938, 939, 940, 941, 942, 943, 944, 945, 946, 947, 948, 949, 950, 951, 952, 953, 954, 955, 956, 957, 958, 959, 960, 961, 962, 963, 964, 965, 966, 967, 968, 969, 970, 971, 972, 973, 974, 975, 976, 977, 978, 979, 980, 981, 982, 983, 984, 985, 986, 987, 988, 989, 990, 991, 992, 993, 994, 995, 996, 997, 1000};
// Массив крепостей для расчета крепости в дистилляторе
const uint16_t t2s_strengths[t2s_size] = {9688, 9513, 9366, 9274, 9181, 9087, 8981, 8887, 8793, 8726, 8661, 8604, 8562, 8523, 8479, 8436, 8391, 8344, 8293, 8264, 8236, 8213, 8189, 8167, 8150, 8128, 8109, 8097, 8081, 8054, 8037, 8023, 8007, 7993, 7978, 7963, 7951, 7938, 7921, 7904, 7887, 7872, 7847, 7829, 7812, 7797, 7780, 7761, 7739, 7716, 7694, 7671, 7644, 7617, 7592, 7568, 7541, 7517, 7489, 7460, 7434, 7407, 7379, 7352, 7325, 7297, 7270, 7245, 7219, 7193, 7168, 7141, 7116, 7088, 7063, 7035, 7005, 6973, 6943, 6914, 6888, 6859, 6831, 6802, 6769, 6737, 6709, 6682, 6654, 6626, 6598, 6571, 6544, 6513, 6486, 6449, 6413, 6377, 6339, 6301, 6267, 6233, 6203, 6168, 6132, 6097, 6065, 6028, 5996, 5958, 5922, 5889, 5858, 5822, 5791, 5758, 5727, 5690, 5657, 5627, 5595, 5560, 5527, 5500, 5465, 5431, 5397, 5366, 5317, 5287, 5265, 5232, 5200, 5176, 5142, 5106, 5070, 5034, 4998, 4970, 4931, 4900, 4848, 4818, 4765, 4715, 4675, 4633, 4588, 4554, 4527, 4478, 4442, 4393, 4345, 4296, 4246, 4195, 4143, 4104, 4052, 4000, 3946, 3892, 3836, 3776, 3724, 3681, 3623, 3565, 3507, 3444, 3395, 3329, 3263, 3196, 3129, 3061, 2991, 2918, 2855, 2768, 2688, 2578, 2452, 2345, 2225, 2067, 1933, 1792, 1647, 1470, 1316, 1166, 1014, 878, 688, 398, 0};


// -----------------------------------------------------------------------------------------------------------------------
// ----------------------------------------------- Калибровки температуры ------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

// Калибровка по точке кипения воды (без спирта)
static float water_boiling_offset = 0.0;
static paramsEntryHandle_t water_boiling_offset_param = nullptr;
static uint8_t water_boiling_offset_changes = 0;
// Корректировка температуры по диапазонам
const uint8_t temp_correction_count = 16;
const uint8_t temp_correction_boundaries[temp_correction_count] = {0, 78, 81, 84, 87, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 110};
static float temp_correction_offsets[temp_correction_count - 1] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
static paramsEntryHandle_t temp_correction_params[temp_correction_count - 1] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
static uint8_t temp_correction_changes[temp_correction_count - 1] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// Плавный переход корректировки температур между диапазонами
static bool temp_correction_smooth_transition = true;

#define CONFIG_ALCO_CORRECTION_KEY              "crt"
#define CONFIG_ALCO_CORRECTION_TOPIC            "corrections"
#define CONFIG_ALCO_CORRECTION_FRIENDLY         "Корректировка температуры"
#define CONFIG_ALCO_WATER_BOILINFG_KEY          "wboiling"
#define CONFIG_ALCO_WATER_BOILINFG_FRIENDLY     "Точка кипения воды (100 °C)"
#define CONFIG_ALCO_RANGE_OFFSET_KEY            "range_%d-%d"
#define CONFIG_ALCO_RANGE_OFFSET_FRIENDLY       "Диапазон %d-%d °C"
#define CONFIG_ALCO_RANGE_SMOOTH_KEY            "smooth"
#define CONFIG_ALCO_RANGE_SMOOTH_FRIENDLY       "Плавный переход между диапазонами"

// -----------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------- Этапы перегонки ---------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

// Стадия процесса перегонки
typedef enum {
  STAGE_NONE      = 0,    // По умолчанию
  STAGE_HEATING   = 1,    // Нагрев
  STAGE_HEADS     = 2,    // Головы
  STAGE_BODY      = 3,    // Тело
  STAGE_TAILS     = 4,    // Хвосты
  STAGE_MAX       = 5     
} stage_t;
static stage_t stage = STAGE_NONE;
// Обозначение этапов перегонки
const uint8_t stage_tags[STAGE_MAX] = {0x20,0x48,0xC3,0x54,0x58};

// Границы этапов по температуре
static float stage_temp_heads = 75.0;
static paramsEntryHandle_t stage_temp_heads_param = nullptr;
static float stage_temp_body  = 78.0;
static paramsEntryHandle_t stage_temp_body_param = nullptr;
static float stage_temp_tails = 95.0;
static paramsEntryHandle_t stage_temp_tails_param = nullptr;
static float stage_temp_stop  = 99.0;
static paramsEntryHandle_t stage_temp_stop_param = nullptr;
// Интервал отправки предупреждений о необходимости смены емкости и переключения этапа
static time_t stage_temp_warning_last = 0;
static uint32_t stage_temp_warning_interval = 30;

#define CONFIG_ALCO_STAGES_KEY                  "stg"
#define CONFIG_ALCO_STAGES_TOPIC                "stages"
#define CONFIG_ALCO_STAGES_FRIENDLY             "Границы режимов работы"
#define CONFIG_ALCO_STAGE_HEADS_KEY             "heads"
#define CONFIG_ALCO_STAGE_HEADS_FRIENDLY        "Головы"
#define CONFIG_ALCO_STAGE_BODY_KEY              "body"
#define CONFIG_ALCO_STAGE_BODY_FRIENDLY         "Тело"
#define CONFIG_ALCO_STAGE_TAILS_KEY             "tails"
#define CONFIG_ALCO_STAGE_TAILS_FRIENDLY        "Хвосты"
#define CONFIG_ALCO_STAGE_STOP_KEY              "stop"
#define CONFIG_ALCO_STAGE_STOP_FRIENDLY         "Окончание перегона"
#define CONFIG_ALCO_STAGE_INTERVAL_KEY          "interval"
#define CONFIG_ALCO_STAGE_INTERVAL_FRIENDLY     "Интервал отправки предупреждений о смене режима"

#define CONFIG_ALCO_MSG_MODE_DATA               "\n\n<code>Температура:    %.3f °C\nСпиртуозность:  %.2f %%\nПрошло времени: %.2d:%.2d:%.2d</code>"
#define CONFIG_ALCO_MSG_MODE_HEATING            "🔥 <b>Процесс перегонки запущен</b>"
#define CONFIG_ALCO_MSG_MODE_HEADS              "🟠 <b>Начат отбор голов</b>" CONFIG_ALCO_MSG_MODE_DATA
#define CONFIG_ALCO_MSG_MODE_BODY               "🟢 <b>Начат отбор основного продукта</b>" CONFIG_ALCO_MSG_MODE_DATA
#define CONFIG_ALCO_MSG_MODE_TAILS              "🟣 <b>Начат отбор хвостов</b>" CONFIG_ALCO_MSG_MODE_DATA
#define CONFIG_ALCO_MSG_MODE_STOP               "✅ <b>Процесс перегонки завершен</b>" CONFIG_ALCO_MSG_MODE_DATA

#define CONFIG_ALCO_MSG_SWITCH_DATA             "\n\n<code>Температура:    %.3f °C\nСпиртуозность:  %.2f %%\nПрошло времени: %.2d:%.2d:%.2d</code>"
#define CONFIG_ALCO_MSG_SWITCH_HEADS            "🌡️ <b>Внимание! Установите емкость для отбора голов и переключите режим</b>" CONFIG_ALCO_MSG_SWITCH_DATA
#define CONFIG_ALCO_MSG_SWITCH_BODY             "🌡️ <b>Внимание! Установите емкость для отбора основного продукта и переключите режим</b>" CONFIG_ALCO_MSG_SWITCH_DATA
#define CONFIG_ALCO_MSG_SWITCH_TAILS            "🌡️ <b>Внимание! Установите емкость для отбора хвостов и переключите режим</b>" CONFIG_ALCO_MSG_SWITCH_DATA
#define CONFIG_ALCO_MSG_SWITCH_STOP             "🌡️ <b>Внимание! Отбор хвостов может быть завершен</b>" CONFIG_ALCO_MSG_SWITCH_DATA

// -----------------------------------------------------------------------------------------------------------------------
// ----------------------- Контроль температуры по времени с начала отбора (головы - тело - хвосты) ----------------------
// -----------------------------------------------------------------------------------------------------------------------

// Контроль температуры по времени с начала отбора (головы - тело - хвосты)
static uint8_t check_temp_on_time = 0;
static time_t time_start_heads = 0;
const uint8_t check_temp_on_time_count = 11;
const uint16_t check_temp_on_time_minutes[check_temp_on_time_count] = {30, 60, 90, 120, 150, 180, 210, 240, 270, 300, 600};
static float check_temp_on_time_temp_max[check_temp_on_time_count] = {81.0, 82.0, 83.0, 84.0, 85.0, 86.0, 87.0, 89.0, 91.0, 93.0, 99.0};
// Интервал отправки предупреждений о выходе температуры за границы
static time_t check_temp_on_time_temp_warning_last = 0;
static uint32_t check_temp_on_time_temp_warning_interval = 60;

#define CONFIG_ALCO_MSG_TIMECTRL_DATA           "\n\n<code>Температура:    %.3f °CnПрошло времени: %.2d:%.2d:%.2d</code>"
#define CONFIG_ALCO_MSG_TIMECTRL_MSG            "🌡️ <b>Внимание! Зафиксирована слишком высокая температура по времени с начала отбора</b>, возможно необходимо немного уменьшить нагрев" CONFIG_ALCO_MSG_TIMECTRL_DATA

#define CONFIG_ALCO_TIME_KEY                    "tot"
#define CONFIG_ALCO_TIME_TOPIC                  "temp-on-time"
#define CONFIG_ALCO_TIME_FRIENDLY               "Контроль по времени с начала отбора"
#define CONFIG_ALCO_TIME_ENABLED_KEY            "enabled"
#define CONFIG_ALCO_TIME_ENABLED_FRIENDLY       "Состояние"
#define CONFIG_ALCO_TIME_TEMP_MAX_KEY           "time_%d"
#define CONFIG_ALCO_TIME_TEMP_MAX_FRIENDLY      "Максимальная температура до %d минут"
#define CONFIG_ALCO_TIME_INTERVAL_KEY           "interval"
#define CONFIG_ALCO_TIME_INTERVAL_FRIENDLY      "Интервал отправки предупреждений о повышенной температуре"

// -----------------------------------------------------------------------------------------------------------------------
// --------------------------------------- Контроль быстрого изменения температуры ---------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

// Разрешить работу КБИТ
static uint8_t rapid_changes_enabled = 1;
// Пороговое изменение температуры за два соседних измерения (КБИТ)
static float rapid_changes_threshold = 1.0;
// Последнее и предыдущее значения температуры
static float rapid_changes_prev = NAN;
static float rapid_changes_curr = NAN;

#define CONFIG_RAPID_CHANGES_TIMER_US           30000000
#define CONFIG_RAPID_CHANGES_KEY                "rch"
#define CONFIG_RAPID_CHANGES_TOPIC              "rchanges"
#define CONFIG_RAPID_CHANGES_FRIENDLY           "Контроль быстрого изменения температуры"
#define CONFIG_RAPID_CHANGES_ENABLED_KEY        "enabled"
#define CONFIG_RAPID_CHANGES_ENABLED_FRIENDLY   "Состояние"
#define CONFIG_RAPID_CHANGES_THRESHOLD_KEY      "threshold"
#define CONFIG_RAPID_CHANGES_THRESHOLD_FRIENDLY "Минимальное значение"

#define CONFIG_RAPID_CHANGES_WARNING            "🆘 <b>Внимание! Обнаружено быстрое изменение температуры:</b> <b>%.1f</b> °С → <b>%.1f</b> °С, проверьте аппарат!"

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------- Задача --------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

void sensorsTaskInit();
bool sensorsTaskStart();
bool sensorsTaskSuspend();
bool sensorsTaskResume();

#endif // __SENSORS_H__