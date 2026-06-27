                                                        //-----------------Схема----------------------//
                                                        // Цифровые                                   //
                                                        // D2 - А энкодера                            //                           
                                                        // D3 - кнопка энкодера                       //                     
                                                        // D4 - B энкодера                            //
                                                        // D5 - ds18b20 куба                          //
                                                        // D6 - ds18b20 дистиллятора                  // 
                                                        // D7 - расход воды                           //
                                                        // D8 - TX bluetooth                          //
                                                        // D9 - RX bluetooth                          //
                                                        // D10 - управление внешней нагрузкой(вода)   //
                                                        // D11 - пищалка                              //
                                                        // D12 - датчик разлития воды                 //
                                                        // D13 - ds18b20 ТСА(если подключен)          //
                                                        // Аналоговые                                 //
                                                        // A0 - резерв                                //
                                                        // A1 - резерв                                //
                                                        // A2 - резерв                                //
                                                        // A3 - резерв                                //
                                                        // А4,А5 - i2c шина - дисплей+BMP085          //
                                                        // A6 - датчик переполнения емкости           //
                                                        // A7 - резерв                                //
                                                        //--------------------------------------------//
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <BMP085.h>
#include <OneWire.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
//Конфигурация пинов
#define PIN_A 2                                          //Пин А энкодера
#define PIN_SW 3                                         //Пин кнопки
#define PIN_B 4                                          //Пин B энкодера
#define PIN_DS_KUB 5                                     //Пин датчика температуры куба
#define PIN_DS_DIS 6                                     //Пин датчика температуры дистиллятора
#define PIN_RASHOD 7                                     //Пин датчика протока воды
#define PIN_RX 8                                         //Пин RX блютуса
#define PIN_TX 9                                         //Пин TX блютуса
#define PIN_RELAY 10                                     //Пин для управления реле(клапан воды)                                   
#define PIN_SIRENA 11                                    //Пищалка
#define PIN_WATER 12                                     //Пин датчика разлития воды
#define PIN_DS_TCA 13                                    //Пин датчика температуры ТСА(если используете датчик ТСА)
//#define TCA_USE                                          //Раскоментить если используете датчик ТСА   
//#define PROTOK_USE                                       //Раскоментировать, если хотите использовать датчик расхода как датчик протока воды
#define PIN_EMKOST A6                                    //Пин датчика наполнения емкости
//Другие настройки
#define PULSE_PIN_LEN 5                                  //минимальная длинна импульса в миллисекундах на которую мы обращаем внимание
#define MENU_NUMBER 4                                    //Количество пунктов меню
#define PARAMS1_NUMBER 8                                 //Количество параметров подменю 1
#define PARAMS2_NUMBER 3                                 //Количество параметров подменю 2
#define PARAMS3_NUMBER 14                                //Количество параметров подменю 3
#define SCREEN_NUMBER 2                                  //Количество экранов с инфой
#define PARAM1_1_MAX 100                                 //Максимальное ограничение для изменения параметра Kstop(0-100%)
#define PARAM1_2_MAX 60                                  //Максимальное ограничение для изменения параметра time_refresh(1-60сек)
#define PARAM2_MIN 78.0                                  //Минимальное ограничение для изменения парметров Tstart и Tstop
#define PARAM2_MAX 100.0                                 //Максимальное ограничение для изменения парметров Tstart и Tstop
#define PARAM3_MIN -5.0                                  //Минимальное ограничение для изменения поправок температур
#define PARAM3_MAX 5.0                                   //Максимальное ограничение для изменения поправок температур

