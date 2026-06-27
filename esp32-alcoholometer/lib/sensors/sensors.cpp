#include "sensors.h"
#include "strings.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <driver/gpio.h>
#include "project_config.h"
#include "def_consts.h"
#include "esp_timer.h"
#include "rLog.h"
#include "rTypes.h"
#include "rStrings.h"
#include "reStates.h"
#include "reEvents.h"
#include "reMqtt.h"
#include "reEsp32.h"
#include "reI2C.h"
#include "reWiFi.h"
#if CONFIG_TELEGRAM_ENABLE
#include "reTgSend.h"
#endif // CONFIG_TELEGRAM_ENABLE
#if CONFIG_DATASEND_ENABLE
#include "reDataSend.h"
#endif // CONFIG_DATASEND_ENABLE

static const char* logTAG  = "ALCO";
static const char* sensorsTaskName = "sensors";
static uint32_t _sensorsReadInterval = CONFIG_SENSORS_TASK_CYCLE;
static TaskHandle_t _sensorsTask;
static EventGroupHandle_t _sensorsFlags = nullptr;
static char* topic_alco = nullptr;

static paramsGroupHandle_t pgSensors;
static paramsGroupHandle_t pgIntervals;
static paramsGroupHandle_t pgAlco;

#define FLG_TIME_CHANGED            BIT0
#define FLG_SENSORS_STORE           BIT1
#define FLG_PARAM_CHANGED           BIT2
#define FLG_BUTTON_PRESSED          BIT3

#define FLG_BUTTON_MODE             BIT4
#define FLG_BUTTON_PLUS             BIT5
#define FLG_BUTTON_MINUS            BIT6
#define FLG_BUTTON_MENU             BIT7
#define FLG_BUTTON_LONG             BIT8

#define FLG_OTA_STARTED             BIT9
#define FLG_OTA_COMPLETED           BIT10

#define FLGS_CHANGES                (FLG_TIME_CHANGED | FLG_PARAM_CHANGED | FLG_BUTTON_PRESSED | FLG_OTA_STARTED)
#define FLGS_AUTORESET              (FLG_TIME_CHANGED | FLG_PARAM_CHANGED | FLG_BUTTON_PRESSED)
#define FLGS_BUTTONS                (FLG_BUTTON_MODE | FLG_BUTTON_PLUS | FLG_BUTTON_MINUS | FLG_BUTTON_MENU | FLG_BUTTON_LONG)


// -----------------------------------------------------------------------------------------------------------------------
// ----------------------------------------- Пересчет температуры в спиртуозность ----------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

double temp2strengths(double temp)
{
  if (!isnan(temp) && ((temp * 10) >= t2s_temps[0])) {
    uint16_t temp_val = temp * 10;
    // Поиск по таблице соответствия до предпоследнего элемента, иначе расчет "сломается"
    for (uint8_t i = 0; i < t2s_size - 1; i++) {
      if (temp_val < t2s_temps[i+1]) {
        double S1 = t2s_strengths[i];
        double S2 = t2s_strengths[i + 1];
        double T1 = t2s_temps[i];
        double T2 = t2s_temps[i + 1];
        double Sout = (S1 + (S2 - S1) * (temp_val - T1) / (T2 - T1)) / 100.0;
        rlog_d(logTAG, "Convert temperature -> strength: temp=%f, index=%d, strength=%f", temp, i, Sout);
        return Sout;
      };
    };
    // Это последний элемент таблицы соответствия
    double Sout = t2s_strengths[t2s_size - 1] / 100.0;
    rlog_d(logTAG, "Convert temperature -> strength: temp=%f, index=%d (last), strength=%f", temp, t2s_size - 1, Sout);
    return Sout;
  };
  return NAN;
}

// Подбор корректировки (смещения) температуры по таблице корректировок
int8_t getTempCorrectionIndex(double temp)
{
  if (!isnan(temp)) {
    for (uint8_t i = 0; i < temp_correction_count - 2; i++) {
      if (temp < (double)temp_correction_boundaries[i+1]) {
        return (int8_t)i;
      };
    };
  };
  return -1;
}

double getTempCorrection(double temp, bool smooth_transition)
{
  float correction = 0.0;
  if (!isnan(temp)) {
    // Поиск по таблице корректировок до предпоследнего элемента, иначе расчет "сломается"
    for (uint8_t i = 0; i < temp_correction_count - 2; i++) {
      if (temp < (double)temp_correction_boundaries[i+1]) {
        correction = smooth_transition ? 
          // Плавный переход к следующему диапазону
          temp_correction_offsets[i] 
            + (temp_correction_offsets[i+1] - temp_correction_offsets[i]) 
            * (temp - (double)temp_correction_boundaries[i])
            / (double)(temp_correction_boundaries[i+1] - temp_correction_boundaries[i]) 
          // Фиксированные значения в каждом диапазоне
          : temp_correction_offsets[i]; 
        rlog_d(logTAG, "Select temperature correction: temp=%f, index=%d, range=%d-%d, offset=%f", 
          temp, i, temp_correction_boundaries[i], temp_correction_boundaries[i+1], correction);
        return correction;
      };
    };
    // Это последний элемент таблицы корректировок
    correction = temp_correction_offsets[temp_correction_count - 2];
    rlog_d(logTAG, "Select temperature correction: temp=%f, index=%d (last), range=%d-%d, offset=%f", 
      temp, temp_correction_count - 2, temp_correction_boundaries[temp_correction_count - 2], temp_correction_boundaries[temp_correction_count - 1], correction);
  };
  return correction;
}

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------- Настройки -----------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

