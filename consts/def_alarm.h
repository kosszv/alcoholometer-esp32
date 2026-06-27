#pragma once
#include "project_config.h"

// EN: Maximum number of events that can be assigned to one sensor
// RU: Максимальное количество событий, которое можно назначить на один сенсор
#define CONFIG_ALARM_MAX_EVENTS 4

// EN: GPIO address translation for I2C port expanders
// RU: Преобразование адреса GPIO для I2C расширителей портов
#define CONFIG_ALARM_IOEXP_SENSOR(bus, address, pin) ((((bus)+1) << 16) | ((address) << 8) | (pin))

// EN: Interval for forced publication of events on MQTT (by timer)
// RU: Интервал принудительной публикации событий на MQTT (по таймеру)
#define CONFIG_ALARM_MQTT_INTERVAL_SEC 60

// EN: Timeouts for resetting 2-wire smoke detectors after triggering
// RU: Таймауты сброса 2-х проводных датчиков дыма после срабатывания
#define CONFIG_ALARM_IOEXP_FIRE_RESET_DELAY_US 5000000
#define CONFIG_ALARM_IOEXP_FIRE_RESET_RESET_US 5000000

// EN: Timeouts for maintaining the active state of sensors (for sensors that transmit only an alarm signal, without a reset signal)
// RU: Таймауты поддержания активного состояния сенсоров (для датчиков, которые передают только сигнал тревоги, без сигнала сброса)
#define CONFIG_ALARM_TIMEOUT_RF_BUTTON       1000*3
#define CONFIG_ALARM_TIMEOUT_RF_RCONTROL     1000*3
#define CONFIG_ALARM_TIMEOUT_RF_PIR_MOTION   1000*30
#define CONFIG_ALARM_TIMEOUT_RF_PERM         1000*60
#define CONFIG_ALARM_TIMEOUT_RF_DOOR         1000*30
#define CONFIG_ALARM_TIMEOUT_RF_SMOKE        1000*60
#define CONFIG_ALARM_TIMEOUT_RF_CO           1000*60*5
#define CONFIG_ALARM_TIMEOUT_RF_TAMPER       1000*60*5
#define CONFIG_ALARM_TIMEOUT_RF_LOW_BAT      1000*60*60*24

// EN: Siren and flasher duration in seconds
// RU: Длительность работы сирены и флешера в секундах
#define CONFIG_ALARM_DURATION_SIREN 60
#define CONFIG_ALARM_DURATION_FLASH 300

// EN: Time to leave the premises after arming, in seconds
// RU: Время на выход из помещения после включения режима охраны в секундах
#define CONFIG_ALARM_EXIT_TIME 60

// EN: Activate the alarm only after confirmation by any sensor within the specified time in ms. Set to 0 to disable
// RU: Активировать тревогу только после подтверждения любым сенсором в течение заданного времени в мс. Поставьте 0 для отключения
#define CONFIG_ALARM_CONFIRMATION_TIMEOUT 60*1000

// EN: Timeout of waiting for completion of a packet of codes from wireless sensors
// RU: Таймаут ожидания завершения пакета кодов с беспроводных сенсоров
#define CONFIG_ALARM_TIMEOUT_ISR 250
#define CONFIG_ALARM_TIMEOUT_RF 500
// EN: The number of codes in a row, after which data processing begins without waiting for the completion of the batch
// RU: Количество кодов подряд, после которого начинается обработка данных без ожидания завершения пакета
#define CONFIG_ALARM_THRESHOLD_RF 1
#define CONFIG_ALARM_THRESHOLD_ISR UINT16_MAX

// EN: Date and time formats
// RU: Форматы даты и времени
#define CONFIG_ALARM_TIMESTAMP_LONG "%d.%m.%Y %H:%M:%S"
#define CONFIG_ALARM_TIMESTAMP_LONG_BUF_SIZE 20
#define CONFIG_ALARM_TIMESTAMP_SHORT "%d.%m.%y %H:%M"
#define CONFIG_ALARM_TIMESTAMP_SHORT_BUF_SIZE 15

