#pragma once
#include "project_config.h"

// EN: Ping task parameters: stack size and priority
// RU: Параметры задачи пинга: размер стека и приоритет
#define CONFIG_PINGER_TASK_STATIC_ALLOCATION 1
#define CONFIG_PINGER_TASK_STACK_SIZE 3*1024
// EN: A list of no more than three ping servers. If you don't need three, comment out the extra ones.
// RU: Список из не более чем трех серверов для пинга. Если не требуется три, закоментируйте лишние.
#define CONFIG_PINGER_HOST_1 "yandex.ru"
#define CONFIG_PINGER_HOST_2 "google.com"
// #define CONFIG_PINGER_HOST_3 "8.8.8.8"
// EN: The period of validity for determining the host's IP address in ms. Use 0 if not use this option
// RU: Период валидности определения IP-адреса хоста в мс. Используйте 0, если не хотите использовать эту опцию
#define CONFIG_PINGER_IP_VALIDITY 1000*60*60
// EN: Filter for "smoothing" ping results (0 - disabled, 1 - average, 2 - median)
// RU: Фильтр для "сглаживания" результатов пинга (0 - отключено, 1 -  среднее, 2 - медиана)
#define CONFIG_PINGER_FILTER_MODE 1
#define CONFIG_PINGER_FILTER_SIZE 64
// EN: Format for ping response time
// RU: Формат для времени ответа пинга
#define CONFIG_FORMAT_PING_TIMERESP_VALUE "%d"
// EN: Format for ping response time as a string
// RU: Формат для времени ответа пинга в виде строки
#define CONFIG_FORMAT_PING_TIMERESP_STRING "%d ms"
// EN: Format for ping response time
// RU: Формат для времени ответа пинга
#define CONFIG_FORMAT_PING_LOSS_VALUE "%.1f"
// EN: Format for ping response time as a string
// RU: Формат для времени ответа пинга в виде строки
#define CONFIG_FORMAT_PING_LOSS_STRING "%.1f% %"
// EN: Format for three values (status, delay, loss) in one topic
// RU: Формат для трех значений (статус, задержка, потери) в одном топике
#define CONFIG_FORMAT_PING_MIXED "%s" CONFIG_JSON_CHAR_EOL "t: %d ms" CONFIG_JSON_CHAR_EOL "l: %.0f% %"
// EN: Ping statuses
// RU: Надписи для статусов пинга
#define CONFIG_FORMAT_PING_OK "📶"
#define CONFIG_FORMAT_PING_SLOWDOWN "🐌"
#define CONFIG_FORMAT_PING_UNAVAILABLED "⛔️"
// EN: Parameters
// RU: Параметры
#define CONFIG_PINGER_PGROUP_ROOT_KEY "ping"
#define CONFIG_PINGER_PGROUP_ROOT_TOPIC "ping"
#define CONFIG_PINGER_PGROUP_ROOT_FRIENDLY "PING"
// EN: Number of packets in one ping session
// RU: Количество пакетов в одной сессии пинга
#define CONFIG_PINGER_PARAM_COUNT 5
#define CONFIG_PINGER_PARAM_COUNT_KEY "count"
#define CONFIG_PINGER_PARAM_COUNT_FRIENDLY "Количество пакетов"
// EN: Ping packet size
// RU: Размер пакета пинга
#define CONFIG_PINGER_PARAM_DATASIZE 32
#define CONFIG_PINGER_PARAM_DATASIZE_KEY "datasize"
#define CONFIG_PINGER_PARAM_DATASIZE_FRIENDLY "Размер пакета"
// EN: Response ping timeout
// RU: Время ожидания ответного пакета
#define CONFIG_PINGER_PARAM_TIMEOUT 3000
#define CONFIG_PINGER_PARAM_TIMEOUT_KEY "timeout"
#define CONFIG_PINGER_PARAM_TIMEOUT_FRIENDLY "Время ожидания, мс"
// EN: Evaluate the final ping result by: 0 - the best value, 1 - the average value, 2 - the worst value
// RU: Оценивать итоговый результат пинга по: 0 - лучшему значению, 1 - среднему значению, 2 - худшему значению
#define CONFIG_PINGER_TOTAL_RESULT_MODE 1
#define CONFIG_PINGER_PARAM_RESULT_MODE_KEY "result_mode"
#define CONFIG_PINGER_PARAM_RESULT_MODE_FRIENDLY "Итоговое значение"
// EN: Thresholds for latency and packet loss above which Internet access is considered "slow"
// RU: Пороговые значения задержки и потери пакетов, при превышении которых доступ в интернет считается "медленным"
#define CONFIG_PINGER_SLOWDOWN_DURATION 500
#define CONFIG_PINGER_SLOWDOWN_LOSS 50.0
#define CONFIG_PINGER_PARAM_SLOWDOWN_DURATION_KEY "slow/duration"
#define CONFIG_PINGER_PARAM_SLOWDOWN_DURATION_FRIENDLY "Пинг при замедлении доступа, мс"
#define CONFIG_PINGER_PARAM_SLOWDOWN_LOSS_KEY "slow/loss"
#define CONFIG_PINGER_PARAM_SLOWDOWN_LOSS_FRIENDLY "Потери при замедлении доступа, %"
// EN: Thresholds for latency and packet loss above which Internet access is considered "unavalabled"
// RU: Пороговые значения задержки и потери пакетов, при превышении которых доступ в интернет считается "не доступным"
#define CONFIG_PINGER_UNAVAILABLE_DURATION 750
#define CONFIG_PINGER_UNAVAILABLE_LOSS 75.0
#define CONFIG_PINGER_PARAM_UNAVAILABLE_DURATION_KEY "failed/duration"
#define CONFIG_PINGER_PARAM_UNAVAILABLE_DURATION_FRIENDLY "Пинг при отсутствии доступа, мс"
#define CONFIG_PINGER_PARAM_UNAVAILABLE_LOSS_KEY "failed/loss"
#define CONFIG_PINGER_PARAM_UNAVAILABLE_LOSS_FRIENDLY "Потери при отсутствии доступа, %"
// EN: If Internet access is completely lost: the number of checks after which network tasks will be suspended
// RU: При полной потере доступа в интернет: количество проверок, после которых будут приостановлены сетевые задачи
#define CONFIG_PINGER_UNAVAILABLE_THRESHOLD 3
#define CONFIG_PINGER_PARAM_UNAVAILABLE_THRESHOLD_KEY "failed/threshold"
#define CONFIG_PINGER_PARAM_UNAVAILABLE_THRESHOLD_FRIENDLY "Количество ошибок подряд для переключения режима"
// EN: Normal mode check interval, in milliseconds. Counts from the BEGINNING of the last check (regular interval)
// RU: Интервал проверки в нормальном режиме, в миллисекундах. Отсчитывается со времени НАЧАЛА последней проверки (равномерный интервал)
#define CONFIG_PINGER_INTERVAL_AVAILABLE 60000 
#define CONFIG_PINGER_PARAM_INTERVAL_AVAILABLE_KEY "intervals/good"
#define CONFIG_PINGER_PARAM_INTERVAL_AVAILABLE_FRIENDLY "Интервал проверки при нормальном пинге, мс"
// EN: Check interval when access is denied, in milliseconds. Counts since the END of the last check (irregular interval)
// RU: Интервал проверки, когда доступ отсутствует, в миллисекундах. Отсчитывается со времени ОКОНЧАНИЯ последней проверки (неравномерный интервал)
#define CONFIG_PINGER_INTERVAL_UNAVAILABLE 10000
#define CONFIG_PINGER_PARAM_INTERVAL_UNAVAILABLE_KEY "intervals/failed"
#define CONFIG_PINGER_PARAM_INTERVAL_UNAVAILABLE_FRIENDLY "Интервал проверки при плохом пинге, мс"