void sensorsInitParameters()
{
  rlog_i(logTAG, "Init parameters...");

  // Параметры сенсоров и интервалы публикации
  pgSensors = paramsRegisterGroup(nullptr, 
    CONFIG_SENSOR_PGROUP_ROOT_KEY, CONFIG_SENSOR_PGROUP_ROOT_TOPIC, CONFIG_SENSOR_PGROUP_ROOT_FRIENDLY);
  pgIntervals = paramsRegisterGroup(pgSensors, 
    CONFIG_SENSOR_PGROUP_INTERVALS_KEY, CONFIG_SENSOR_PGROUP_INTERVALS_TOPIC, CONFIG_SENSOR_PGROUP_INTERVALS_FRIENDLY);
  pgAlco = paramsRegisterGroup(nullptr, 
    CONFIG_ALCO_PGROUP_KEY, CONFIG_ALCO_PGROUP_TOPIC, CONFIG_ALCO_PGROUP_FRIENDLY);

  // Период публикации данных с сенсоров на MQTT
  paramsRegisterValue(OPT_KIND_PARAMETER, OPT_TYPE_U32, nullptr, pgIntervals,
    CONFIG_SENSOR_PARAM_INTERVAL_MQTT_KEY, CONFIG_SENSOR_PARAM_INTERVAL_MQTT_FRIENDLY,
    CONFIG_MQTT_PARAMS_QOS, (void*)&iMqttPubInterval);

  #if CONFIG_OPENMON_ENABLE
    paramsRegisterValue(OPT_KIND_PARAMETER, OPT_TYPE_U32, nullptr, pgIntervals,
      CONFIG_SENSOR_PARAM_INTERVAL_OPENMON_KEY, CONFIG_SENSOR_PARAM_INTERVAL_OPENMON_FRIENDLY,
      CONFIG_MQTT_PARAMS_QOS, (void*)&iOpenMonInterval);
  #endif // CONFIG_OPENMON_ENABLE

  #if CONFIG_NARODMON_ENABLE
    paramsRegisterValue(OPT_KIND_PARAMETER, OPT_TYPE_U32, nullptr, pgIntervals,
      CONFIG_SENSOR_PARAM_INTERVAL_NARODMON_KEY, CONFIG_SENSOR_PARAM_INTERVAL_NARODMON_FRIENDLY,
      CONFIG_MQTT_PARAMS_QOS, (void*)&iNarodMonInterval);
  #endif // CONFIG_NARODMON_ENABLE

  #if CONFIG_THINGSPEAK_ENABLE
    paramsRegisterValue(OPT_KIND_PARAMETER, OPT_TYPE_U32, nullptr, pgIntervals,
      CONFIG_SENSOR_PARAM_INTERVAL_THINGSPEAK_KEY, CONFIG_SENSOR_PARAM_INTERVAL_THINGSPEAK_FRIENDLY,
      CONFIG_MQTT_PARAMS_QOS, (void*)&iThingSpeakInterval);
  #endif // CONFIG_THINGSPEAK_ENABLE

  // Корректировка температуры
  paramsGroupHandle_t pgCorrTemp = paramsRegisterGroup(pgAlco, 
    CONFIG_ALCO_CORRECTION_KEY, CONFIG_ALCO_CORRECTION_TOPIC, CONFIG_ALCO_CORRECTION_FRIENDLY);
  if (pgCorrTemp) {
    // Точка кипения
    water_boiling_offset_changes = 0;
    water_boiling_offset_param = paramsRegisterValue(OPT_KIND_PARAMETER, OPT_TYPE_FLOAT, nullptr, pgCorrTemp,
      CONFIG_ALCO_WATER_BOILINFG_KEY, CONFIG_ALCO_WATER_BOILINFG_FRIENDLY,
      CONFIG_MQTT_PARAMS_QOS, (void*)&water_boiling_offset);
    // По диапазонам
    for (uint8_t i = 0; i < temp_correction_count - 1; i++) {
      temp_correction_changes[i] = 0;
      char * param_key = malloc_stringf(CONFIG_ALCO_RANGE_OFFSET_KEY, temp_correction_boundaries[i], temp_correction_boundaries[i+1]);
      char * param_friendly = malloc_stringf(CONFIG_ALCO_RANGE_OFFSET_FRIENDLY, temp_correction_boundaries[i], temp_correction_boundaries[i+1]);
      if (param_key && param_friendly) {
        temp_correction_params[i] = paramsRegisterValue(OPT_KIND_PARAMETER, OPT_TYPE_FLOAT, nullptr, pgCorrTemp,
          param_key, param_friendly,
          CONFIG_MQTT_PARAMS_QOS, (void*)&temp_correction_offsets[i]);
      };
    }
    // Плавный переход между дапазонами
    paramsRegisterValue(OPT_KIND_PARAMETER, OPT_TYPE_U8, nullptr, pgCorrTemp,
      CONFIG_ALCO_RANGE_SMOOTH_KEY, CONFIG_ALCO_RANGE_SMOOTH_FRIENDLY,
      CONFIG_MQTT_PARAMS_QOS, (void*)&temp_correction_smooth_transition);
  };

  // Контроль быстрого изменения температуры
  paramsGroupHandle_t pgRapidChanges = paramsRegisterGroup(pgAlco, 
    CONFIG_RAPID_CHANGES_KEY, CONFIG_RAPID_CHANGES_TOPIC, CONFIG_RAPID_CHANGES_FRIENDLY);
  if (pgRapidChanges) {
    paramsRegisterValue(OPT_KIND_PARAMETER, OPT_TYPE_U8, nullptr, pgRapidChanges,
      CONFIG_RAPID_CHANGES_ENABLED_KEY, CONFIG_RAPID_CHANGES_ENABLED_FRIENDLY,
      CONFIG_MQTT_PARAMS_QOS, (void*)&rapid_changes_enabled);
    paramsRegisterValue(OPT_KIND_PARAMETER, OPT_TYPE_FLOAT, nullptr, pgRapidChanges,
      CONFIG_RAPID_CHANGES_THRESHOLD_KEY, CONFIG_RAPID_CHANGES_THRESHOLD_FRIENDLY,
      CONFIG_MQTT_PARAMS_QOS, (void*)&rapid_changes_threshold);
  };

  // Границы режимов работы по температуре
  paramsGroupHandle_t pgTempStages = paramsRegisterGroup(pgAlco, 
    CONFIG_ALCO_STAGES_KEY, CONFIG_ALCO_STAGES_TOPIC, CONFIG_ALCO_STAGES_FRIENDLY);
  if (pgTempStages) {
    stage_temp_heads_param = paramsRegisterValue(OPT_KIND_PARAMETER, OPT_TYPE_FLOAT, nullptr, pgTempStages,
      CONFIG_ALCO_STAGE_HEADS_KEY, CONFIG_ALCO_STAGE_HEADS_FRIENDLY,
      CONFIG_MQTT_PARAMS_QOS, (void*)&stage_temp_heads);
    stage_temp_body_param = paramsRegisterValue(OPT_KIND_PARAMETER, OPT_TYPE_FLOAT, nullptr, pgTempStages,
      CONFIG_ALCO_STAGE_BODY_KEY, CONFIG_ALCO_STAGE_BODY_FRIENDLY,
      CONFIG_MQTT_PARAMS_QOS, (void*)&stage_temp_body);
    stage_temp_tails_param = paramsRegisterValue(OPT_KIND_PARAMETER, OPT_TYPE_FLOAT, nullptr, pgTempStages,
      CONFIG_ALCO_STAGE_TAILS_KEY, CONFIG_ALCO_STAGE_TAILS_FRIENDLY,
      CONFIG_MQTT_PARAMS_QOS, (void*)&stage_temp_tails);
    stage_temp_stop_param = paramsRegisterValue(OPT_KIND_PARAMETER, OPT_TYPE_FLOAT, nullptr, pgTempStages,
      CONFIG_ALCO_STAGE_STOP_KEY, CONFIG_ALCO_STAGE_STOP_FRIENDLY,
      CONFIG_MQTT_PARAMS_QOS, (void*)&stage_temp_stop);
    paramsRegisterValue(OPT_KIND_PARAMETER, OPT_TYPE_U32, nullptr, pgTempStages,
      CONFIG_ALCO_STAGE_INTERVAL_KEY, CONFIG_ALCO_STAGE_INTERVAL_FRIENDLY,
      CONFIG_MQTT_PARAMS_QOS, (void*)&stage_temp_warning_interval);
  };

  // Контроль температуры по времени с начала отбора
  paramsGroupHandle_t pgTempOnTime = paramsRegisterGroup(pgAlco, 
    CONFIG_ALCO_TIME_KEY, CONFIG_ALCO_TIME_TOPIC, CONFIG_ALCO_TIME_FRIENDLY);
  if (pgTempOnTime) {
    paramsSetLimitsI8(
      paramsRegisterValue(OPT_KIND_PARAMETER, OPT_TYPE_U8, nullptr, pgTempOnTime,
        CONFIG_ALCO_TIME_ENABLED_KEY, CONFIG_ALCO_TIME_ENABLED_FRIENDLY,
        CONFIG_MQTT_PARAMS_QOS, (void*)&check_temp_on_time),
      0, 1);
    paramsRegisterValue(OPT_KIND_PARAMETER, OPT_TYPE_U32, nullptr, pgTempOnTime,
      CONFIG_ALCO_TIME_INTERVAL_KEY, CONFIG_ALCO_TIME_INTERVAL_FRIENDLY,
      CONFIG_MQTT_PARAMS_QOS, (void*)&check_temp_on_time_temp_warning_interval);
    for (uint8_t i = 0; i < check_temp_on_time_count - 1; i++) {
      char * param_key = malloc_stringf(CONFIG_ALCO_TIME_TEMP_MAX_KEY, check_temp_on_time_minutes[i]);
      char * param_friendly = malloc_stringf(CONFIG_ALCO_TIME_TEMP_MAX_FRIENDLY, check_temp_on_time_minutes[i]);
      if (param_key && param_friendly) {
        paramsRegisterValue(OPT_KIND_PARAMETER, OPT_TYPE_FLOAT, nullptr, pgTempOnTime,
          param_key, param_friendly,
          CONFIG_MQTT_PARAMS_QOS, (void*)&check_temp_on_time_temp_max[i]);
      };
    };
  };

  // Уведомления
  paramsSetLimitsU8(
    paramsRegisterValue(OPT_KIND_PARAMETER, OPT_TYPE_U8, nullptr, pgAlco, 
      CONFIG_NOTIFY_TG_KEY, CONFIG_NOTIFY_TG_FRIENDLY, 
      CONFIG_MQTT_PARAMS_QOS, (void*)&notify_tg),
    (uint8_t)NOTIFY_OFF, (uint8_t)NOTIFY_SOUND);
  paramsSetLimitsU8(
    paramsRegisterValue(OPT_KIND_PARAMETER, OPT_TYPE_U8, nullptr, pgAlco, 
      CONFIG_NOTIFY_BUZZER_KEY, CONFIG_NOTIFY_BUZZER_FRIENDLY, 
      CONFIG_MQTT_PARAMS_QOS, (void*)&notify_buzzer),
    0x00, 0x01);
}

bool sensorsSaveParameters()
{
  bool stored = false;
  if (water_boiling_offset_changes > 0) {
    paramsValueStore(water_boiling_offset_param, true);
    water_boiling_offset_changes = 0;
    stored = true;
  };
  for (uint8_t i = 0; i < temp_correction_count - 1; i++) {
    if (temp_correction_changes[i] > 0) {
      paramsValueStore(temp_correction_params[i], true);
      temp_correction_changes[i] = 0;
      stored = true;
    };
  };
  return stored;
}

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------- Зуммер --------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

#if defined(CONFIG_GPIO_BUZZER_ACTIVE)

static ledQueue_t buzzer = nullptr;

void buzzerInit()
{
  buzzer = ledTaskCreate(CONFIG_GPIO_BUZZER_ACTIVE, 0x01, false, "buzzer", CONFIG_LED_TASK_STACK_SIZE, nullptr);
}

void notifyBuzzerKeyPress() 
{
  if (notify_buzzer) {
    ledTaskSend(buzzer, CONFIG_BEEP_KEY_PRESS);
  };
};

void notifyBuzzerKeySave() 
{
  if (notify_buzzer) {
    ledTaskSend(buzzer, CONFIG_BEEP_KEY_SAVE);
  };
};

void notifyBuzzerKeyError() 
{
  if (notify_buzzer) {
    ledTaskSend(buzzer, CONFIG_BEEP_KEY_ERROR);
  };
};

void notifyBuzzerModeChange() 
{
  if (notify_buzzer) {
    ledTaskSend(buzzer, CONFIG_BEEP_MODE_CHANGE);
  };
};

void notifyBuzzerRapidChanges() 
{
  if (notify_buzzer) {
    ledTaskSend(buzzer, CONFIG_BEEP_RAPID_CHANGES);
  };
};

void notifyBuzzerTempExcess() 
{
  if (notify_buzzer) {
    ledTaskSend(buzzer, CONFIG_BEEP_TEMP_EXCESS);
  };
};

#else

void buzzerInit()
{
};

void notifyBuzzerKeyPress() 
{
  if (notify_buzzer) {
    beepTaskSend(CONFIG_BEEP_KEY_PRESS);
  };
};