// EN: Notifications in Telegram
// RU: Уведомления в Telegram
#define CONFIG_ALARM_NOTIFY_PRIORITY_MODE_CHANGE MP_CRITICAL
#define CONFIG_ALARM_NOTIFY_PRIORITY_ALARM MP_CRITICAL
#define CONFIG_ALARM_NOTIFY_PRIORITY_COMMAND_UNDEFINED MP_REDUCED
#define CONFIG_ALARM_NOTIFY_PRIORITY_SENSOR_UNDEFINED MP_REDUCED

// EN: MQTT - publishing status
// RU: MQTT - публикация статуса
#define CONFIG_ALARM_MQTT_SECURITY_TOPIC "security"

#define CONFIG_ALARM_MQTT_STATUS_TOPIC "status"
#define CONFIG_ALARM_MQTT_STATUS_LOCAL 0
#define CONFIG_ALARM_MQTT_STATUS_QOS 1
#define CONFIG_ALARM_MQTT_STATUS_RETAINED 1

#define CONFIG_ALARM_MQTT_STATUS_DISPLAY 0
#define CONFIG_ALARM_MQTT_STATUS_SUMMARY "%s : %.2d : %s"
#define CONFIG_ALARM_MQTT_STATUS_DEVICE_EMPTY "Нет событий"
#define CONFIG_ALARM_MQTT_STATUS_JSON_ALARM "{\"sensor\":\"%s\",\"time\":\"%s\",\"time_short\":\"%s\",\"unixtime\":%s}"
#define CONFIG_ALARM_MQTT_STATUS_JSON_ANNUNCIATOR "{\"siren\":%d,\"flasher\":%d,\"summary\":%d}"
#define CONFIG_ALARM_MQTT_STATUS_EOL CONFIG_JSON_CHAR_EOL

// EN: MQTT - publishing events
// RU: MQTT - публикация событий
#define CONFIG_ALARM_MQTT_EVENTS_TOPIC "events"
#define CONFIG_ALARM_MQTT_EVENTS_LOCAL 0
#define CONFIG_ALARM_MQTT_EVENTS_QOS 2
#define CONFIG_ALARM_MQTT_EVENTS_RETAINED 1

#define CONFIG_ALARM_MQTT_EVENTS_TYPE "type"
#define CONFIG_ALARM_MQTT_EVENTS_STATUS "status"
#define CONFIG_ALARM_MQTT_EVENTS_JSON "json"
#define CONFIG_ALARM_MQTT_EVENTS_JSON_TEMPLATE "{\"status\":%d,\"time\":\"%s\",\"time_short\":\"%s\",\"unixtime\":%s,\"count\":%d}"
#define CONFIG_ALARM_MQTT_EVENTS_ASE_ALARM "alarm"
#define CONFIG_ALARM_MQTT_EVENTS_ASE_TAMPER "tamper"
#define CONFIG_ALARM_MQTT_EVENTS_ASE_POWER "power"
#define CONFIG_ALARM_MQTT_EVENTS_ASE_BATTERY "lowbat"
#define CONFIG_ALARM_MQTT_EVENTS_ASE_CONTROL_ON "control/on"
#define CONFIG_ALARM_MQTT_EVENTS_ASE_CONTROL_OFF "control/off"
#define CONFIG_ALARM_MQTT_EVENTS_ASE_CONTROL_PERIMETER "control/perimeter"
#define CONFIG_ALARM_MQTT_EVENTS_ASE_CONTROL_OUTBUILDINGS "control/outbuildings"

// EN: MQTT - unknown sensors
// RU: MQTT - неизвестные датчики
#define CONFIG_ALARM_MQTT_RX433_UNKNOWN_TOPIC "rx433new"
#define CONFIG_ALARM_MQTT_RX433_UNKNOWN_LOCAL 0
#define CONFIG_ALARM_MQTT_RX433_UNKNOWN_QOS 0
#define CONFIG_ALARM_MQTT_RX433_UNKNOWN_RETAINED 1

