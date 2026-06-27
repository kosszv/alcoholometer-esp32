void screen()
 {
  //Перемещение по экранам 
    if(joystick()==1 && Menu==0)                                   // Джойстик влево
     {        
      Screen--;
      if(Screen<1){Screen=1;}
     }      
    if(joystick()==2 && Menu==0)                                    //Джойстик вправо
     {        
      Screen++;
      if(Screen>3){Screen=3;}
     }   
  // Проверка заполнения емкости
  if(Emkost==1 && Edit==0 && digitalRead(6)==LOW)
    {
     Menu=0;
     Screen=4;
     sirena_start();
    } 
  // Проверка разлития жидкости на полу
  if(Water==1 && Edit==0 && digitalRead(3)==LOW)
    {
     Menu=0;
     Screen=5;
     sirena_start();
    }
    
 // Проверка работы датчика Ткуба
 if(TempK<5 || TempD<5)
  {
   if(Button==1 && sound_off==0)
    {
     sound_off=1;
     Button=0;
    }
   if(sound_off==0)
    {
     sirena_start();
    }
   Screen=6;
   Menu=0; 
  }
   
switch(Screen)
 {
   case 1:
   // Если звук включен и нажать вниз, то звук выключится     
    if(joystick()==4 && sound_enabled==1)                            // Джойстик вниз
     {
      sound_enabled=0;
     }
    // Если звук выключен и нажать вверх, то звук включится         
     if(joystick()==3 && sound_enabled==0)                            // Джойстик вверх
      {
       sound_enabled=1;
      }
     // Отключаем подсветку 
     if(Button==1 && Backlight==1)
      {
       lcd.noBacklight();
       Backlight=0;
       Button=0;
       delay(500);
      }
     // Включаем подсветку
     if(Button==1 && Backlight==0)
      {
       lcd.backlight();
       Backlight=1;
       Button=0;
       delay(500); 
      }
    // Информация на экране если звук включен
      lcd.setCursor(0, 0);
      lcd.print("Ky6 "); 
      lcd.print(TempK,1); 
      lcd.print(" / ");
      if(K1==0)
       {
        lcd.print("Lo");
       }
      else
       {
        lcd.print(K1,0);
       }
      lcd.print("% ");
      if(sound_enabled==1)                //звук включен
       {
        lcd.print("+   ");
       }
      else if(sound_enabled==0)                 //звук выключен
       {
        lcd.print("-   ");
       } 
       lcd.setCursor(0, 1);
       if(Diaposon==2)
        {
         lcd.print("BoDa ON         ");
        }
       if(Diaposon==4)
        {
         lcd.print("Power & BoDa OFF");
        }
       else
       { 
        lcd.print("Duc ");
        lcd.print(TempD2,1);
        lcd.print(" / ");
        if(K2==0)
         {
          lcd.print("Lo");
         }
        else
         {
          lcd.print(K2,0);
         }
        lcd.print("%      ");
       }
     break;

     // Второй экран с дополнительной информацией
     case 2:
      if(Menu==0)
      {
      lcd.setCursor(0, 0);
      lcd.print("P:");
      lcd.print(Davlenie,0);
      lcd.print("mm dP:");
      lcd.print(dP,0);
      lcd.print("mm   ");
      lcd.setCursor(0, 1);
      lcd.print("T:");
      lcd.print(Temp_davlenie,1);
      lcd.print(" dT:");
      lcd.print(dT,2);
      lcd.print("   ");
      exit();
      }
      // Нажав кнопку на джойстике попадаем в меню 
      if (Button==1 && Menu==0)
       {
        Menu=1;
        Punkt_menu=1;
        Button=0;
       }
      break;
     
      //3 экран с инфой о расходе воды
      case 3:
       Rashod();
       lcd.setCursor(0, 0);
       lcd.print("PacxoD water   ");
       lcd.setCursor(0, 1);
       lcd.print(rashod);
       lcd.print(" l/h           ");
      break;
      
      // Экран при замыкании датчика приемной емкости      
      case 4:
       lcd.setCursor(0, 0);
       lcd.print("cMeHuTe EMkoCTb ");
       lcd.setCursor(0, 1);
       lcd.print("                ");
       Screen=1;
      break; 
      
      // Экран при замыкании датчика разлития воды            
      case 5:
       lcd.setCursor(0, 0);
       lcd.print("   ALARM BODA   ");
       lcd.setCursor(0, 1);
       lcd.print("                ");
       Screen=1;
      break; 
     
      // Экран при отвалившемся датчике температуры куба            
      case 6: 
       lcd.setCursor(0, 0);
       lcd.print(" npoBepb DaT4uk ");
       lcd.setCursor(0, 1);
       if(TempK<5 && TempD>10)
       {
       lcd.print("TeMn ky6a       ");
       }
       else if(TempD<5 && TempK>10)
       {
       lcd.print("TeMn DucT       ");
       }
       else if(TempK<5 && TempD<5)
       {
       lcd.print("Ky6a u DucT   ");
       }
       Screen=1;
      break;         
     } 
   }
  
//Процедура отсчета времени для выхода на основной экран в случае бездействия
void exit()
 { 
  if(flag_timer1==0)
   {
    flag_timer1=1;
    timer1=millis();
    }
  if(joystick()==0 && Edit==0)
   {
    if(millis()-timer1>10000)
     {
      Screen=1;
      flag_timer1=0; 
      Menu=0;
      Punkt_menu=1;
     }
    }
   else
    {
     timer1=millis();
    }
  }