void notifyBuzzerKeySave() 
{
  if (notify_buzzer) {
    beepTaskSend(CONFIG_BEEP_KEY_SAVE);
  };
};

void notifyBuzzerKeyError() 
{
  if (notify_buzzer) {
    beepTaskSend(CONFIG_BEEP_KEY_ERROR);
  };
};

void notifyBuzzerModeChange() 
{
  if (notify_buzzer) {
    beepTaskSend(CONFIG_BEEP_MODE_CHANGE);
  };
};

void notifyBuzzerRapidChanges() 
{
  if (notify_buzzer) {
    beepTaskSend(CONFIG_BEEP_RAPID_CHANGES);
  };
};

void notifyBuzzerTempExcess() 
{
  if (notify_buzzer) {
    beepTaskSend(CONFIG_BEEP_TEMP_EXCESS);
  };
};

#endif // CONFIG_GPIO_BUZZER_ACTIVE

// -----------------------------------------------------------------------------------------------------------------------
// ----------------------------------------------------- Уведомления -----------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

void notifyModeChange()
{
  if (notify_tg != NOTIFY_OFF) {
    switch (stage) {
      case STAGE_HEATING:
        tgSend(MK_MAIN, MP_ORDINARY, notify_tg == NOTIFY_SOUND, CONFIG_TELEGRAM_DEVICE, 
          CONFIG_ALCO_MSG_MODE_HEATING);
        break;
      case STAGE_HEADS:
        tgSend(MK_MAIN, MP_ORDINARY, notify_tg == NOTIFY_SOUND, CONFIG_TELEGRAM_DEVICE, 
          CONFIG_ALCO_MSG_MODE_HEADS, temp_pressure_compensated + temp_correction, alcohol_content, duration_h, duration_m, duration_s);
        break;
      case STAGE_BODY:
        tgSend(MK_MAIN, MP_ORDINARY, notify_tg == NOTIFY_SOUND, CONFIG_TELEGRAM_DEVICE, 
          CONFIG_ALCO_MSG_MODE_BODY, temp_pressure_compensated + temp_correction, alcohol_content, duration_h, duration_m, duration_s);
        break;
      case STAGE_TAILS:
        tgSend(MK_MAIN, MP_ORDINARY, notify_tg == NOTIFY_SOUND, CONFIG_TELEGRAM_DEVICE, 
          CONFIG_ALCO_MSG_MODE_TAILS, temp_pressure_compensated + temp_correction, alcohol_content, duration_h, duration_m, duration_s);
        break;
      default:
        tgSend(MK_MAIN, MP_ORDINARY, notify_tg == NOTIFY_SOUND, CONFIG_TELEGRAM_DEVICE, 
          CONFIG_ALCO_MSG_MODE_STOP, temp_pressure_compensated + temp_correction, alcohol_content, duration_h, duration_m, duration_s);
        break;
    };
  };
}

void notifyTempExcess()
{
  if (notify_tg != NOTIFY_OFF) {
    switch (stage) {
      case STAGE_HEATING:
        tgSend(MK_MAIN, MP_ORDINARY, notify_tg == NOTIFY_SOUND, CONFIG_TELEGRAM_DEVICE, 
          CONFIG_ALCO_MSG_SWITCH_HEADS, temp_pressure_compensated + temp_correction, alcohol_content, duration_h, duration_m, duration_s);
        break;
      case STAGE_HEADS:
        tgSend(MK_MAIN, MP_ORDINARY, notify_tg == NOTIFY_SOUND, CONFIG_TELEGRAM_DEVICE, 
          CONFIG_ALCO_MSG_SWITCH_BODY, temp_pressure_compensated + temp_correction, alcohol_content, duration_h, duration_m, duration_s);
        break;
      case STAGE_BODY:
        tgSend(MK_MAIN, MP_ORDINARY, notify_tg == NOTIFY_SOUND, CONFIG_TELEGRAM_DEVICE, 
          CONFIG_ALCO_MSG_SWITCH_TAILS, temp_pressure_compensated + temp_correction, alcohol_content, duration_h, duration_m, duration_s);
        break;
      case STAGE_TAILS:
        tgSend(MK_MAIN, MP_ORDINARY, notify_tg == NOTIFY_SOUND, CONFIG_TELEGRAM_DEVICE, 
          CONFIG_ALCO_MSG_SWITCH_STOP, temp_pressure_compensated + temp_correction, alcohol_content, duration_h, duration_m, duration_s);
        break;
      default:
        break;
    };
  };
}

// Контроль границ этапов по температуре
void sensorsCheckTempRanges()
{
  bool temp_excess = false;
  double temp = temp_pressure_compensated + temp_correction;
  switch (stage) {
    case STAGE_HEATING:
      temp_excess = temp >= stage_temp_heads;
      break;
    case STAGE_HEADS:
      temp_excess = temp >= stage_temp_body;
      break;
    case STAGE_BODY:
      temp_excess = temp >= stage_temp_tails;
      break;
    case STAGE_TAILS:
      temp_excess = temp >= stage_temp_stop;
      break;
    default:
      temp_excess = false;
      break;
  };

  if (temp_excess && (time(nullptr) - stage_temp_warning_last) >= stage_temp_warning_interval) {
    stage_temp_warning_last = time(nullptr);

    notifyBuzzerTempExcess();
    notifyTempExcess();
  };
}

// Контроль температуры по времени с начала отбора (головы - тело - хвосты)
void sensorsCheckTempOnTime()
{
  time_t now = time(nullptr);
  // Поиск по таблице времени в минутах
  uint8_t i = 0;
  time_t duration_min = (now - time_start_heads) / 60;
  while ((i < check_temp_on_time_count) && (duration_min <= check_temp_on_time_minutes[i])) {
    i++;
  };
  // Если индекс записи найден
  if (i < check_temp_on_time_count) {
    // Проверяем конечную температуру
    if ((temp_pressure_compensated + temp_correction) > check_temp_on_time_temp_max[i]) {
      // Проверяем время отправки последнего уведомленния
      if ((now - check_temp_on_time_temp_warning_last) > check_temp_on_time_temp_warning_interval) {
        check_temp_on_time_temp_warning_last = now;
        // Отправляем сообщение
        if (notify_tg != NOTIFY_OFF) {
          uint16_t h = 0;
          uint16_t m = 0;
          uint16_t s = 0;
          if (time_start_heads > 0) {
            time_t duration_heads = time(nullptr) - time_start_heads;
            h = duration_heads / 3600;
            m = duration_heads % 3600 / 60;
            s = duration_heads % 3600 % 60;
          };
          tgSend(MK_MAIN, MP_ORDINARY, notify_tg == NOTIFY_SOUND, CONFIG_TELEGRAM_DEVICE, 
            CONFIG_ALCO_MSG_TIMECTRL_MSG, temp_pressure_compensated + temp_correction, h, m, s);
        };
      };
    };
  };
}

// -----------------------------------------------------------------------------------------------------------------------
// --------------------------------------- Контроль быстрого изменения температуры ---------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

static esp_timer_handle_t tmrRapidChanges = nullptr;

void tmrRapidChangesTimeout(void* arg)
{
  esp_timer_delete(tmrRapidChanges);
  tmrRapidChanges = nullptr;
}

void sensorsMessageRapidChanges(float prev_temp, float last_temp)
{
  if (tmrRapidChanges == nullptr) {
    esp_timer_create_args_t timer_cfg;
    memset(&timer_cfg, 0, sizeof(timer_cfg));
    timer_cfg.name = "rpc_timer";
    timer_cfg.callback = tmrRapidChangesTimeout;
    timer_cfg.skip_unhandled_events = false;
    RE_OK_CHECK(esp_timer_create(&timer_cfg, &tmrRapidChanges), return);
    esp_timer_start_once(tmrRapidChanges, CONFIG_RAPID_CHANGES_TIMER_US);

    if (notify_tg != NOTIFY_OFF) {
      tgSend(MK_MAIN, MP_ORDINARY, true, CONFIG_TELEGRAM_DEVICE, CONFIG_RAPID_CHANGES_WARNING, prev_temp, last_temp);
    };
  };
}

// -----------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------- Сенсоры ------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

static bool sensorsPublish(rSensor *sensor, char* topic, char* payload, const bool free_topic, const bool free_payload)
{
  return mqttPublish(topic, payload, CONFIG_MQTT_SENSORS_QOS, CONFIG_MQTT_SENSORS_RETAINED, free_topic, free_payload);
}

static void sensorsMqttTopicsCreate(bool primary)
{
  sensorColumn.topicsCreate(primary);
  sensorPressure.topicsCreate(primary);
  if (topic_alco) free(topic_alco);
  topic_alco = mqttGetTopicDevice1(primary, false, ALCOHOL_JSON_TOPIC);
}

static void sensorsMqttTopicsFree()
{
  sensorColumn.topicsFree();
  sensorPressure.topicsFree();
  if (topic_alco) free(topic_alco);
  topic_alco = nullptr;
}

static void sensorsStoreData()
{
  rlog_i(logTAG, "Store sensors data");

  sensorColumn.nvsStoreExtremums(SENSOR_COLUMN_KEY); 
  sensorPressure.nvsStoreExtremums(SENSOR_PRESSURE_KEY); 
}

