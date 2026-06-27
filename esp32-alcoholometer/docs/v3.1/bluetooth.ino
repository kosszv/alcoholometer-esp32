void bluetooth()
 {
  if (Serial.available() > 0) 
   {  
    incomingByte = Serial.read();
    delay(100);
    switch (incomingByte) 
    {
     case '1': //Данные для приложения
      Serial.print("Куб "); 
      Serial.print(TempK,1);
      Serial.print(" *С  "); 
      Serial.print(K1,0);
      Serial.println("%");
      Serial.print("Дис "); 
      Serial.print(TempD2,1);
      Serial.print(" *С  ");
      Serial.print(K2,0);
      Serial.println("%");
     break;  
  }
 } 
} 
 
