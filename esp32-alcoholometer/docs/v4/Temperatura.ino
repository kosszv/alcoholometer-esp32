//-----------------------------------------------Считывание температуры------------------------------------------------------//
//Команда на чтение температуры из памяти датчика
double get_temperature(OneWire ds)
 {
  byte data[9];
  ds.reset();
  //Запрос температуры из памяти прибора
  ds.write(0xCC);
  ds.write(0xBE);
  ds.read_bytes(data,9);
  if(ds.crc8(data,8)!=data[8]||(data[5]!= 0xFF))  
   { 
    return 404; 
   }
  double t = (data[1]<< 8)+data[0];
  t = t*0.0625;
  return t;
 }

//Команда на получение температуры с датчика и конвертации 
void start(OneWire ds)
 {
  ds.reset();
  //Запуск чтения и конвертации температуры
  ds.write(0xCC);
  ds.write(0x44);
 }
 
 //Получение и обработка температуры
 void temperatura()
  {
   //Замер температуры раз в минуту
   if((millis()-temp_refresh)>param1[5]*1000)
   {
    pressure(); 
    double tmp; 
    //Запрашиваем температуру куба
    tmp = get_temperature(ds_kub);
    start(ds_kub);
    if(tmp < 110)
     {
      t_kub = tmp+(760-Davlenie)*0.035;                                      //Измеренная температура куба с поправкой на давление
      error_t_kub = 0;
      sirena_stop();
     }
    else
     {
      if(error_t_kub < 3)
       {
        error_t_kub++;
       }
      }
   //Запрашиваем температуру дистиллятора
   tmp = get_temperature(ds_dis);
   start(ds_dis);
   if(tmp < 110)
     {
      t_dis = tmp+(760-Davlenie)*0.035;                                      //Измеренная температура пара с поправкой на давление
      t_dis = t_dis+correct_temp();
      error_t_dis = 0;
      sirena_stop();
     }
    else
     {
      if(error_t_dis < 3)
       {
        error_t_dis++;
       }
      }
   //Запрашиваем температуру ТСА
   #ifdef TCA_USE
   tmp = get_temperature(ds_tca);
   start(ds_tca);
   if(tmp < 110)
     {
      t_tca = tmp;                                                 
      error_t_tca = 0;
     }
    else
     {
      if(error_t_tca < 3)
       {
        error_t_tca++;
       }
      }
    #endif 
    temp_refresh=millis();
    alcoholicity(); 
   }
  } 
  
 //Рассчет поправки
 double correct_temp()
  {
   if (t_dis>=T[1] && t_dis<T[14])
   {
    double temp_t=0; 
    byte i=0;
    do
     { 
      i++;
     }
    while (t_dis>T[i+1]);
    temp_t=(param3[i+1]-param3[i])*(t_dis-T[i])/(T[i+1]-T[i])+param3[i];
    return temp_t;  
   }
  else return 0; 
  }