static void sensorsInitSensors()
{
  rlog_i(logTAG, "Init sensors...");

  // DS18B20 : основной датчик температуры паров спирта в дефлегматоре (до конденсатора)
  static rTemperatureItem siColumnTemp(nullptr, CONFIG_SENSOR_TEMP_NAME, CONFIG_FORMAT_TEMP_UNIT,
    SENSOR_COLUMN_FILTER_MODE, SENSOR_COLUMN_FILTER_SIZE, 
    CONFIG_FORMAT_TEMP_VALUE, CONFIG_FORMAT_TEMP_STRING,
    #if CONFIG_SENSOR_TIMESTAMP_ENABLE
      CONFIG_FORMAT_TIMESTAMP_L, 
    #endif // CONFIG_SENSOR_TIMESTAMP_ENABLE
    #if CONFIG_SENSOR_TIMESTRING_ENABLE  
      CONFIG_FORMAT_TIMESTAMP_S, CONFIG_FORMAT_TSVALUE
    #endif // CONFIG_SENSOR_TIMESTRING_ENABLE
  );
  sensorColumn.initExtItems(SENSOR_COLUMN_NAME, SENSOR_COLUMN_TOPIC, false,
    (gpio_num_t)SENSOR_COLUMN_PIN, SENSOR_COLUMN_ADDR, SENSOR_COLUMN_INDEX, SENSOR_COLUMN_RESOLUTION, SENSOR_COLUMN_SAVE_SP, 
    &siColumnTemp, 
    SENSORS_MINIMAL_READ_INTERVAL, SENSOR_COLUMN_ERRORS_LIMIT, nullptr, sensorsPublish);
  sensorColumn.registerParameters(pgSensors, SENSOR_COLUMN_KEY, SENSOR_COLUMN_TOPIC, SENSOR_COLUMN_NAME);
  sensorColumn.nvsRestoreExtremums(SENSOR_COLUMN_KEY);

  // BME280 : измерение давления для компенсации влияния колебаний атмосферного давления на температуру кипения воды

  static rPressureItem siPressurePres(nullptr, CONFIG_SENSOR_PRESSURE_NAME, CONFIG_FORMAT_PRESSURE_UNIT,
    SENSOR_PRESSURE_FILTER_MODE, SENSOR_PRESSURE_FILTER_SIZE, 
    CONFIG_FORMAT_PRESSURE_VALUE, CONFIG_FORMAT_PRESSURE_STRING,
    #if CONFIG_SENSOR_TIMESTAMP_ENABLE
      CONFIG_FORMAT_TIMESTAMP_L, 
    #endif // CONFIG_SENSOR_TIMESTAMP_ENABLE
    #if CONFIG_SENSOR_TIMESTRING_ENABLE  
      CONFIG_FORMAT_TIMESTAMP_S, CONFIG_FORMAT_TSVALUE
    #endif // CONFIG_SENSOR_TIMESTRING_ENABLE
  );
  static rTemperatureItem siPressureTemp(nullptr, CONFIG_SENSOR_TEMP_NAME, CONFIG_FORMAT_TEMP_UNIT,
    SENSOR_PRESSURE_FILTER_MODE, SENSOR_PRESSURE_FILTER_SIZE, 
    CONFIG_FORMAT_TEMP_VALUE, CONFIG_FORMAT_TEMP_STRING,
    #if CONFIG_SENSOR_TIMESTAMP_ENABLE
      CONFIG_FORMAT_TIMESTAMP_L, 
    #endif // CONFIG_SENSOR_TIMESTAMP_ENABLE
    #if CONFIG_SENSOR_TIMESTRING_ENABLE  
      CONFIG_FORMAT_TIMESTAMP_S, CONFIG_FORMAT_TSVALUE
    #endif // CONFIG_SENSOR_TIMESTRING_ENABLE
  );
  sensorPressure.initExtItems(SENSOR_PRESSURE_NAME, SENSOR_PRESSURE_TOPIC, false,
    SENSOR_PRESSURE_BUS, SENSOR_PRESSURE_ADDRESS, SENSOR_PRESSURE_MODE, SENSOR_PRESSURE_ODR, SENSOR_PRESSURE_FILTER, 
    SENSOR_PRESSURE_OSM, SENSOR_PRESSURE_OSM, 
    &siPressurePres, &siPressureTemp, 
    SENSORS_MINIMAL_READ_INTERVAL, SENSOR_PRESSURE_ERRORS_LIMIT, nullptr, sensorsPublish);
  sensorPressure.registerParameters(pgSensors, SENSOR_PRESSURE_KEY, SENSOR_PRESSURE_TOPIC, SENSOR_PRESSURE_NAME);
  sensorPressure.nvsRestoreExtremums(SENSOR_PRESSURE_KEY);

  xEventGroupClearBits(_sensorsFlags, FLG_SENSORS_STORE);
  espRegisterShutdownHandler(sensorsStoreData); // #2
}

static void sensorsReadSensors()
{
  sensorColumn.readData();
  if (sensorColumn.getStatus() == SENSOR_STATUS_OK) {
    rlog_i(SENSOR_COLUMN_NAME, "Values raw: %.3f °С | out: %.3f °С | min: %.3f °С | max: %.3f °С", 
      sensorColumn.getValue(false).rawValue, 
      sensorColumn.getValue(false).filteredValue,
      sensorColumn.getExtremumsDaily(false).minValue.filteredValue,
      sensorColumn.getExtremumsDaily(false).maxValue.filteredValue);
    // Запоминаем измеренные значения
    rapid_changes_curr = sensorColumn.getValue(false).rawValue;
    temp_measured = sensorColumn.getValue(false).filteredValue;
  };

  sensorPressure.readData();
  if (sensorPressure.getStatus() == SENSOR_STATUS_OK) {
    rlog_i(SENSOR_PRESSURE_NAME, "Values raw: %.2f °С / %.0f Pa | out: %.2f °С / %.2f mmHg | min: %.2f °С / %.2f mmHg | max: %.2f °С / %.2f mmHg", 
      sensorPressure.getValue2(false).rawValue, sensorPressure.getValue1(false).rawValue, 
      sensorPressure.getValue2(false).filteredValue, sensorPressure.getValue1(false).filteredValue, 
      sensorPressure.getExtremumsDaily2(false).minValue.filteredValue, sensorPressure.getExtremumsDaily1(false).minValue.filteredValue, 
      sensorPressure.getExtremumsDaily2(false).maxValue.filteredValue, sensorPressure.getExtremumsDaily1(false).maxValue.filteredValue);
    // Пересчитываем температуру по датчику давления и добавляем смещение температуры кипения при 760 мм.рт.ст.
    if (!isnan(temp_measured)) {
      temp_pressure_compensated = temp_measured + (759.94 - sensorPressure.getValue1(false).filteredValue) * 0.0385 + water_boiling_offset;
    };
  } else {
    // Кое-какая работа в случае выхода BME280 из строя
    temp_pressure_compensated = temp_measured;
  };

  // Контроль быстрого изменения температуры
  if (!isnan(rapid_changes_curr)) {
    if (rapid_changes_enabled && !isnan(rapid_changes_prev) && ((rapid_changes_curr - rapid_changes_prev) >= rapid_changes_threshold)) {
      sensorsMessageRapidChanges(rapid_changes_prev, rapid_changes_curr);
      notifyBuzzerRapidChanges();
    };
    rapid_changes_prev = rapid_changes_curr;
  };
}

static void sensorsCalcAlcohol()
{
  // Вычисляем спирт в парах
  temp_correction = getTempCorrection(temp_pressure_compensated, temp_correction_smooth_transition);
  alcohol_content = temp2strengths(temp_pressure_compensated + temp_correction);
  
  rlog_d(logTAG, "Calculated data: temp_measured = %f, water_boiling_offset = %f, temp_compensated = %f, temp_correction=%f, alcohol_content=%f",
    temp_measured, water_boiling_offset, temp_pressure_compensated, temp_correction, alcohol_content);

  // Контроль максимальной температуры этапа
  sensorsCheckTempRanges();

  // Контроль максимальной температуры по времени с начала отбора голов
  if ((check_temp_on_time > 0) && (stage > STAGE_HEATING)) {
    sensorsCheckTempOnTime();
  };
}

static void sensorsPublishAlcohol()
{
  if (topic_alco && (sensorColumn.getStatus() == SENSOR_STATUS_OK) && (sensorPressure.getStatus() == SENSOR_STATUS_OK)) {
    char *data = nullptr;
    if (!isnan(alcohol_content)) {
      data = malloc_stringf("{\"stage\":%d,\"alcohol\":%.2f,\"temperature\":{\"measured\":%.3f,\"compensated\":%.3f,\"correction\":%.3f,\"corrected\":%.3f},\"duration\":\"%.2d:%.2d:%.2d\",\"display\":\"%.2f%%" CONFIG_JSON_CHAR_EOL "%.2f°С\"}", 
        (uint8_t)stage, alcohol_content, temp_measured, temp_pressure_compensated, temp_correction, temp_pressure_compensated + temp_correction,
        duration_h, duration_m, duration_s,
        alcohol_content, temp_pressure_compensated + temp_correction);
    } else {
      data = malloc_stringf("{\"stage\":%d,\"alcohol\":0.0,\"temperature\":{\"measured\":%.3f,\"compensated\":%.3f,\"correction\":%.3f,\"corrected\":%.3f},\"duration\":\"%.2d:%.2d:%.2d\",\"display\":\"---%%" CONFIG_JSON_CHAR_EOL "%.2f°С\"}", 
        (uint8_t)stage, temp_measured, temp_pressure_compensated, temp_correction, temp_pressure_compensated + temp_correction,
        duration_h, duration_m, duration_s,
        temp_pressure_compensated + temp_correction);
    };
    if (data) {
      mqttPublish(topic_alco, data, ALCOHOL_JSON_QOS, ALCOHOL_JSON_RETAINED, false, true);
    };
  };
}