// EN: Settings
// RU: Параметры
#define CONFIG_ALARM_PARAMS_ROOT_KEY "security"
#define CONFIG_ALARM_PARAMS_ROOT_TOPIC "security"
#define CONFIG_ALARM_PARAMS_ROOT_FRIENDLY "Сигнализация"

#define CONFIG_ALARM_PARAMS_MODE_KEY "mode"
#define CONFIG_ALARM_PARAMS_MODE_FRIENDLY "Режим"
#define CONFIG_ALARM_PARAMS_SIREN_DUR_KEY "siren_duration"
#define CONFIG_ALARM_PARAMS_SIREN_DUR_FRIENDLY "Длительность звучания сирены"
#define CONFIG_ALARM_PARAMS_SIREN_SILENT_ENABLED_KEY "silent_enabled"
#define CONFIG_ALARM_PARAMS_SIREN_SILENT_ENABLED_FRIENDLY "Блокировать сирену в ночное время"
#define CONFIG_ALARM_PARAMS_SIREN_SILENT_PERIOD_KEY "silent_period"
#define CONFIG_ALARM_PARAMS_SIREN_SILENT_PERIOD_FRIENDLY "Интервал времени блокировки сирены"
#define CONFIG_ALARM_PARAMS_FLASHER_DUR_KEY "flash_duration"
#define CONFIG_ALARM_PARAMS_FLASHER_DUR_FRIENDLY "Длительность световых вспышек"
#define CONFIG_ALARM_PARAMS_BUZZER_KEY "buzzer"
#define CONFIG_ALARM_PARAMS_BUZZER_FRIENDLY "Звуковой сигнал на пульте"
#define CONFIG_ALARM_PARAMS_CONFIRMATION_TIMEOUT_KEY "confirmation"
#define CONFIG_ALARM_PARAMS_CONFIRMATION_TIMEOUT_FRIENDLY "Время подтверждения тревоги (мс)"
#define CONFIG_ALARM_PARAMS_EXIT_TIME_KEY "exit_time"
#define CONFIG_ALARM_PARAMS_EXIT_TIME_FRIENDLY "Время на выход из зоны охраны (сек)"
#define CONFIG_ALARM_PARAMS_FIX_RX433_CODES_KEY "fix_433_codes"
#define CONFIG_ALARM_PARAMS_FIX_RX433_CODES_FRIENDLY "Фиксировать новые RX433 коды"
#define CONFIG_ALARM_PARAMS_MQTT_INTERVAL_KEY "mqtt_interval"
#define CONFIG_ALARM_PARAMS_MQTT_INTERVAL_FRIENDLY "Интервал публикации списка событий на MQTT (сек)"


#define CONFIG_ALARM_PARAMS_QOS 1
#define CONFIG_ALARM_PARAMS_MIN_DURATION 0
#define CONFIG_ALARM_PARAMS_MAX_DURATION 3600

// EN: Text descriptions of security modes
// RU: Текстовые описания режимов охраны
#define CONFIG_ALARM_MODE_CHAR_DISABLED "🔓"
#define CONFIG_ALARM_MODE_CHAR_ARMED "🔐" // "🔒" (замок) | "🛡" (щит)
#define CONFIG_ALARM_MODE_CHAR_PERIMETER "⭕️"
#define CONFIG_ALARM_MODE_CHAR_OUTBUILDINGS "🚘"

#define CONFIG_ALARM_ANNUNCIATOR_OFF "🔕"
#define CONFIG_ALARM_ANNUNCIATOR_FLASHER "🔆"
#define CONFIG_ALARM_ANNUNCIATOR_SIREN "🔔"
#define CONFIG_ALARM_ANNUNCIATOR_TOTAL "🚨"

#define CONFIG_ALARM_MODE_DISABLED "Охрана отключена"
#define CONFIG_ALARM_MODE_ARMED "Полная охрана"
#define CONFIG_ALARM_MODE_PERIMETER "Охрана периметра"
#define CONFIG_ALARM_MODE_OUTBUILDINGS "Охрана гаража"

#define CONFIG_ALARM_SIREN_DISABLED "Отключена"
#define CONFIG_ALARM_SIREN_ENABLED "Включена"

