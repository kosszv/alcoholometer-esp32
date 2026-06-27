void Main()
 {
  temperatura();
  alarm();
  //Основной алгоритм программы
  switch(state)
   {
    // Куб прогревается до Tvoda
    case 0:
     if((t_kub<param2[1] && param2[1]<=param2[2])||(t_dis<60 && param2[1]>param2[2]))
      {
       state=1;
      }
    break;
   
    //Ожидаем когда температура куба превысит первую уставку Tvoda
    case 1: 
      if((t_kub>=param2[1] && param2[1]<=param2[2])||(param2[1]>param2[2] && t_dis>60))
      {
       timer_sirena=millis();
       digitalWrite(PIN_RELAY ,HIGH);                         //Включаем воду
       state=2;
       button_pressed = 0;
      }
    break;
   
    //Запускаем сирену либо на время, которое указано в настройках, либо до отключения джойстиком звука
    case 2:
     sirena_start();
     if (((millis()-timer_sirena) > param1[7]*1000)||(button_state()==1&&screen_number==1)) 
      {
       sirena_stop();
       state=3;
      }
     break;
     
     //Ожидаем когда температура куба превысит вторую установку Tstop или крепость будет ниже заданной в настройках при активации стопа по крепости
     case 3:
      if (((t_kub >= param2[2])&&param1[3]==0)||((alc_dis <= param1[4])&&(t_kub >= param2[1])&&param1[3]==1))
       {
        state=4;
        button_pressed = 0;
       }
      break;
      
     // Ждем нажатия кнопки, отключаем звук, запускаем таймер для отсчета 30 сек
     case 4:
      sirena_start();
      if(button_state()==1&&screen_number==1)
       {
        timer_relay=millis();        
        sirena_stop();   
        state=5;   
       }
     break; 
     
     // Ждем пока пройдет 60 сек для отключения воды
     case 5:
        if(millis()-timer_relay>60000)
         {
          digitalWrite(PIN_RELAY ,LOW);                             //Отключаем воду
          state=0;
         }
     break; 
   }
 }

//Функция обработки аварийных сигналов 
byte alarm() //0-нет аварий, 1-авария переполнения емкости, 2-авария разлития воды, 3-авария превышение температуры ТСА выше 45*(если подключен датчик), 4 - нет протока воды больше 15 сек
 {
  //Авария приемной емкости
  if(param1[1]==1 && analogRead(PIN_EMKOST)>50 && analogRead(PIN_EMKOST)<900) {modeState=9; sirena_start(); return 1;} 
  //Авария разлития воды по полу
  if(param1[2]==1 && digitalRead(PIN_WATER)==LOW) {modeState=9; sirena_start(); return 2;}
  //Авария превышения температуры ТСА
  #ifdef TCA_USE
  if(t_tca>45) {modeState=9; sirena_start(); return 3;}
  #endif
  //Авария протока воды
  #ifdef PROTOK_USE
  if(digitalRead(PIN_RELAY)==1) 
   {
    if(protok()==0){modeState=9; sirena_start(); return 4;} 
   }
  #endif
  //Нет аварий, все штатно
  else  sirena_stop(); return 0;
 }

 //Функция отслеживания протока воды
 #ifdef PROTOK_USE
 byte protok()
  {
   if(digitalRead(PIN_RASHOD)!=prev){timer_protok=millis();prev=digitalRead(PIN_RASHOD);}
   if(millis()-timer_protok>15000){return 0;}
   else return 1;
  }  
 #endif
 
 //Вывод на экран информации об аварии
 void alarm_lcd()
  {
   if(alarm()==0) {modeState=0; return;}
   lcd.setCursor(0,0);
   lcd.print("     ALARM      ");
   lcd.setCursor(0,1);
   switch(alarm())
    {
     case 1:
      lcd.print("cMeHuTe EMkoCTb ");
     break;
    
     case 2:
      lcd.print("     BODA       ");
     break;
     
     #ifdef TCA_USE     
     case 3:
      lcd.print("    TCA>45*C    ");
     break;
     #endif 
     
     #ifdef PROTOK_USE 
     case 4:
      lcd.print("  HET npoToka   ");
     break;
     #endif
    }  
  }
    
 //Функция переключения режимов отображения на дисплее
 void Mode(void)
 {
  switch(modeState)
   {
    //Режим вывода инфы
    case 0:
     screen();
    break;
   
    //Режим меню
    case 1: 
     menu();
    break;
    
    //Режим подменю 1
    case 2:
     podmenu1();
    break;
   
    //Режим подменю 2
    case 3:
     podmenu2();
    break;
    
    //Режим подменю 3
    case 4:
     podmenu3();
    break;
    
    //Режим редактирования параметров подменю 1
    case 5:
     edit1();
    break;
    
    //Режим редактирования параметров подменю 2
    case 6:
     edit2();
    break;
    
    //Режим редактирования параметров подменю 3
    case 7:
     edit3();
    break;
    
    //Информация о расходе
    case 8:
     Rashod();
    break;
    
    //Информация о расходе
    case 9:
     alarm_lcd();
    break;
   }
 } 