static void sensorsPublishSensors()
{
  sensorColumn.publishData(false);
  sensorPressure.publishData(false);
  sensorsPublishAlcohol();
}

static char* sensorsGetOpenMonitoring()
{
  char* ret = nullptr;
  // 01. DS18 raw:FLOAT:~:ON:OFF
  if (sensorColumn.getStatus() == SENSOR_STATUS_OK) {
    ret = concat_strings_div(ret, 
      malloc_stringf("p1=%.3f", sensorColumn.getValue(false).rawValue),
      "&");
  };
  // 02. DS18 filtered:FLOAT:~:ON:OFF
  if (!isnan(temp_measured)) {
    ret = concat_strings_div(ret, 
      malloc_stringf("p2=%.3f", temp_measured),
      "&");
  };
  // 03. DS18 compensated:FLOAT:~:ON:OFF
  if (!isnan(temp_pressure_compensated)) {
    ret = concat_strings_div(ret, 
      malloc_stringf("p3=%.3f", temp_pressure_compensated),
      "&");
  };
  // 04. DS18 corrected:FLOAT:~:ON:OFF
  if (!isnan(temp_pressure_compensated + temp_correction)) {
    ret = concat_strings_div(ret, 
      malloc_stringf("p4=%.3f", temp_pressure_compensated + temp_correction),
      "&");
  };
  // 05. BME280 press:FLOAT:~:OFF:OFF
  // 06. BME280 temp:FLOAT:~:OFF:OFF
  // 07. BME280 humd:FLOAT:~:OFF:OFF
  if (sensorPressure.getStatus() == SENSOR_STATUS_OK) {
    ret = concat_strings_div(ret, 
      malloc_stringf("p5=%.3f&p6=%.3f", 
        sensorPressure.getValue1(false).filteredValue, 
        sensorPressure.getValue2(false).filteredValue),
      "&");
  };
  // 08. Alcohol:FLOAT:~:ON:OFF
  if (!isnan(alcohol_content)) {
    ret = concat_strings_div(ret, 
      malloc_stringf("p8=%.2f", alcohol_content),
      "&");
  };
  // 09. Stage:INT:~:OFF:OFF
  ret = concat_strings_div(ret, 
    malloc_stringf("p9=%d", (uint8_t)stage),
    "&");
  return ret;
}

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------- Дисплей -------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

static void lcdInit()
{
  lcd.init();
  lcd.clear();
  lcd.setBacklight(true);
  lcd.printpos(0, 0, CONFIG_LCD_MSG_LOADING);
  lcd.printpos(0, 1, APP_VERSION);
  // vTaskDelay(pdMS_TO_TICKS(5000));
}

static void lcdClear()
{
  lcd.clear();
}

static void lcdShowTemp(const temp_show_t mode, bool smooth_transition)
{
  lcd.setCursor(0, 1);
  if (isnan(temp_pressure_compensated)) {
    lcd.printstr("NO SENSOR ");
  } else {
    switch (mode) {
      // Измеренное значение
      case TM_RAW:
        if (temp_measured >= 100.0) {
          lcd.printf("%05.1f°C R ", temp_measured);
        } else {
          lcd.printf("%05.2f°C R ", temp_measured);
        };
        break;

      // Измеренное значение, скорректированное по давлению
      case TM_COMPENSATED:
        if (temp_pressure_compensated >= 100.0) {
          lcd.printf("%05.1f°C P ", temp_pressure_compensated);
        } else {
          lcd.printf("%05.2f°C P ", temp_pressure_compensated);
        };
        break;

      // Полностью скорректированное значение
      case TM_CORRECTED:
        if ((temp_pressure_compensated + temp_correction) >= 100.0) {
          lcd.printf("%05.1f°C N ", temp_pressure_compensated + temp_correction);
        } else {
          lcd.printf("%05.2f°C N ", temp_pressure_compensated + temp_correction);
        };
        break;

      // Расширенный вид
      default:
        double temp_offset = getTempCorrection(temp_pressure_compensated, smooth_transition);
        if (temp_pressure_compensated >= 100.0) {
          lcd.printf("%05.1f%+5.2f ", temp_pressure_compensated, temp_offset);
        } else {
          lcd.printf("%05.2f%+5.2f ", temp_pressure_compensated, temp_offset);
        };
        break;
    };
  };
}

static void lcdShowStep(bool changes)
{
  lcd.setCursor(11, 0);
  lcd.write(changes ? '*' : ' ');
  lcd.printf("%04.2f", (double)step_changes / 100.0);
}

static void lcdShowAlco()
{
  lcd.setCursor(10, 1);
  lcd.write('~');
  if (isnan(alcohol_content)) {
    lcd.printstr("--.-");
  } else {
    lcd.printn(11, 1, 4, "%.1f", alcohol_content);
  };
  lcd.write('%');
}

static void lcdUpdate()
{
  // --------------------------------------------------------------------------------
  // Считаем общую длительность процесса, если включен режим отбора
  // --------------------------------------------------------------------------------
  time_t curr_time = time(nullptr);
  if ((stage != STAGE_NONE) && (time_start > 0) && (curr_time > time_start)) {
    duration_sec = curr_time - time_start;
    duration_h = duration_sec / 3600;
    duration_m = duration_sec % 3600 / 60;
    duration_s = duration_sec % 3600 % 60;
  } else {
    duration_sec = 0;
    duration_h = 0;
    duration_m = 0;
    duration_s = 0;
  };

  // --------------------------------------------------------------------------------
  // Основной режим
  // --------------------------------------------------------------------------------
  if (mode == MODE_NORMAL) {
    // Отображаем текущее время
    lcd.setCursor(0, 0);
    struct tm time_data;
    localtime_r(&curr_time, &time_data);
    lcd.printf("%.2d:%.2d", time_data.tm_hour, time_data.tm_min);
    lcd.write(' ');

    // Отображаем стадию процесса
    lcd.setCursor(6, 0);
    lcd.write(stage_tags[stage]);
    lcd.write(' ');

    // Отображаем время процесса
    if (duration_sec > 0) {
      lcd.printf("%.2d:%.2d:%.2d", duration_h, duration_m, duration_s);
    } else {
      lcd.printstr("--:--:--");
    };

    // Отображаем температуру
    lcdShowTemp(temp_show, temp_correction_smooth_transition);

    // Отображаем содержание спирта
    lcdShowAlco();
  } 

  // --------------------------------------------------------------------------------
  // Режим установки корректировки температуры в текущем диапазоне
  // --------------------------------------------------------------------------------
  else if (mode == MODE_CORRECTION) {
    // Получаем индекс темературного диапазона
    int8_t index = getTempCorrectionIndex(temp_pressure_compensated);

    // Отображаем режим
    lcd.setCursor(0, 0);
    lcd.printstr("К-");

    // Отображаем диапазон
    if (index == -1) {
      lcd.printstr("??   -   ° ");
    } else {
      lcd.printf("%02d %02d-%02d° ", index, temp_correction_boundaries[index], temp_correction_boundaries[index+1]);
    };

    // Отображаем шаг изменения
    lcdShowStep(temp_correction_changes[index] > 0);

    // Отображаем температуру в расширенном виде без плавного перехода
    lcdShowTemp(TM_EXTENDED, false);

    // Отображаем содержание спирта
    lcdShowAlco();
  }

  // --------------------------------------------------------------------------------
  // Режим установки температуры кипения чистой воды
  // --------------------------------------------------------------------------------
  else if (mode == MODE_WBOILING) {
    // Отображаем режим
    lcd.setCursor(0, 0);
    lcd.printstr("КОРР 100°C");

    // Отображаем шаг изменения
    lcdShowStep(water_boiling_offset_changes > 0);

    // Отображаем смещение температуры 100 градусов
    lcd.setCursor(0, 1);
    lcd.printf("%+.2f ", water_boiling_offset);

    // Отображаем целевую температуру (учитываем, что символ ° занимает 2 байта)
    lcd.printn(7, 1, 10, "~%6.2f°C", temp_pressure_compensated);
 };
}

static void lcdShowOTA()
{
  lcdClear();
  lcd.printpos(0, 0, "OTA software");
  lcd.printpos(0, 1, "downloading...");
}

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------- Кнопки --------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

void gpioInit()
{
  btnMode.setEventGroup(_sensorsFlags, FLG_BUTTON_PRESSED | FLG_BUTTON_MODE, FLG_BUTTON_PRESSED | FLG_BUTTON_MODE | FLG_BUTTON_LONG);
  btnMenu.setEventGroup(_sensorsFlags, FLG_BUTTON_PRESSED | FLG_BUTTON_MENU, FLG_BUTTON_PRESSED | FLG_BUTTON_MENU | FLG_BUTTON_LONG);
  btnPlus.setEventGroup(_sensorsFlags, FLG_BUTTON_PRESSED | FLG_BUTTON_PLUS, FLG_BUTTON_PRESSED | FLG_BUTTON_PLUS | FLG_BUTTON_LONG);
  btnMinus.setEventGroup(_sensorsFlags, FLG_BUTTON_PRESSED | FLG_BUTTON_MINUS, FLG_BUTTON_PRESSED | FLG_BUTTON_MINUS | FLG_BUTTON_LONG);

  btnMode.initGPIO();
  btnMenu.initGPIO();
  btnPlus.initGPIO();
  btnMinus.initGPIO();
}