// EN: Text commands
// RU: Текстовые команды
#define CONFIG_ALARM_COMMAND_MODE_DISABLED "alarm off"
#define CONFIG_ALARM_COMMAND_MODE_ARMED "alarm full"
#define CONFIG_ALARM_COMMAND_MODE_PERIMETER "alarm perimiter"
#define CONFIG_ALARM_COMMAND_MODE_OUTBUILDINGS "alarm garage"
#define CONFIG_ALARM_COMMAND_ALARM_CANCEL "alarm cancel"
#define CONFIG_ALARM_COMMAND_ALARM_RESET "alarm reset"

// EN: Text descriptions of security events
// RU: Текстовые описания событий охраны
#define CONFIG_ALARM_EVENT_MESSAGE_TAMPER "⚠️ Попытка взлома датчика"
#define CONFIG_ALARM_EVENT_MESSAGE_CLOSED "💠 Датчик восстановлен"
#define CONFIG_ALARM_EVENT_MESSAGE_UNKNOWN "🚨 Неизвестная тревога"
#define CONFIG_ALARM_EVENT_MESSAGE_DOOROPEN "🚨 Открыта дверь"
#define CONFIG_ALARM_EVENT_MESSAGE_MOTION "🚨 Обнаружено движение"
#define CONFIG_ALARM_EVENT_MESSAGE_GAS "🚨 Обнаружена утечка газа"
#define CONFIG_ALARM_EVENT_MESSAGE_CMOX "🚨 Обнаружен угарный газ"
#define CONFIG_ALARM_EVENT_MESSAGE_SMOKE "🔥 Обнаружено задымление"
#define CONFIG_ALARM_EVENT_MESSAGE_WATER "💧 Протечка воды"
#define CONFIG_ALARM_EVENT_MESSAGE_CLEAR "🟢 Авария устранена"
#define CONFIG_ALARM_EVENT_MESSAGE_POWER_ON "🟩 Электропитание восстановлено"
#define CONFIG_ALARM_EVENT_MESSAGE_POWER_OFF "🟨 Электропитание отключено"
#define CONFIG_ALARM_EVENT_MESSAGE_POWER_MAIN_ON "💡 Основное питание восстановлено"
#define CONFIG_ALARM_EVENT_MESSAGE_POWER_MAIN_OFF "🔴 Основное питание отключено"
#define CONFIG_ALARM_EVENT_MESSAGE_BATTERY_LOW "🔋 Низкий уровень заряда батареи"
#define CONFIG_ALARM_EVENT_MESSAGE_BATTERY_NRM "🔋 Аккумулятор заряжен"
#define CONFIG_ALARM_EVENT_MESSAGE_DOORBELL "🔔 Звонок в дверь"
#define CONFIG_ALARM_EVENT_MESSAGE_BUTTON "🔴 Нажата тревожная кнопка"
#define CONFIG_ALARM_EVENT_MESSAGE_RCTRL_OFF CONFIG_ALARM_MODE_CHAR_DISABLED " Пульт: режим охраны отключен"
#define CONFIG_ALARM_EVENT_MESSAGE_RCTRL_ON CONFIG_ALARM_MODE_CHAR_ARMED " Пульт: режим охраны включен" 
#define CONFIG_ALARM_EVENT_MESSAGE_RCTRL_PERIMETER CONFIG_ALARM_MODE_CHAR_PERIMETER " Пульт: режим охраны периметра"
#define CONFIG_ALARM_EVENT_MESSAGE_RCTRL_OUTBUILDINGS CONFIG_ALARM_MODE_CHAR_OUTBUILDINGS " Пульт: режим охраны внешних помещений"

// EN: Text descriptions of security events
// RU: Текстовые описания канала управления
#define CONFIG_ALARM_SOURCE_STORED "Память"
#define CONFIG_ALARM_SOURCE_BUTTONS "Панель управления"
#define CONFIG_ALARM_SOURCE_RCONTROL "Пульт"
#define CONFIG_ALARM_SOURCE_MQTT "Интернет"
#define CONFIG_ALARM_SOURCE_COMMAND "Интернет"

