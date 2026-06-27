void pressure()
 {  
  dps.getPressure(&Pressure);                        // Читаем давление с датчика давления
  Davlenie=Pressure/133.3;                           // Переводим давление из паскалей в мм рт ст
 }