/*************************************************************************
 * Логика работы кнопок:
 * 
 * [MENU] (короткое) в любом режиме переключает меню ОСНОВНОЙ - КОРРЕКТИРОВКА - ТОЧКА КИПЕНИЯ и т.д.
 * 
 * 1. В основном режиме:
 * 1.1. [MODE] переключает режим НАГРЕВ - ГОЛОВЫ - ТЕЛО - ХВОСТЫ - СТОП
 * 1.2. [+] и [-] изменяет режим отображения температуры (измерянная/корр.по давлению/корр.полная)
 * 
 * 2. В режиме корректировки
 * 2.1 [+] и [-] "короткое" изменяет значение смещения для текущего диапазона
 * 2.2 [+] и [-] "длинное" изменяет шаг изменения значения 0.01 / 0.10 / 1.00
 * 2.3 [MODE] "короткое" записывает значение смещения в память
 * 
 * 3. В режиме установки точки кипения
 * 3.1 [+] и [-] "короткое" изменяет значение смещения температуры
 * 3.2 [+] и [-] "длинное" изменяет шаг изменения значения 0.01 / 0.10 / 1.00
 * 3.3 [MODE] "короткое" записывает текущее значение смещения в память
 * 3.4 [MODE] "длинное" автоматически вычисляет и записывает смещение так, чтобы получилось 100 градусов] 
 * 
 *************************************************************************/

void gpioButtonsProcess(EventBits_t bits)
{
  // --------------------------------------------------------------------------------
  // Меню
  // --------------------------------------------------------------------------------
  if ((bits & FLG_BUTTON_MENU) > 0) {
    notifyBuzzerKeyPress();
    // Сохраняем измененные параметры
    sensorsSaveParameters();
    // Переключаем меню
    switch (mode) {
      case MODE_NORMAL:
        mode = MODE_CORRECTION;
        break;
      case MODE_CORRECTION:
        mode = MODE_WBOILING;
        break;
      default:
        mode = MODE_NORMAL;
        break;
    };
    // Очищаем экран во избежание артефактов
    lcdClear();
    // Выходим
    return;
  };

  // --------------------------------------------------------------------------------
  // Основной режим
  // --------------------------------------------------------------------------------
  if (mode == MODE_NORMAL) {
    // Переключаем этап перегонки
    if ((bits & FLG_BUTTON_MODE) > 0) {
      switch (stage) {
        case STAGE_NONE:
          time_start = time(nullptr);
          stage = STAGE_HEATING;
          break;
        case STAGE_HEATING:
          stage = STAGE_HEADS;
          break;
        case STAGE_HEADS:
          time_start_heads = time(nullptr);
          stage = STAGE_BODY;
          break;
        case STAGE_BODY:
          stage = STAGE_TAILS;
          break;
        default:
          stage = STAGE_NONE;
          break;
      };
      stage_temp_warning_last = 0;
      notifyBuzzerModeChange();
      notifyModeChange();
    } 
    // Переключаем отображение температуры
    else if ((bits & FLG_BUTTON_PLUS) > 0) {
      notifyBuzzerKeyPress();
      switch (temp_show) {
        case TM_RAW:
          temp_show = TM_COMPENSATED;
          break;
        case TM_COMPENSATED:
          temp_show = TM_CORRECTED;
          break;
        case TM_CORRECTED:
          temp_show = TM_EXTENDED;
          break;
        default:
          temp_show = TM_RAW;
          break;
      };
    } 
    else if ((bits & FLG_BUTTON_MINUS) > 0) {
      notifyBuzzerKeyPress();
      switch (temp_show) {
        case TM_EXTENDED:
          temp_show = TM_CORRECTED;
          break;
        case TM_CORRECTED:
          temp_show = TM_COMPENSATED;
          break;
        case TM_COMPENSATED:
          temp_show = TM_RAW;
          break;
        default:
          temp_show = TM_EXTENDED;
          break;
      };
    }; 
  } 

  // --------------------------------------------------------------------------------
  // Режим установки корректировки температуры в текущем диапазоне
  // --------------------------------------------------------------------------------
  else if (mode == MODE_CORRECTION) {
    if ((bits & FLG_BUTTON_LONG) > 0) {
      notifyBuzzerKeyPress();
      // Переключаем единицу изменения
      if ((bits & FLG_BUTTON_PLUS) > 0) {
        if (step_changes == 1) {
          step_changes = 10;
        } else if (step_changes == 10) {
          step_changes = 100;
        } else {
          step_changes = 1;
        };
      } else if ((bits & FLG_BUTTON_MINUS) > 0) {
        if (step_changes == 100) {
          step_changes = 10;
        } else if (step_changes == 10) {
          step_changes = 1;
        } else {
          step_changes = 100;
        };
      };
    } else {
      if ((bits & FLG_BUTTON_MODE) > 0) {
        // Сохраняем измененные параметры
        if (sensorsSaveParameters()) {
          notifyBuzzerKeySave();
        } else {
          notifyBuzzerKeyPress();
        };
      } else {
        // Изменяем настройку в текущем диапазоне
        if ((bits & FLG_BUTTON_PLUS) > 0) {
          int8_t index = getTempCorrectionIndex(temp_pressure_compensated);
          if (index > -1) {
            notifyBuzzerKeyPress();
            temp_correction_changes[index]++;
            temp_correction_offsets[index] = temp_correction_offsets[index] + (double)step_changes / 100.0;
          } else {
            notifyBuzzerKeyError();
          };
        } else if ((bits & FLG_BUTTON_MINUS) > 0) {
          int8_t index = getTempCorrectionIndex(temp_pressure_compensated);
          if (index > -1) {
            notifyBuzzerKeyPress();
            temp_correction_changes[index]++;
            temp_correction_offsets[index] = temp_correction_offsets[index] - (double)step_changes / 100.0;
          } else {
            notifyBuzzerKeyError();
          };
        };
      };
    };
  } 

  // --------------------------------------------------------------------------------
  // Режим установки температуры кипения чистой воды
  // --------------------------------------------------------------------------------
  else if (mode == MODE_WBOILING) {
    if ((bits & FLG_BUTTON_MODE) > 0) {
      // Автоматическая настройка
      if (((bits & FLG_BUTTON_LONG) > 0) && !isnan(temp_pressure_compensated)) {
        water_boiling_offset_changes++;
        water_boiling_offset = 100.0 - (temp_pressure_compensated - water_boiling_offset);
      };
      // Сохраняем измененные параметры
      if (sensorsSaveParameters()) {
        notifyBuzzerKeySave();
      } else {
        notifyBuzzerKeyPress();
      };
    } else {
      if ((bits & FLG_BUTTON_LONG) > 0) {
        notifyBuzzerKeyPress();
        // Переключаем единицу изменения
        if ((bits & FLG_BUTTON_PLUS) > 0) {
          if (step_changes == 1) {
            step_changes = 10;
          } else if (step_changes == 10) {
            step_changes = 100;
          } else {
            step_changes = 1;
          };
        } else if ((bits & FLG_BUTTON_MINUS) > 0) {
          if (step_changes == 100) {
            step_changes = 10;
          } else if (step_changes == 10) {
            step_changes = 1;
          } else {
            step_changes = 100;
          };
        };
      } else {
        notifyBuzzerKeyPress();
        // Изменяем настройку
        if ((bits & FLG_BUTTON_PLUS) > 0) {
          water_boiling_offset_changes++;
          water_boiling_offset = water_boiling_offset + (double)step_changes / 100.0;
        } else if ((bits & FLG_BUTTON_MINUS) > 0) {
          water_boiling_offset_changes++;
          water_boiling_offset = water_boiling_offset - (double)step_changes / 100.0;
        };
      };
    };
  };
}

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------- Events --------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

static void sensorsMqttEventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
  // MQTT connected
  if (event_id == RE_MQTT_CONNECTED) {
    re_mqtt_event_data_t* data = (re_mqtt_event_data_t*)event_data;
    sensorsMqttTopicsCreate(data->primary);
  } 
  // MQTT disconnected
  else if ((event_id == RE_MQTT_CONN_LOST) || (event_id == RE_MQTT_CONN_FAILED)) {
    sensorsMqttTopicsFree();
  }
}

static void sensorsTimeEventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
  if (event_id == RE_TIME_EVERY_MINUTE) {
    xEventGroupSetBits(_sensorsFlags, FLG_TIME_CHANGED);
  } else if (event_id == RE_TIME_START_OF_DAY) {
    xEventGroupSetBits(_sensorsFlags, FLG_SENSORS_STORE);
  };
}

void sensorsParamsEventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
  if (event_id == RE_PARAMS_CHANGED) {
    xEventGroupSetBits(_sensorsFlags, FLG_PARAM_CHANGED);
  };
}