LiquidCrystal_I2C lcd(0x27,16,2);                        //Инициализация дисплея
OneWire ds_kub(PIN_DS_KUB);                              //Датчик температуры куба
OneWire ds_dis(PIN_DS_DIS);                              //Датчик температуры дистиллятора
#ifdef TCA_USE
OneWire ds_tca(PIN_DS_TCA);                              //Датчик температуры ТСА
#endif
BMP085 dps = BMP085();
SoftwareSerial bluetooth(PIN_RX, PIN_TX);                //RX-TX 
//----------------------------------------------Переменные----------------------------------------------------------------//
//Энкодер//
volatile unsigned long failingTime = 0;
volatile bool value_b = 0;
volatile byte prevA = 0;
volatile char encValue = 0;
volatile unsigned long pulseLen = 0;
//Кнопка//
volatile char button_pressed=0;                                            //Состояние кнопки
//Экран
char screen_number=1;                                                      //Номер экрана(1-главный)
//Меню
byte modeState=0;                                                          //Состояние режима (0-экраны инфо, 1-меню, 2-подменю 1, 3-подменю 2, 4-подменю 3, 5-редактирование подменю 1, 6-редактирование подменю 2, 7-редактирование подменю 3)
char menu_number=1;                                                        //Номер пункта меню
char podmenu1_number=1;                                                    //Номер пункта подменю 1(параметра)
char podmenu2_number=1;                                                    //Номер пункта подменю 2(параметра)
char podmenu3_number=1;                                                    //Номер пункта подменю 3(параметра)
char *menu_name[MENU_NUMBER]={"Up","HacTpouku","TeMnepaTypa","nonpaBku"};  //Массив названий пунктов меню
//Массив заголовков подменю 1
char *param1_name[PARAMS1_NUMBER]={"Up","DaT4uk eMkocTu","DaT4uk BoDbI", "cTon no(0-T,1-K)", "Kstop(0-100%)", "Time temp","Time bluetooth","Time sirena"};     
//Массив значений параметров 1(целочисленные)
byte param1[PARAMS1_NUMBER]={0,0,0,0,20,1,5,10};                           //0,Emkost,Water,Stop,Kstop,t_sirena ,time_refresh,refresh_bluetooth,time_sirena                               
//Массив заголовков подменю 2
char *param2_name[PARAMS2_NUMBER]={"Up","Temp start","Temp stop"};      
//Массив значений параметров 2(с плавающей точкой)
double param2[PARAMS2_NUMBER]={0,85,98};                                   //0,Tstart,Tstop
//Массив заголовков подменю 3
char *param3_name[PARAMS3_NUMBER]={"Up","dT 84*C", "dT 86*C", "dT 88*C", "dT 90*C", "dT 91*C", "dT 92*C", "dT 93*C", "dT 94*C", "dT 95*C", "dT 96*C", "dT 97*C", "dT 98*C", "dT 99*C"};      
//Массив значений параметров 3(с плавающей точкой)
double param3[PARAMS3_NUMBER]={0,0,0,0,0,0,0,0,0,0,0,0,0,0};               //0,dT1,dT2,dT3,dT4,dT5,dT6,dT7,dT8,dT9,dT10,dT11,dT12,dT13
//Выход по таймауту
char flag_quit=0;                                                          //Флаг нужен ли выход
unsigned long timer_quit;                                                  //Таймер для отсчета 10 сек выхода по бездействию
//Остальные параметры
char V[]="V4.0.1";                                                         //Версия прибора и программы
int temp1=0;                                                               //Временная переменная для хранения редактируемой в подменю 1
double temp2=0;                                                            //Временная переменная для хранения редактируемой в подменю 2
double temp3=0;                                                            //Временная переменная для хранения редактируемой в подменю 3
char incomingByte;                                                         //Данные, принятые от блютус
unsigned long bluetooth_refresh;                                           //Таймер для обновления инфы с блютуса
byte state=1;                                                              //Текущий режим диапозона температуры
//Температурные параметры
byte T[PARAMS3_NUMBER]={0,84,86,88,90,91,92,93,94,95,96,97,98,99};         //Массив температур для поправок
double t_kub,t_dis;                                                        //Переменные температуры полученные с датчиков
byte error_t_kub,error_t_dis=0;                                            //Счетчики ошибок считывания температур
unsigned long temp_refresh;                                                //Таймер для обновления температур
// Массив крепостей для расчета крепости в дистилляторе (K[0]->78.15°, K[1]->78.71°, K[2]->79.27° .. K[39]->99.99°) т.е. шаг 0.56°С 
int K[40]={9717,9320,9063,8773,8606,8489,8367,8220,8105,8009,7930,7828,7723,7585,7436,7283,7139,6982,6821,6657,
6498,6298,6108,5913,5727,5547,5356,5184,4988,4687,4543,4296,4013,3726,3395,3026,2588,1820,950,32};
//Крепость кубового остатка и дистиллятора
double alc_kub,alc_dis=0;
//Давление
long Pressure = 0;  
double Davlenie=0;
//Использование датчика ТСА
#ifdef TCA_USE
double t_tca;
byte error_t_tca=0;
#endif
//Сирена
unsigned long timer_sirena;                                                //Таймер для задержки отключения пищалки
byte sirena_state=0;                                                       //Состояние сирены
unsigned long timer_sirena_state;                                          //Таймер для перехода состояний
//Реле(вода)
unsigned long timer_relay;                                                 //Таймер для задержки отключения воды после нажатия кнопки
//Расход
int rashod;
volatile byte NbTopsFan;
unsigned long timer_protok=0;                                              //Таймер для отсуствия протока 15 сек
byte prev=LOW;
//------------------------------------------Setup---------------------------------------------------------------//
void setup()
{
 Serial.begin(9600);
 bluetooth.begin(9600);
 Wire.begin();
 dps.init(MODE_ULTRA_HIGHRES, 0, true); 
 lcd.init();
 lcd.backlight();
 digitalWrite(PIN_A,HIGH);
 digitalWrite(PIN_B,HIGH);
 attachInterrupt(0, encoder, CHANGE);                                       //Прерывание для энкодера 
 attachInterrupt(1, button, RISING);                                        //Прерывание для кнопки
 PORTD|=1<<7;
 ACSR=(0<<ACD)|(1<<ACBG)|(1<<ACIE)|(1<<ACIS1)|(1<<ACIS0);                   //Прерывание для замера скорости потока
 pinMode(PIN_WATER, INPUT_PULLUP);
 pinMode(PIN_RELAY, OUTPUT);
 #ifdef PROTOK_USE
 pinMode(PIN_RASHOD, INPUT_PULLUP);
 #endif
 start(ds_kub);
 start(ds_dis);
 #ifdef TCA_USE
 start(ds_tca);
 #endif
 lcd.setCursor(0, 0);
 lcd.print("Helper for");
 lcd.setCursor(0, 1);
 lcd.print("Distiller ");
 lcd.print(V);
 delay(3000);
 tone(PIN_SIRENA, 700, 700);
 delay(100);
 noTone(PIN_SIRENA);
 lcd.clear();
 //При удержании кнопки при запуске параметры скидываются на заводские
 if(digitalRead(PIN_SW)==LOW)
  {
   eeprom_write();
   lcd.setCursor(0, 0);
   lcd.print("C6poc npou3BeDeH");
   delay(3000);
   lcd.clear();
   button_pressed = 0;
  }
 // Читаем из еепрома параметры для редактирования   
 eeprom_read();
 temp_refresh=millis()-param1[5]*1000; 
}
//------------------------------------------Loop-----------------------------------------------------------------//
void loop()
{          
 Main();
 Mode();
 Bluetooth();
}
//-------------------------------------------Для отладки--------------------------------------------------------//  
//Зажечь диод
void led()
  {
   digitalWrite(13,HIGH);
  }

//Потушить диод
void led0()
  {
   digitalWrite(13,LOW);
  }  