// EN: Telegram notifications when switching the security mode
// RU: Уведомления в Telegram при переключении режима охраны
#define CONFIG_NOTIFY_TELEGRAM_ALARM_MODE_DISABLED CONFIG_ALARM_MODE_CHAR_DISABLED " Охрана <b>отключена</b> ( <b>%s</b> )"
#define CONFIG_NOTIFY_TELEGRAM_ALARM_MODE_ARMED_INSTANT CONFIG_ALARM_MODE_CHAR_ARMED " Охрана <b>включена</b> ( <b>%s</b> )"
#define CONFIG_NOTIFY_TELEGRAM_ALARM_MODE_ARMED_DELAYED "🕘 Охрана <b>включена с задержкой на выход %d секунд(ы)</b> ( <b>%s</b> ). <i>Необходимо покинуть зону охраны за указанное время</i>"
#define CONFIG_NOTIFY_TELEGRAM_ALARM_MODE_PERIMETER CONFIG_ALARM_MODE_CHAR_PERIMETER " Включена <b>охрана периметра</b> ( <b>%s</b> )"
#define CONFIG_NOTIFY_TELEGRAM_ALARM_MODE_OUTBUILDINGS CONFIG_ALARM_MODE_CHAR_OUTBUILDINGS " Включена <b>охрана внешних помещений</b> ( <b>%s</b> )"
#define CONFIG_NOTIFY_TELEGRAM_ALARM_MODE_ACTIVATED CONFIG_ALARM_MODE_CHAR_ARMED " Время выхода из зоны охраны истекло, <b>режим охраны активирован</b>"

// EN: Telegram notification templates
// RU: Шаблоны уведомлений в Telegram
#define CONFIG_NOTIFY_TELEGRAM_ALARM_CANCELED "🔕 Тревога <b>отключена</b> ( <b>%s</b> )"
#define CONFIG_NOTIFY_TELEGRAM_ALARM_RESET "💠 Счетчик тревог сброшен ( <b>%s</b> )"
#define CONFIG_NOTIFY_TELEGRAM_ALARM_TEMPLATE "<b>%s</b>\n\n<code>Сенсор:   </code><b>%s</b>\n<code>Зона:     </code><b>%s</b>\n<code>Режим:    </code><b>%s</b>\n<code>Сирена:   </code><b>%s</b>\n<code>Время:    </code><b>%s</b>\n<code>Тревоги:  </code><b>%d</b>"
#define CONFIG_NOTIFY_TELEGRAM_ALARM_SENSOR_UNDEFINED_TEMPLATE "🟡 Получена команда от неизвестного датчика ОПС\n\n<code>Значение: 0x%.8X\nАдрес:    0x%.8X\nКоманда:  0x%02X</code>"
#define CONFIG_NOTIFY_TELEGRAM_ALARM_COMMAND_UNDEFINED_TEMPLATE "🟡 Получена неизвестная команда от датчика ОПС\n\n<code>Датчик:   %s\nЗначение: 0x%.8X\nАдрес:    0x%.8X\nКоманда:  0x%02X</code>"

// EN: LED flash mode on input signal
// RU: Режим вспышек светодиода при входящем сигнале
#define CONFIG_ALARM_INCOMING_QUANTITY 1
#define CONFIG_ALARM_INCOMING_DURATION 50
#define CONFIG_ALARM_INCOMING_INTERVAL 50

// EN: Flash mode of the flasher in alarm mode
// RU: Режим вспышек извещателя в режиме тревоги
#define CONFIG_ALARM_ALARM_QUANTITY 1
#define CONFIG_ALARM_ALARM_DURATION 500
#define CONFIG_ALARM_ALARM_INTERVAL 500

// EN: Flash mode of the flasher in perimeter armed mode
// RU: Режим вспышек извещателя в режиме частичной охраны
#define CONFIG_ALARM_PARTIAL_QUANTITY 2
#define CONFIG_ALARM_PARTIAL_DURATION 100
#define CONFIG_ALARM_PARTIAL_INTERVAL 5000