static void sensorsResetExtremumsSensor(rSensor* sensor, const char* sensor_name, uint8_t mode) 
{ 
  if (mode == 0) {
    sensor->resetExtremumsTotal();
    #if CONFIG_TELEGRAM_ENABLE
      tgSend(CONFIG_SENSOR_COMMAND_KIND, CONFIG_SENSOR_COMMAND_PRIORITY, CONFIG_SENSOR_COMMAND_NOTIFY, CONFIG_TELEGRAM_DEVICE,
        CONFIG_MESSAGE_TG_SENSOR_CLREXTR_TOTAL_DEV, sensor_name);
    #endif // CONFIG_TELEGRAM_ENABLE
  } else if (mode == 1) {
    sensor->resetExtremumsDaily();
    #if CONFIG_TELEGRAM_ENABLE
      tgSend(CONFIG_SENSOR_COMMAND_KIND, CONFIG_SENSOR_COMMAND_PRIORITY, CONFIG_SENSOR_COMMAND_NOTIFY, CONFIG_TELEGRAM_DEVICE,
        CONFIG_MESSAGE_TG_SENSOR_CLREXTR_DAILY_DEV, sensor_name);
    #endif // CONFIG_TELEGRAM_ENABLE
  } else if (mode == 2) {
    sensor->resetExtremumsWeekly();
    #if CONFIG_TELEGRAM_ENABLE
      tgSend(CONFIG_SENSOR_COMMAND_KIND, CONFIG_SENSOR_COMMAND_PRIORITY, CONFIG_SENSOR_COMMAND_NOTIFY, CONFIG_TELEGRAM_DEVICE,
        CONFIG_MESSAGE_TG_SENSOR_CLREXTR_WEEKLY_DEV, sensor_name);
    #endif // CONFIG_TELEGRAM_ENABLE
  } else if (mode == 3) {
    sensor->resetExtremumsEntirely();
    #if CONFIG_TELEGRAM_ENABLE
      tgSend(CONFIG_SENSOR_COMMAND_KIND, CONFIG_SENSOR_COMMAND_PRIORITY, CONFIG_SENSOR_COMMAND_NOTIFY, CONFIG_TELEGRAM_DEVICE,
        CONFIG_MESSAGE_TG_SENSOR_CLREXTR_ENTIRELY_DEV, sensor_name);
    #endif // CONFIG_TELEGRAM_ENABLE
  };
}

static void sensorsResetExtremumsSensors(uint8_t mode)
{
  if (mode == 0) {
    sensorColumn.resetExtremumsTotal();
    sensorPressure.resetExtremumsTotal();
    #if CONFIG_TELEGRAM_ENABLE
      tgSend(CONFIG_SENSOR_COMMAND_KIND, CONFIG_SENSOR_COMMAND_PRIORITY, CONFIG_SENSOR_COMMAND_NOTIFY, CONFIG_TELEGRAM_DEVICE,
        CONFIG_MESSAGE_TG_SENSOR_CLREXTR_TOTAL_ALL);
    #endif // CONFIG_TELEGRAM_ENABLE
  } else if (mode == 1) {
    sensorColumn.resetExtremumsDaily();
    sensorPressure.resetExtremumsDaily();
    #if CONFIG_TELEGRAM_ENABLE
      tgSend(CONFIG_SENSOR_COMMAND_KIND, CONFIG_SENSOR_COMMAND_PRIORITY, CONFIG_SENSOR_COMMAND_NOTIFY, CONFIG_TELEGRAM_DEVICE,
        CONFIG_MESSAGE_TG_SENSOR_CLREXTR_DAILY_ALL);
    #endif // CONFIG_TELEGRAM_ENABLE
  } else if (mode == 2) {
    sensorColumn.resetExtremumsWeekly();
    sensorPressure.resetExtremumsWeekly();
    #if CONFIG_TELEGRAM_ENABLE
      tgSend(CONFIG_SENSOR_COMMAND_KIND, CONFIG_SENSOR_COMMAND_PRIORITY, CONFIG_SENSOR_COMMAND_NOTIFY, CONFIG_TELEGRAM_DEVICE,
        CONFIG_MESSAGE_TG_SENSOR_CLREXTR_WEEKLY_ALL);
    #endif // CONFIG_TELEGRAM_ENABLE
  } else if (mode == 3) {
    sensorColumn.resetExtremumsEntirely();
    sensorPressure.resetExtremumsEntirely();
    #if CONFIG_TELEGRAM_ENABLE
      tgSend(CONFIG_SENSOR_COMMAND_KIND, CONFIG_SENSOR_COMMAND_PRIORITY, CONFIG_SENSOR_COMMAND_NOTIFY, CONFIG_TELEGRAM_DEVICE,
        CONFIG_MESSAGE_TG_SENSOR_CLREXTR_ENTIRELY_ALL);
    #endif // CONFIG_TELEGRAM_ENABLE
  };
};

static void sensorsCommandsEventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
  if ((event_id == RE_SYS_COMMAND) && (event_data)) {
    char* buf = malloc_string((char*)event_data);
    if (buf != nullptr) {
      const char* seps = " ";
      char* cmd = nullptr;
      char* mode = nullptr;
      char* sensor = nullptr;
      uint8_t imode = 0;
      cmd = strtok(buf, seps);
      if ((cmd != nullptr) && (strcasecmp(cmd, CONFIG_SENSOR_COMMAND_EXTR_RESET) == 0)) {
        rlog_i(logTAG, "Reset extremums: %s", buf);
        sensor = strtok(nullptr, seps);
        if (sensor != nullptr) {
          mode = strtok(nullptr, seps);
        };
      
        // Опрделение режима сброса
        if (mode == nullptr) {
          // Возможно, вторым токеном идет режим, в этом случае сбрасываем для всех сенсоров
          if (sensor) {
            if (strcasecmp(sensor, CONFIG_SENSOR_EXTREMUMS_DAILY) == 0) {
              sensor = nullptr;
              imode = 1;
            } else if (strcasecmp(sensor, CONFIG_SENSOR_EXTREMUMS_WEEKLY) == 0) {
              sensor = nullptr;
              imode = 2;
            } else if (strcasecmp(sensor, CONFIG_SENSOR_EXTREMUMS_ENTIRELY) == 0) {
              sensor = nullptr;
              imode = 3;
            };
          };
        } else if (strcasecmp(mode, CONFIG_SENSOR_EXTREMUMS_DAILY) == 0) {
          imode = 1;
        } else if (strcasecmp(mode, CONFIG_SENSOR_EXTREMUMS_WEEKLY) == 0) {
          imode = 2;
        } else if (strcasecmp(mode, CONFIG_SENSOR_EXTREMUMS_ENTIRELY) == 0) {
          imode = 3;
        };

        // Определение сенсора
        if ((sensor == nullptr) || (strcasecmp(sensor, CONFIG_SENSOR_COMMAND_SENSORS_PREFIX) == 0)) {
          sensorsResetExtremumsSensors(imode);
        } else {
          if (strcasecmp(sensor, SENSOR_COLUMN_TOPIC) == 0) {
            sensorsResetExtremumsSensor(&sensorColumn, SENSOR_COLUMN_TOPIC, imode);
          } else if (strcasecmp(sensor, SENSOR_PRESSURE_TOPIC) == 0) {
            sensorsResetExtremumsSensor(&sensorPressure, SENSOR_PRESSURE_TOPIC, imode);
          } else {
            rlog_w(logTAG, "Sensor [ %s ] not found", sensor);
            #if CONFIG_TELEGRAM_ENABLE
              tgSend(CONFIG_SENSOR_COMMAND_KIND, CONFIG_SENSOR_COMMAND_PRIORITY, CONFIG_SENSOR_COMMAND_NOTIFY, CONFIG_TELEGRAM_DEVICE,
                CONFIG_MESSAGE_TG_SENSOR_CLREXTR_UNKNOWN, sensor);
            #endif // CONFIG_TELEGRAM_ENABLE
          };
        };
      };
    };
    if (buf != nullptr) free(buf);
  };
}

static void sensorsOtaEventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
  if ((event_id == RE_SYS_OTA) && (event_data)) {
    re_system_event_data_t* data = (re_system_event_data_t*)event_data;
    if (data->type == RE_SYS_SET) {
      xEventGroupClearBits(_sensorsFlags, FLG_OTA_COMPLETED);
      xEventGroupSetBits(_sensorsFlags, FLG_OTA_STARTED);
    } else {
      xEventGroupClearBits(_sensorsFlags, FLG_OTA_STARTED);
      xEventGroupSetBits(_sensorsFlags, FLG_OTA_COMPLETED);
    };
  };
}

bool sensorsEventHandlersRegister()
{
  return eventHandlerRegister(RE_MQTT_EVENTS, ESP_EVENT_ANY_ID, &sensorsMqttEventHandler, nullptr) 
      && eventHandlerRegister(RE_TIME_EVENTS, ESP_EVENT_ANY_ID, &sensorsTimeEventHandler, nullptr)
      && eventHandlerRegister(RE_PARAMS_EVENTS, ESP_EVENT_ANY_ID, &sensorsParamsEventHandler, nullptr)
      && eventHandlerRegister(RE_SYSTEM_EVENTS, RE_SYS_COMMAND, &sensorsCommandsEventHandler, nullptr)
      && eventHandlerRegister(RE_SYSTEM_EVENTS, RE_SYS_OTA, &sensorsOtaEventHandler, nullptr);
}

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------- Задача --------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

