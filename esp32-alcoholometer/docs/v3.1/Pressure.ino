void pressure()
 {
  dps.init(MODE_ULTRA_HIGHRES, 0, true); 
  dps.getPressure(&Pressure);                        // Читаем давление с датчика давления
  dps.getTemperature(&Temperature);                  // Читаем температуру с датчика давления 
  Davlenie=Pressure/133.3;                           // Переводим давление из паскалей в мм рт ст
  dP=Davlenie-760;                                   // Расчитываем разницу давлений между нормой 760 мм и текущим
  dT=0.035*dP;                                       // Расчитываем поправку к температуре исходя из давления
  Temp_davlenie=Temperature*0.1;                     // Температура в помещении
 }
