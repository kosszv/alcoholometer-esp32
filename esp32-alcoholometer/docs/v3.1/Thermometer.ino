void thermometer()
 {
  Wire.begin();
  sensors.requestTemperatures();
  TempK = sensors.getTempC(kub);                    // Читаем температуру датчика куба
  TempD = sensors.getTempC(distil);                 // Читаем температуру датчика дистиллятора 
  TempK = TempK - dT;                               // Вносим поправку температуры от давления
  TempD = TempD - dT;                               // Вносим поправку температуры от давления   
  
  //Вносим поправку температуры в разных температурных диапозонах для более точного определения крепости
   if (TempD<81)
    {
     TempD2 = TempD; 
    }
//   if (TempD>=78 && TempD<81)
//    {
//     TempD2 = TempD+dTD78_81;
//    }
   if (TempD>=81 && TempD<84)
    {
     TempD2 = TempD+dTD81_84;
    }
   if (TempD>=84 && TempD<87)
    {
     TempD2 = TempD+dTD84_87;
    }
   if (TempD>=87 && TempD<90)
    {
     TempD2 = TempD+dTD87_90;
    }
   if (TempD>=90 && TempD<91)
    {
     TempD2 = TempD+dTD90_91;
    }
   if (TempD>=91 && TempD<92)
    { 
     TempD2 = TempD+dTD91_92;
    }
   if (TempD>=92 && TempD<93)
    {
    TempD2 = TempD+dTD92_93;
    }
   if (TempD>=93 && TempD<94)
    {
     TempD2 = TempD+dTD93_94;
    }
   if (TempD>=94 && TempD<95)
    {
     TempD2 = TempD+dTD94_95;
    }
   if (TempD>=95 && TempD<96)
    {
     TempD2 = TempD+dTD95_96;
    }
   if (TempD>=96 && TempD<97)
    {
     TempD2 = TempD+dTD96_97;
    }
   if (TempD>=97 && TempD<98)
    {
     TempD2 = TempD+dTD97_98;
    }  
   if (TempD>=98 && TempD<99)
    {
     TempD2 = TempD+dTD98_99;
    }
   if (TempD>=99 && TempD<100)
    {
     TempD2 = TempD+dTD99_100;
    }
   // Вычислитель остаточной крепости в кубе
   if (TempK > 78.1 && TempK < 100)
    {  
     K1 = 16.75-19.05*((TempK-89.03)/6.54)+12.64*sq((TempK-89.03)/6.54)-3.69*pow(((TempK-89.03)/6.54),3)-0.38*pow(((TempK-89.03)/6.54),4);
    }
   else
    {
     K1=0; 
    }
    
    // Вычисляем крепость пара в дистилляторе
    TempD4=TempD2*10;
    if (TempD4>=(T[0]+750) && TempD4<(T[169]+750))
     {
      byte n=0;
      do
      { 
       n=n+1;
      } 
      while (TempD4>(T[n]+750));
      double K21=K[n];
      double K22=K[n+1];
      double T21=T[n]+750;
      double T22=T[n+1]+750;
      TempD5=TempD4;
      K2 = (K21+(K22-K21)*(TempD5-T21)/(T22-T21))/100;
     }
     else
      {
       K2=0; 
      }
   // Обработка температурных диапозонов
   switch(Diaposon)
    {
     // Куб прогревается до Tvoda
     case 0:
      if(TempK < Tvoda)
       {
        Diaposon=1;
       }
     break;
     
     //Ожидаем когда температура куба превысит первую уставку Tvoda
     case 1:
      if(TempK >= Tvoda)
       {
        timer2=millis();
        Diaposon=2;
        sound_enabled=1;
       }
     break;
   
     //Запускаем сирену либо на время, которое указано в настройках, либо до отключения джойстиком звука
     case 2:
      sirena_start();
      if (((millis()-timer2) > Time_sirena*1000)||sound_enabled==0) 
       {
        Diaposon=3;
        sound_enabled=1;
        noTone(7);       
       }
      break;
      
     //Ожидаем когда температура куба превысит вторую установку Tstop или крепость будет ниже заданной в настройках при активации стопа по крепости
     case 3:
      if (((TempK >= Tstop)&&Stop==0)||((K2 <= Kstop)&&(TempK >= Tvoda)&&Stop==1))
       {
        Diaposon=4;
       }
      break; 
     
     // Ждем отключения звука и возвращаемся в начало на следующий круг
     case 4:
      sirena_start();
      if(sound_enabled==0)
       {
        Diaposon=0;
        sound_enabled=1;
        noTone(7);      
       }
     break; 
    }   
 }
 
 
