//------------------------------------------------------------Энкодер--------------------------------------------------------//
//Обработчик энкодера
void encoder(void)
{
  byte A = digitalRead(PIN_A);                                 // пока не отчитались ничего больше не делаем
   if(prevA && !A)
    {                                 // фронт упал
     value_b = digitalRead(PIN_B);   // определили направление, но пока только "запомнили его"
     failingTime = millis();         // и запомнили когда мы "упали в ноль", начали отсчет длины импульса
    }
   if(!prevA && A && failingTime)
    {                                 // восходящий фронт и мы в режиме "отсчет времени импульса
     pulseLen = millis() - failingTime;
     if( pulseLen > PULSE_PIN_LEN)
      {                               // импульс бы достаточно длинный что-бы поверить тому что мы прочитали в его начале
       if(value_b) encValue++; 
        else encValue--;
      }
      failingTime = 0;                // больше не ведем отсчет времени импульса
    }
  prevA = A;
}

//Состояние энкодера
char encoder_state()  // 1 - кручу вправо(вперед,увеличение), -1-кручу влево(назад,уменьшение), 0-не кручу
{
 if (encValue > 0)
 {
  encValue = 0;
  flag_quit=0;
  lcd.clear();
  return 1;
 }
 else if (encValue < 0)
 { 
  encValue = 0;
  flag_quit=0;
  lcd.clear();
  return -1;
 }
 else
 {
  return 0;
 }
}
