void Bluetooth()
 {
  if(alarm()==0&&error_t_kub<3&&error_t_dis<3&&state!=2&&state!=4)
   { 
    if(param1[6]>0)
     { 
      if((millis()-bluetooth_refresh)>param1[6]*1000)
       {
        bluetooth_data();
        bluetooth_refresh=millis();
       } 
     }
    else
     {
      if (bluetooth.available() > 0) 
       {  
        incomingByte = bluetooth.read();
        delay(100);
        switch (incomingByte) 
        {
         case '1': //Данные для приложения
          bluetooth_data();
         break;  
        }
       }
      }
    } 
 }

void bluetooth_data()
 {
  bluetooth.print(" Куб "); 
  bluetooth.print(t_kub,1);
  bluetooth.print(" *С  "); 
  bluetooth.print(alc_kub,0);
  bluetooth.println("%");
  bluetooth.print("Дис "); 
  bluetooth.print(t_dis,1);
  bluetooth.print(" *С  ");
  bluetooth.print(alc_dis,1);
  bluetooth.println("%");
 }
