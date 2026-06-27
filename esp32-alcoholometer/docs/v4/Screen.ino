//Навигация по экранам
void screenNavi()
 {
  char enc = encoder_state(); 
  if(enc != 0)
   {
    screen_number+=enc;     
    if(screen_number>=SCREEN_NUMBER) {screen_number=SCREEN_NUMBER;}
    else if(screen_number<0) {screen_number=0;}    
   }    
 }
 
//Вывод основной инфы на экран modeState=0
void screen()
 {
  screenNavi();
  switch(screen_number)
   {
    case 0:
     if(button_state()==1){modeState=1;return;}
     lcd.setCursor(0,0);
     lcd.print("Menu");
     lcd.setCursor(0,1);
     lcd.print("press to enter");
     quit();
    break; 
    
    //Главный экран
    case 1:
     //1 строчка
     lcd.setCursor(0,0);
     if(error_t_kub==3)
      { 
       lcd.print("DS ky6 error    ");
       sirena_start();
      }
     else
      {
       lcd.print("Ky6 ");
       lcd.print(t_kub,1);
       lcd.print(" / ");
       if(alc_kub==0)
        {
         lcd.print("n/a  ");
        }
       else
        { 
         lcd.print(alc_kub,0);
         lcd.print("%  ");
        }
      }
     //2 строчка 
     lcd.setCursor(0,1);
     if(error_t_dis==3)
      {            
       lcd.print("DS Duc error    ");
       sirena_start();
      }
     else
      {
       if(state==0)
        {
         lcd.print("BoDa OFF        ");
         return;
        }
       if(state==2)
        {
         lcd.print("BoDa ON         ");
         return;
        }
       if(state==4)
        {
         lcd.print("HEAT OFF        ");
         return;
        }
      if(state==5)
        {
         lcd.print("Wait 60c        ");
         return;
        }
       else
        {
         lcd.print("Duc ");
         lcd.print(t_dis,1);
         lcd.print(" / ");
         if(alc_dis==0)
          {
           lcd.print("n/a  ");
          }
         else
          { 
           lcd.print(alc_dis,1);
           lcd.print("%");
          }
        } 
      }     
    break;
    
    //Второй экран с дополнительной информацией
    case 2:
      if(button_state()==1){modeState=8;screen_number=1;return;}
      pressure();
      lcd.setCursor(0, 0);
      lcd.print("P=");
      lcd.print(Davlenie,0);
      lcd.print("mm dT=");
      lcd.print((760-Davlenie)*0.035);
      lcd.setCursor(0, 1);
      if(digitalRead(PIN_RELAY)==HIGH)
       {
        lcd.print("BoDa ON ");  
       }
      else
       {
        lcd.print("BoDa OFF ");  
       } 
      #ifdef TCA_USE
      lcd.print("TCA=");
      if(error_t_tca==3)
       {  
        lcd.print("err");  
       }
      else
       {
        lcd.print(t_tca,0);
        lcd.print(" ");
       }
       #endif
     quit();
    break;
   }
 }
 
 //Процедура выхода по таймауту бездействия на главный экран
 void quit(void)
 { 
  if(flag_quit==0)
   {
    flag_quit=1;
    timer_quit=millis();
    return;
   }
  if(millis()-timer_quit>10000)
   {
    modeState=0;
    screen_number=1;
    menu_number=1;
    podmenu1_number=1;
    podmenu2_number=1;
    podmenu3_number=1;
    flag_quit=0; 
    lcd.clear();
    return;
   }
  }