void sensorsTaskExec(void *pvParameters)
{
  // -------------------------------------------------------------------------------------------------------
  // Инициализация устройств и сенсоров
  // -------------------------------------------------------------------------------------------------------
  gpioInit();
  sensorsInitSensors();

  // -------------------------------------------------------------------------------------------------------
  // Инициализация контроллеров
  // -------------------------------------------------------------------------------------------------------
  // Инициализация контроллеров OpenMon
  #if CONFIG_OPENMON_ENABLE
    dsChannelInit(EDS_OPENMON, 
      CONFIG_OPENMON_CTR01_ID, CONFIG_OPENMON_CTR01_TOKEN, 
      CONFIG_OPENMON_MIN_INTERVAL, CONFIG_OPENMON_ERROR_INTERVAL);
  #endif // CONFIG_OPENMON_ENABLE
  
  // Инициализация контроллеров NarodMon
  #if CONFIG_NARODMON_ENABLE
    dsChannelInit(EDS_NARODMON, 
      CONFIG_NARODMON_DEVICE01_ID, CONFIG_NARODMON_DEVICE01_KEY, 
      CONFIG_NARODMON_MIN_INTERVAL, CONFIG_NARODMON_ERROR_INTERVAL);
  #endif // CONFIG_NARODMON_ENABLE

  // Инициализация каналов ThingSpeak
  #if CONFIG_THINGSPEAK_ENABLE
    dsChannelInit(EDS_THINGSPEAK, 
      CONFIG_THINGSPEAK_DEVICE01_ID, CONFIG_THINGSPEAK_DEVICE01_KEY, 
      CONFIG_THINGSPEAK_MIN_INTERVAL, CONFIG_THINGSPEAK_ERROR_INTERVAL);
  #endif // CONFIG_THINGSPEAK_ENABLE

  // -------------------------------------------------------------------------------------------------------
  // Таймеры публикции данных с сенсоров
  // -------------------------------------------------------------------------------------------------------
  esp_timer_t mqttPubTimer;
  timerSet(&mqttPubTimer, iMqttPubInterval*1000);
  #if CONFIG_OPENMON_ENABLE
    esp_timer_t omSendTimer;
    timerSet(&omSendTimer, iOpenMonInterval*1000);
  #endif // CONFIG_OPENMON_ENABLE
  #if CONFIG_NARODMON_ENABLE
    esp_timer_t nmSendTimer;
    timerSet(&nmSendTimer, iNarodMonInterval*1000);
  #endif // CONFIG_NARODMON_ENABLE
  #if CONFIG_THINGSPEAK_ENABLE
    esp_timer_t tsSendTimer;
    timerSet(&tsSendTimer, iThingSpeakInterval*1000);
  #endif // CONFIG_THINGSPEAK_ENABLE

  lcdClear();

  static TickType_t startTicks = 0;
  static TickType_t currTicks = 0;
  static TickType_t waitTicks = 0; 
  static EventBits_t bits;
  while (1) {
    // Ждем таймаута (до периода чтения 1 сек) или нажатия кнопки
    bits = xEventGroupWaitBits(_sensorsFlags, FLGS_CHANGES, pdFALSE, pdFALSE, waitTicks);
    // Фиксируем время начала данного рабочего цикла
    startTicks = xTaskGetTickCount(); 
    // Сбрасываем биты изменений
    xEventGroupClearBits(_sensorsFlags, FLGS_AUTORESET);

    if ((bits & FLG_OTA_STARTED) > 0) {
      // -----------------------------------------------------------------------------------------------------
      // Запущено OTA-обновление, ждем его завершения (бесконечно)
      // -----------------------------------------------------------------------------------------------------
      lcdShowOTA();
      xEventGroupWaitBits(_sensorsFlags, FLG_OTA_COMPLETED, pdFALSE, pdFALSE, portMAX_DELAY);
      xEventGroupClearBits(_sensorsFlags, (FLG_OTA_STARTED | FLG_OTA_COMPLETED));
      lcdClear();
      waitTicks = 0;
    } else {
      // -----------------------------------------------------------------------------------------------------
      // Обработка нажатий на кнопки
      // -----------------------------------------------------------------------------------------------------
      if ((bits & FLG_BUTTON_PRESSED) > 0) {
        xEventGroupClearBits(_sensorsFlags, FLGS_BUTTONS);
        gpioButtonsProcess(bits);
      };

      // -----------------------------------------------------------------------------------------------------
      // Чтение данных с сенсоров, вычисления и обновление данных на дисплее
      // -----------------------------------------------------------------------------------------------------
      if ((bits & FLG_BUTTON_PRESSED) == 0) {
        sensorsReadSensors();
      };
      sensorsCalcAlcohol();
      lcdUpdate();

      // -----------------------------------------------------------------------------------------------------
      // Сохранение экстремумов с сенсоров
      // -----------------------------------------------------------------------------------------------------
      if ((bits & FLG_SENSORS_STORE) > 0) {
        xEventGroupClearBits(_sensorsFlags, FLG_SENSORS_STORE);
        sensorsStoreData();
      };

      // -----------------------------------------------------------------------------------------------------
      // Публикация данных с сенсоров
      // -----------------------------------------------------------------------------------------------------
      if ((bits & FLG_BUTTON_PRESSED) == 0) {
        // MQTT брокер
        if (esp_heap_free_check() && statesMqttIsConnected() && timerTimeout(&mqttPubTimer)) {
          timerSet(&mqttPubTimer, iMqttPubInterval*1000);
          sensorsPublishSensors();
        };

        // open-monitoring.online
        #if CONFIG_OPENMON_ENABLE
          if (statesInetIsAvailabled() && timerTimeout(&omSendTimer)) {
            timerSet(&omSendTimer, iOpenMonInterval*1000);
            char * omValues = sensorsGetOpenMonitoring();
            if (omValues) {
              dsSend(EDS_OPENMON, CONFIG_OPENMON_CTR01_ID, omValues, false);
              free(omValues);
            };
          };
        #endif // CONFIG_OPENMON_ENABLE

        // narodmon.ru
        #if CONFIG_NARODMON_ENABLE
          if (statesInetIsAvailabled() && timerTimeout(&nmSendTimer)) {
            timerSet(&nmSendTimer, iNarodMonInterval*1000);
            char * nmValues = sensorsGetNarodMon();
            if (nmValues) {
              dsSend(EDS_NARODMON, CONFIG_NARODMON_DEVICE01_ID, nmValues, false);
              free(nmValues);
            };
          };
        #endif // CONFIG_NARODMON_ENABLE

        // thingspeak.com
        #if CONFIG_THINGSPEAK_ENABLE
          if (statesInetIsAvailabled() && timerTimeout(&tsSendTimer)) {
            timerSet(&tsSendTimer, iThingSpeakInterval*1000);
            char * tsValues = sensorsGetThingSpeak();
            if (tsValues) {
              dsSend(EDS_THINGSPEAK, CONFIG_THINGSPEAK_CHANNEL01_ID, tsValues, false);
              free(tsValues);
            };
          };
        #endif // CONFIG_THINGSPEAK_ENABLE
      };

      // -----------------------------------------------------------------------------------------------------
      // Вычисление времени ожидания
      // -----------------------------------------------------------------------------------------------------
      currTicks = xTaskGetTickCount();
      if ((currTicks - startTicks) >= pdMS_TO_TICKS(CONFIG_SENSORS_TASK_CYCLE)) {
        waitTicks = 0;
      } else {
        waitTicks = pdMS_TO_TICKS(CONFIG_SENSORS_TASK_CYCLE) - (currTicks - startTicks);
      };
    };
  };

  vTaskDelete(nullptr);
  espRestart(RR_UNKNOWN);
}

void sensorsTaskInit()
{
  lcdInit();
  buzzerInit();
  sensorsInitParameters();
}

bool sensorsTaskStart()
{
  #if CONFIG_SENSORS_STATIC_ALLOCATION
    static StaticEventGroup_t sensorsFlagsBuffer;
    static StaticTask_t sensorsTaskBuffer;
    static StackType_t sensorsTaskStack[CONFIG_SENSORS_TASK_STACK_SIZE];
    _sensorsFlags = xEventGroupCreateStatic(&sensorsFlagsBuffer);
    _sensorsTask = xTaskCreateStaticPinnedToCore(sensorsTaskExec, sensorsTaskName, 
      CONFIG_SENSORS_TASK_STACK_SIZE, NULL, CONFIG_TASK_PRIORITY_SENSORS, sensorsTaskStack, &sensorsTaskBuffer, CONFIG_TASK_CORE_SENSORS);
  #else
    _sensorsFlags = xEventGroupCreate();
    xTaskCreatePinnedToCore(sensorsTaskExec, sensorsTaskName, 
      CONFIG_SENSORS_TASK_STACK_SIZE, NULL, CONFIG_TASK_PRIORITY_SENSORS, &_sensorsTask, CONFIG_TASK_CORE_SENSORS);
  #endif // CONFIG_SENSORS_STATIC_ALLOCATION
  if (_sensorsFlags && _sensorsTask) {
    xEventGroupClearBits(_sensorsFlags, 0x00FFFFFF);
    rloga_i("Task [ %s ] has been successfully created and started", sensorsTaskName);
    return sensorsEventHandlersRegister();
  }
  else {
    rloga_e("Failed to create a task for processing sensor readings!");
    return false;
  };
}

bool sensorsTaskSuspend()
{
  if ((_sensorsTask) && (eTaskGetState(_sensorsTask) != eSuspended)) {
    vTaskSuspend(_sensorsTask);
    if (eTaskGetState(_sensorsTask) == eSuspended) {
      rloga_d("Task [ %s ] has been suspended", sensorsTaskName);
      return true;
    } else {
      rloga_e("Failed to suspend task [ %s ]!", sensorsTaskName);
    };
  };
  return false;
}

bool sensorsTaskResume()
{
  if ((_sensorsTask) && (eTaskGetState(_sensorsTask) == eSuspended)) {
    vTaskResume(_sensorsTask);
    if (eTaskGetState(_sensorsTask) != eSuspended) {
      rloga_i("Task [ %s ] has been successfully resumed", sensorsTaskName);
      return true;
    } else {
      rloga_e("Failed to resume task [ %s ]!", sensorsTaskName);
    };
  };
  return false;
}