// EN: Flash mode of the flasher in full armed mode without alarms
// RU: Режим вспышек извещателя в режиме полной охраны без тревог
#define CONFIG_ALARM_ARMED_QUANTITY 1
#define CONFIG_ALARM_ARMED_DURATION 100
#define CONFIG_ALARM_ARMED_INTERVAL 5000

// EN: Flash mode of the flasher in full armed mode with alarms
// RU: Режим вспышек извещателя в режиме полной охраны c тревогами
#define CONFIG_ALARM_WARNING_QUANTITY 3
#define CONFIG_ALARM_WARNING_DURATION 200
#define CONFIG_ALARM_WARNING_INTERVAL 5000

// EN: Siren signal when full armed mode is on
// RU: Сигнал сирены при включении режима полной охраны
#define CONFIG_ALARM_SIREN_ARMED_QUANTITY 1
#define CONFIG_ALARM_SIREN_ARMED_DURATION 250
#define CONFIG_ALARM_SIREN_ARMED_INTERVAL 250
#define CONFIG_ALARM_BUZZER_ARMED_FREQUENCY 1568
#define CONFIG_ALARM_BUZZER_ARMED_DURATION 250
#define CONFIG_ALARM_BUZZER_ARMED_QUANTITY 1

// EN: Siren sound when partial arming is on
// RU: Сигнал сирены при включении режима частичной охраны
#define CONFIG_ALARM_SIREN_PARTIAL_QUANTITY 0
#define CONFIG_ALARM_SIREN_PARTIAL_DURATION 100
#define CONFIG_ALARM_SIREN_PARTIAL_INTERVAL 100
#define CONFIG_ALARM_BUZZER_PARTIAL_FREQUENCY 1568
#define CONFIG_ALARM_BUZZER_PARTIAL_DURATION 100
#define CONFIG_ALARM_BUZZER_PARTIAL_QUANTITY 2

// EN: Siren sound when disarming without alarms
// RU: Сигнал сирены при отключении режима охраны без тревог
#define CONFIG_ALARM_SIREN_DISABLED_NORMAL_QUANTITY 3
#define CONFIG_ALARM_SIREN_DISABLED_NORMAL_DURATION 100
#define CONFIG_ALARM_SIREN_DISABLED_NORMAL_INTERVAL 100
#define CONFIG_ALARM_BUZZER_DISABLED_NORMAL_FREQUENCY 1568
#define CONFIG_ALARM_BUZZER_DISABLED_NORMAL_DURATION 100
#define CONFIG_ALARM_BUZZER_DISABLED_NORMAL_QUANTITY 3

// EN: Siren sound when disarming with alarms
// RU: Сигнал сирены при отключении режима охраны с тревогами
#define CONFIG_ALARM_SIREN_DISABLED_WARNING_QUANTITY 5
#define CONFIG_ALARM_SIREN_DISABLED_WARNING_DURATION 100
#define CONFIG_ALARM_SIREN_DISABLED_WARNING_INTERVAL 100
#define CONFIG_ALARM_BUZZER_DISABLED_WARNING_FREQUENCY 1568
#define CONFIG_ALARM_BUZZER_DISABLED_WARNING_DURATION 100
#define CONFIG_ALARM_BUZZER_DISABLED_WARNING_QUANTITY 5

// EN: Buzzer sound on alarm
// RU: Сигнал зуммера при тревоге
#define CONFIG_ALARM_BUZZER_ALARM_FREQUENCY 1568
#define CONFIG_ALARM_BUZZER_ALARM_DURATION 500
#define CONFIG_ALARM_BUZZER_ALARM_QUANTITY 5

// EN: Buzzer sound when alarm is canceled
// RU: Сигнал зуммера при отмене тревоги
#define CONFIG_ALARM_BUZZER_ALARM_CLEAR_FREQUENCY 1568
#define CONFIG_ALARM_BUZZER_ALARM_CLEAR_DURATION 100
#define CONFIG_ALARM_BUZZER_ALARM_CLEAR_QUANTITY 3
