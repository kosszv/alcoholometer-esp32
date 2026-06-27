void menu()
 {
  // Листание меню
    if(Menu==1)
     {
      if(joystick()==1 && Edit==0)
       {
        Punkt_menu--;
        if (Punkt_menu<1)
         {
          Punkt_menu=20;
         }
       }
       if(joystick()==2 && Edit==0)
        {
         Punkt_menu++;
         if (Punkt_menu>20)
          {
           Punkt_menu=1;
          }
         }
       //Само меню
       switch(Punkt_menu)
       {        
        // Редактируем параметр Tvoda 
        case 1:
         if (Button==1 &&  Edit==0)
          {
           Edit=1;
           Button=0;
           dT0=Tvoda;
           }
          if (Edit==1)
           {
            if(joystick()==3)
             {
              dT0=dT0+d;
              if(dT0 >= Tmax)
               {
                dT0=Tmax;
               }
             }
           if(joystick()==4)
            {
             dT0=dT0-d;
             if(dT0 <= Tmin)
              {
               dT0=Tmin;
              }
             }
            if(dT0>=Tstop)
             {
              dT0=Tstop;
             }
            if(Button==1)
             {
              Edit=0;
              Button=0;
              if(dT0!=Tvoda)
               {
                Tvoda=dT0;
                EEFW(0, Tvoda);
               }
             }  
           }
           exit();
           // Вывод на экран информации о редактировании переменной           
           lcd.setCursor(0, 0);
           lcd.print("Tvoda ");
           if(Edit==0)
            {
             lcd.print(Tvoda,1);
             lcd.print("      ");
             lcd.setCursor(0, 1);
             lcd.print("                ");
            }
           else
            { 
             lcd.print(dT0,1);
             lcd.print("      ");
             lcd.setCursor(0, 1);
             lcd.print("edit +/-        ");
            }
        break;
             
        // Редактируем параметр Tstop
        case 2:
         if (Button==1 &&  Edit==0)
          {
           Edit=1;
           Button=0;
           dT0=Tstop;
          }
         if (Edit==1)
          {
           if(joystick()==3)
            {
             dT0=dT0+d;
             if(dT0 >= Tmax)
              {
               dT0=Tmax;
              }
            }
           if(joystick()==4)
            {
             dT0=dT0-d;
             if(dT0 <= Tmin)
              {
               dT0=Tmin;
              }
             }
            if(dT0<=Tvoda)
             {
               dT0=Tvoda;
             }
            if(Button==1)
             {
              Edit=0;
              Button=0;
              if(dT0!=Tstop)
               {
                Tstop=dT0;
                EEFW(8, Tstop);
               }
             } 
           }
           exit();
           // Вывод на экран информации о редактировании переменной           
           lcd.setCursor(0, 0);
           lcd.print("Tstop ");
           if(Edit==0)
            {
             lcd.print(Tstop,1);
             lcd.print("      ");
             lcd.setCursor(0, 1);
             lcd.print("                ");
            }
           else if(Edit==1)
            { 
             lcd.print(dT0,1);
             lcd.print("      ");
             lcd.setCursor(0, 1);
             lcd.print("edit +/-        ");
            }
        break;
          
        // Редактируем параметр Kstop
        case 3:
         if (Button==1 &&  Edit==0)
          {
           Edit=1;
           Button=0;
           dC=Kstop;
          }
         if (Edit==1)
          {
           if(joystick()==3)
            {
             dC=dC+1;
             if(dC >= Kstopmax)
              {
               dC=Kstopmax;
              }
             }
            if(joystick()==4)
             {
              dC=dC-1;
              if(dC <= Kstopmin)
               {
                dC=Kstopmin;
               }
              }
            if(Button==1)
             {
              Edit=0;
              Button=0;
              if(dC!=Kstop)
               {
                Kstop=dC;
                EEPROM.write(68, Kstop);
               }
             }  
           }
           exit();
           // Вывод на экран информации о редактировании переменной           
           lcd.setCursor(0, 0);
           lcd.print("Kstop ");
           if(Edit==0)
            {
             lcd.print(Kstop,0);
             lcd.print("%       ");
             lcd.setCursor(0, 1);
             lcd.print("                ");
            }
           else
            { 
             lcd.print(dC);
             lcd.print("%        ");
             lcd.setCursor(0, 1);
             lcd.print("edit +/-        ");
            }
        break;
           
        // Редактируем параметр Time_sirena - время писка при достижении Tvoda
        case 4:
         if (Button==1 &&  Edit==0)
          {
           Edit=1;
           Button=0;
           dC=Time_sirena;
           }
          if (Edit==1)
           {
            if(joystick()==3)
             {
              dC=dC+5;
              if(dC >= 250)
             {
              dC=250;
             }
           }
        if(joystick()==4)
         {
          dC=dC-5;
          if(dC <= 10)
           {
            dC=10;
            }
           }
            if(Button==1)
             {
              Edit=0;
              Button=0;
              if(dC!=Time_sirena)
               {
                Time_sirena=dC;
                EEPROM.write(4, Time_sirena);
               }
             }  
           }
           exit();
           // Вывод на экран информации о редактировании переменной           
           lcd.setCursor(0, 0);
           lcd.print("Time sirena ");
           if(Edit==0)
            {
             lcd.print(Time_sirena);
             lcd.print("c  ");
             lcd.setCursor(0, 1);
             lcd.print("                ");
            }
           else
            { 
             lcd.print(dC);
             lcd.print("c  ");
             lcd.setCursor(0, 1);
             lcd.print("edit +/-        ");
            }
        break;
          
        // Редактируем параметр Emkost (использовать ли датчик в приемной емкости)
        case 5:
         if (Button==1 &&  Edit==0)
          {
           Edit=1;
           Button=0;
           dC=Emkost;
           }
          if (Edit==1)
           {
            if(joystick()==3)
             {
              dC=1;
             }
            if(joystick()==4)
             {
              dC=0;
             }
           if(Button==1)
            {
             Edit=0;
             Button=0;
             if(dC!=Emkost)
              {
               Emkost=dC;
               EEPROM.write(69, Emkost);
              }
            }  
           }
           exit();
           // Вывод на экран информации о редактировании переменной           
           lcd.setCursor(0, 0);
           lcd.print("DaT4uk eMkocTu ");
           if(Edit==0)
            {
             if(Emkost==0)
              {
               lcd.print("-");
              }
             else
              {
               lcd.print("+");
              }
             }
            else
             {
              if(dC==0)
               {
                lcd.print("-");
               }
              else
               {
                lcd.print("+");
               }
              }
           lcd.setCursor(0, 1);
           if(Edit==1)
            {
             lcd.print("edit +/-        ");
            }
           else
            {
             lcd.print("                ");
            }
        break;
          
        // Редактируем параметр Water (использовать ли датчик разлития воды)
        case 6:
         if (Button==1 &&  Edit==0)
          {
           Edit=1;
           Button=0;
           dC=Water;
           }
          if (Edit==1)
           {
            if(joystick()==3)
             {
              dC=1;
             }
            if(joystick()==4)
             {
              dC=0;
             }
            if(Button==1)
             {
              Edit=0;
              Button=0;
              if(dC!=Water)
               {
                Water=dC;
                EEPROM.write(70, Water);
               }
             }  
           }
           exit();
           // Вывод на экран информации о редактировании переменной           
           lcd.setCursor(0, 0);
           lcd.print("DaT4uk BoDbI ");
           if(Edit==0)
            {
             if(Water==0)
              {
               lcd.print("-  ");
              }
             else
              {
               lcd.print("+  ");
              }
             }
            else
             {
              if(dC==0)
               {
                lcd.print("-  ");
               }
              else
               {
                lcd.print("+  ");
               }
              }
           lcd.setCursor(0, 1);
           if(Edit==1)
            {
             lcd.print("edit +/-        ");
            }
           else
            {
             lcd.print("                ");
            }
        break;
         
        // Редактируем параметр Stop (как заканчивать отбор - по Ткуба или по крепости погона)
        case 7:
         if (Button==1 &&  Edit==0)
          {
           Edit=1;
           Button=0;
           dC=Stop;
           }
          if (Edit==1)
           {
            if(joystick()==3)
             {
              dC=1;
             }
            if(joystick()==4)
             {
              dC=0;
             }
            if(Button==1)
             {
              Edit=0;
              Button=0;
              if(dC!=Stop)
               {
                Stop=dC;
                EEPROM.write(71, Stop);
               }
             }  
           }
           exit();
           // Вывод на экран информации о редактировании переменной           
           lcd.setCursor(0, 0);
           lcd.print("cTon no ");
           if(Edit==0)
            {
             if(Stop==0)
              {
               lcd.print("Tkyba   ");
              }
             else
              {
               lcd.print("kpenocTu");
              }
             }
            else
             {
              if(dC==0)
               {
                lcd.print("Tkyba   ");
               }
              else
               {
                lcd.print("kpenocTu");
               }
             }
           lcd.setCursor(0, 1);
           if(Edit==1)
            {
             lcd.print("edit +/-        ");
            }
           else
            {
             lcd.print("                ");
            }
        break;
         
//        // Редактируем параметр dTD78_81(поправку на диапозоне 78-81*С)
//        case 8:
//         if (Button==1 &&  Edit==0)
//          {
//           Edit=1;
//           Button=0;
//           dT0=dTD78_81;
//           }
//          if (Edit==1)
//           {
//            if(joystick()==3)
//             {
//              dT0=dT0+d;
//              if(dT0 >= dTDmax)
//               {
//                dT0=dTDmax;
//               }
//              }
//            if(joystick()==4)
//             {
//              dT0=dT0-d;
//              if(dT0 <= dTDmin)
//               {
//                dT0=dTDmin;
//               }
//             }
//            if(Button==1)
//             {
//              Edit=0;
//              Button=0;
//              if(dT0!=dTD78_81)
//               {
//                dTD78_81=dT0;
//                EEFW(12, dTD78_81);
//               }
//             }  
//           }
//           exit();
//           // Вывод на экран информации о редактировании переменной           
//           lcd.setCursor(0, 0);
//           lcd.print("dTD78_81 ");
//           if(Edit==0)
//            {
//             lcd.print(dTD78_81,1);
//             lcd.print("    ");
//             lcd.setCursor(0, 1);
//             lcd.print("                ");
//            }
//           else
//            { 
//             lcd.print(dT0,1);
//             lcd.setCursor(0, 1);
//             lcd.print("edit +/-        ");
//            }            
//        break;
          
// Редактируем параметр dTD81_84(поправку на диапозоне 81-84*С)
        case 8:
         if (Button==1 &&  Edit==0)
          {
           Edit=1;
           Button=0;
           dT0=dTD81_84;
           }
          if (Edit==1)
           {
            if(joystick()==3)
             {
              dT0=dT0+d;
              if(dT0 >= dTDmax)
               {
                dT0=dTDmax;
               }
              }
            if(joystick()==4)
             {
              dT0=dT0-d;
              if(dT0 <= dTDmin)
               {
                dT0=dTDmin;
               }
             }
            if(Button==1)
             {
              Edit=0;
              Button=0;
              if(dT0!=dTD81_84)
               {
                dTD81_84=dT0;
                EEFW(16, dTD81_84);
               }
             }  
           }
           exit();
           // Вывод на экран информации о редактировании переменной           
           lcd.setCursor(0, 0);
           lcd.print("dTD81_84 ");
           if(Edit==0)
            {
             lcd.print(dTD81_84,1);
             lcd.print("    ");
             lcd.setCursor(0, 1);
             lcd.print("                ");
            }
           else
            { 
             lcd.print(dT0,1);
             lcd.setCursor(0, 1);
             lcd.print("edit +/-        ");
            }
        break; 
          
        // Редактируем параметр dTD84_87(поправку на диапозоне 84-87*С)
        case 9:
         if (Button==1 &&  Edit==0)
          {
           Edit=1;
           Button=0;
           dT0=dTD84_87;
           }
          if (Edit==1)
           {
            if(joystick()==3)
             {
              dT0=dT0+d;
              if(dT0 >= dTDmax)
               {
                dT0=dTDmax;
               }
              }
            if(joystick()==4)
             {
              dT0=dT0-d;
              if(dT0 <= dTDmin)
               {
                dT0=dTDmin;
               }
             }
            if(Button==1)
             {
              Edit=0;
              Button=0;
              if(dT0!=dTD84_87)
               {
                dTD84_87=dT0;
                EEFW(20, dTD84_87);
               }
             }  
           }
           exit();
           // Вывод на экран информации о редактировании переменной           
           lcd.setCursor(0, 0);
           lcd.print("dTD84_87 ");
           if(Edit==0)
            {
             lcd.print(dTD84_87,1);
             lcd.print("    ");
             lcd.setCursor(0, 1);
             lcd.print("                ");
            }
           else
            { 
             lcd.print(dT0,1);
             lcd.setCursor(0, 1);
             lcd.print("edit +/-        ");
            }
        break;
          
        // Редактируем параметр dTD87_90(поправку на диапозоне 87-90*С)
        case 10:
         if (Button==1 &&  Edit==0)
          {
           Edit=1;
           Button=0;
           dT0=dTD87_90;
           }
          if (Edit==1)
           {
            if(joystick()==3)
             {
              dT0=dT0+d;
              if(dT0 >= dTDmax)
               {
                dT0=dTDmax;
               }
              }
            if(joystick()==4)
             {
              dT0=dT0-d;
              if(dT0 <= dTDmin)
               {
                dT0=dTDmin;
               }
             }
            if(Button==1)
             {
              Edit=0;
              Button=0;
              if(dT0!=dTD87_90)
               {
                dTD87_90=dT0;
                EEFW(24, dTD87_90);
               }
             }  
           }
           exit();
           // Вывод на экран информации о редактировании переменной           
           lcd.setCursor(0, 0);
           lcd.print("dTD87_90 ");
           if(Edit==0)
            {
             lcd.print(dTD87_90,1);
             lcd.print("    ");
             lcd.setCursor(0, 1);
             lcd.print("                ");
            }
           else
            { 
             lcd.print(dT0,1);
             lcd.setCursor(0, 1);
             lcd.print("edit +/-        ");
            }
        break;
          
        // Редактируем параметр dTD90_91(поправку на диапозоне 90-91*С)
        case 11:
         if (Button==1 &&  Edit==0)
          {
           Edit=1;
           Button=0;
           dT0=dTD90_91;
           }
          if (Edit==1)
           {
            if(joystick()==3)
             {
              dT0=dT0+d;
              if(dT0 >= dTDmax)
               {
                dT0=dTDmax;
               }
              }
            if(joystick()==4)
             {
              dT0=dT0-d;
              if(dT0 <= dTDmin)
               {
                dT0=dTDmin;
               }
             }
            if(Button==1)
             {
              Edit=0;
              Button=0;
              if(dT0!=dTD90_91)
               {
                dTD90_91=dT0;
                EEFW(28, dTD90_91);
               }
             }  
           }
           exit();
           // Вывод на экран информации о редактировании переменной           
           lcd.setCursor(0, 0);
           lcd.print("dTD90_91 ");
           if(Edit==0)
            {
             lcd.print(dTD90_91,1);
             lcd.print("    ");
             lcd.setCursor(0, 1);
             lcd.print("                ");
            }
           else
            { 
             lcd.print(dT0,1);
             lcd.setCursor(0, 1);
             lcd.print("edit +/-        ");
            }
          break;
          
        // Редактируем параметр dTD91_92(поправку на диапозоне 91-92*С)
        case 12:
         if (Button==1 &&  Edit==0)
          {
           Edit=1;
           Button=0;
           dT0=dTD91_92;
           }
          if (Edit==1)
           {
            if(joystick()==3)
             {
              dT0=dT0+d;
              if(dT0 >= dTDmax)
               {
                dT0=dTDmax;
               }
              }
            if(joystick()==4)
             {
              dT0=dT0-d;
              if(dT0 <= dTDmin)
               {
                dT0=dTDmin;
               }
             }
            if(Button==1)
             {
              Edit=0;
              Button=0;
              if(dT0!=dTD91_92)
               {
                dTD91_92=dT0;
                EEFW(32, dTD91_92);
               }
             }  
           }
           exit();
           // Вывод на экран информации о редактировании переменной           
           lcd.setCursor(0, 0);
           lcd.print("dTD91_92 ");
           if(Edit==0)
            {
             lcd.print(dTD91_92,1);
             lcd.print("    ");
             lcd.setCursor(0, 1);
             lcd.print("                ");
            }
           else
            { 
             lcd.print(dT0,1);
             lcd.setCursor(0, 1);
             lcd.print("edit +/-        ");
            }
        break;
          
        // Редактируем параметр dTD92_93(поправку на диапозоне 92-93*С)
        case 13:
        if (Button==1 &&  Edit==0)
          {
           Edit=1;
           Button=0;
           dT0=dTD92_93;
           }
          if (Edit==1)
           {
            if(joystick()==3)
             {
              dT0=dT0+d;
              if(dT0 >= dTDmax)
               {
                dT0=dTDmax;
               }
              }
            if(joystick()==4)
             {
              dT0=dT0-d;
              if(dT0 <= dTDmin)
               {
                dT0=dTDmin;
               }
             }
            if(Button==1)
             {
              Edit=0;
              Button=0;
              if(dT0!=dTD92_93)
               {
                dTD92_93=dT0;
                EEFW(36, dTD92_93);
               }
             }  
           }
           exit();
           // Вывод на экран информации о редактировании переменной           
           lcd.setCursor(0, 0);
           lcd.print("dTD92_93 ");
           if(Edit==0)
            {
             lcd.print(dTD92_93,1);
             lcd.print("    ");
             lcd.setCursor(0, 1);
             lcd.print("                ");
            }
           else
            { 
             lcd.print(dT0,1);
             lcd.setCursor(0, 1);
             lcd.print("edit +/-        ");
            }
        break;
          
        // Редактируем параметр dTD93_94(поправку на диапозоне 93-94*С)
        case 14:
         if (Button==1 &&  Edit==0)
          {
           Edit=1;
           Button=0;
           dT0=dTD93_94;
           }
          if (Edit==1)
           {
            if(joystick()==3)
             {
              dT0=dT0+d;
              if(dT0 >= dTDmax)
               {
                dT0=dTDmax;
               }
              }
            if(joystick()==4)
             {
              dT0=dT0-d;
              if(dT0 <= dTDmin)
               {
                dT0=dTDmin;
               }
             }
            if(Button==1)
             {
              Edit=0;
              Button=0;
              if(dT0!=dTD93_94)
               {
                dTD93_94=dT0;
                EEFW(40, dTD93_94);
               }
             }  
           }
           exit();
           // Вывод на экран информации о редактировании переменной           
           lcd.setCursor(0, 0);
           lcd.print("dTD93_94 ");
           if(Edit==0)
            {
             lcd.print(dTD93_94,1);
             lcd.print("    ");
             lcd.setCursor(0, 1);
             lcd.print("                ");
            }
           else
            { 
             lcd.print(dT0,1);
             lcd.setCursor(0, 1);
             lcd.print("edit +/-        ");
            }
        break;
          
        // Редактируем параметр dTD94_95(поправку на диапозоне 94-95*С)
        case 15:
         if (Button==1 &&  Edit==0)
          {
           Edit=1;
           Button=0;
           dT0=dTD94_95;
           }
          if (Edit==1)
           {
            if(joystick()==3)
             {
              dT0=dT0+d;
              if(dT0 >= dTDmax)
               {
                dT0=dTDmax;
               }
              }
            if(joystick()==4)
             {
              dT0=dT0-d;
              if(dT0 <= dTDmin)
               {
                dT0=dTDmin;
               }
             }
            if(Button==1)
             {
              Edit=0;
              Button=0;
              if(dT0!=dTD94_95)
               {
                dTD94_95=dT0;
                EEFW(44, dTD94_95);
               }
             }  
           }
           exit();
           // Вывод на экран информации о редактировании переменной           
           lcd.setCursor(0, 0);
           lcd.print("dTD94_95 ");
           if(Edit==0)
            {
             lcd.print(dTD94_95,1);
             lcd.print("    ");
             lcd.setCursor(0, 1);
             lcd.print("                ");
            }
           else
            { 
             lcd.print(dT0,1);
             lcd.setCursor(0, 1);
             lcd.print("edit +/-        ");
            }
        break;
          
        // Редактируем параметр dTD95_96(поправку на диапозоне 95-96*С)
        case 16:
          if (Button==1 &&  Edit==0)
          {
           Edit=1;
           Button=0;
           dT0=dTD95_96;
           }
          if (Edit==1)
           {
            if(joystick()==3)
             {
              dT0=dT0+d;
              if(dT0 >= dTDmax)
               {
                dT0=dTDmax;
               }
              }
            if(joystick()==4)
             {
              dT0=dT0-d;
              if(dT0 <= dTDmin)
               {
                dT0=dTDmin;
               }
             }
            if(Button==1)
             {
              Edit=0;
              Button=0;
              if(dT0!=dTD95_96)
               {
                dTD95_96=dT0;
                EEFW(48, dTD95_96);
               }
             }  
           }
           exit();
           // Вывод на экран информации о редактировании переменной           
           lcd.setCursor(0, 0);
           lcd.print("dTD95_96 ");
           if(Edit==0)
            {
             lcd.print(dTD95_96,1);
             lcd.print("    ");
             lcd.setCursor(0, 1);
             lcd.print("                ");
            }
           else
            { 
             lcd.print(dT0,1);
             lcd.setCursor(0, 1);
             lcd.print("edit +/-        ");
            }
        break;
          
        // Редактируем параметр dTD96_97(поправку на диапозоне 96-97*С)
        case 17:
          if (Button==1 &&  Edit==0)
          {
           Edit=1;
           Button=0;
           dT0=dTD96_97;
           }
          if (Edit==1)
           {
            if(joystick()==3)
             {
              dT0=dT0+d;
              if(dT0 >= dTDmax)
               {
                dT0=dTDmax;
               }
              }
            if(joystick()==4)
             {
              dT0=dT0-d;
              if(dT0 <= dTDmin)
               {
                dT0=dTDmin;
               }
             }
            if(Button==1)
             {
              Edit=0;
              Button=0;
              if(dT0!=dTD96_97)
               {
                dTD96_97=dT0;
                EEFW(52, dTD96_97);
               }
             }  
           }
           exit();
           // Вывод на экран информации о редактировании переменной           
           lcd.setCursor(0, 0);
           lcd.print("dTD96_97 ");
           if(Edit==0)
            {
             lcd.print(dTD96_97,1);
             lcd.print("    ");
             lcd.setCursor(0, 1);
             lcd.print("                ");
            }
           else
            { 
             lcd.print(dT0,1);
             lcd.setCursor(0, 1);
             lcd.print("edit +/-        ");
            }
        break;
          
        // Редактируем параметр dTD97_98(поправку на диапозоне 97-98*С)
        case 18:
         if (Button==1 &&  Edit==0)
          {
           Edit=1;
           Button=0;
           dT0=dTD97_98;
           }
          if (Edit==1)
           {
            if(joystick()==3)
             {
              dT0=dT0+d;
              if(dT0 >= dTDmax)
               {
                dT0=dTDmax;
               }
              }
            if(joystick()==4)
             {
              dT0=dT0-d;
              if(dT0 <= dTDmin)
               {
                dT0=dTDmin;
               }
             }
            if(Button==1)
             {
              Edit=0;
              Button=0;
              if(dT0!=dTD97_98)
               {
                dTD97_98=dT0;
                EEFW(56, dTD97_98);
               }
             }  
           }
           exit();
           // Вывод на экран информации о редактировании переменной           
           lcd.setCursor(0, 0);
           lcd.print("dTD97_98 ");
           if(Edit==0)
            {
             lcd.print(dTD97_98,1);
             lcd.print("    ");
             lcd.setCursor(0, 1);
             lcd.print("                ");
            }
           else
            { 
             lcd.print(dT0,1);
             lcd.setCursor(0, 1);
             lcd.print("edit +/-        ");
            }
          break;
          
// Редактируем параметр dTD98_99(поправку на диапозоне 98-99*С)
        case 19:
         if (Button==1 &&  Edit==0)
          {
           Edit=1;
           Button=0;
           dT0=dTD98_99;
           }
          if (Edit==1)
           {
            if(joystick()==3)
             {
              dT0=dT0+d;
              if(dT0 >= dTDmax)
               {
                dT0=dTDmax;
               }
              }
            if(joystick()==4)
             {
              dT0=dT0-d;
              if(dT0 <= dTDmin)
               {
                dT0=dTDmin;
               }
             }
            if(Button==1)
             {
              Edit=0;
              Button=0;
              if(dT0!=dTD98_99)
               {
                dTD98_99=dT0;
                EEFW(60, dTD98_99);
               }
             }  
           }
           exit();
           // Вывод на экран информации о редактировании переменной           
           lcd.setCursor(0, 0);
           lcd.print("dTD98_99 ");
           if(Edit==0)
            {
             lcd.print(dTD98_99,1);
             lcd.print("    ");
             lcd.setCursor(0, 1);
             lcd.print("                ");
            }
           else
            { 
             lcd.print(dT0,1);
             lcd.setCursor(0, 1);
             lcd.print("edit +/-        ");
            }
          break;
          
// Редактируем параметр dTD99_100(поправку на диапозоне 99-100*С)
        case 20:
         if (Button==1 &&  Edit==0)
          {
           Edit=1;
           Button=0;
           dT0=dTD99_100;
           }
          if (Edit==1)
           {
           if(joystick()==3)
             {
              dT0=dT0+d;
              if(dT0 >= dTDmax)
               {
                dT0=dTDmax;
               }
              }
            if(joystick()==4)
             {
              dT0=dT0-d;
              if(dT0 <= dTDmin)
               {
                dT0=dTDmin;
               }
             }
            if(Button==1)
             {
              Edit=0;
              Button=0;
              if(dT0!=dTD99_100)
               {
                dTD99_100=dT0;
                EEFW(64, dTD99_100);
               }
             }  
           }
           exit();
           // Вывод на экран информации о редактировании переменной           
           lcd.setCursor(0, 0);
           lcd.print("dTD99_100 ");
           if(Edit==0)
            {
             lcd.print(dTD99_100,1);
             lcd.print("    ");
             lcd.setCursor(0, 1);
             lcd.print("                ");
            }
           else
            { 
             lcd.print(dT0,1);
             lcd.setCursor(0, 1);
             lcd.print("edit +/-        ");
            }
          break;
         }
     }                 
 }
