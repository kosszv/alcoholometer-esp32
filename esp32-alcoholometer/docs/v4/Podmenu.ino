//Навигация по подменю 1
void podmenu1Navi(void)
 {
  char enc = encoder_state(); 
  if(enc != 0) 
   {
    podmenu1_number+=enc;
    if(podmenu1_number>PARAMS1_NUMBER-1) {podmenu1_number=PARAMS1_NUMBER-1;}
    else if(podmenu1_number<0) {podmenu1_number=0;}  
   }     
 }
 
//Навигация по подменю 2
void podmenu2Navi(void)
 {
  char enc = encoder_state(); 
  if(enc != 0) 
   {
    podmenu2_number+=enc;
    if(podmenu2_number>PARAMS2_NUMBER-1) {podmenu2_number=PARAMS2_NUMBER-1;}
    else if(podmenu2_number<0) {podmenu2_number=0;}  
   }      
 }

//Навигация по подменю 3
void podmenu3Navi(void)
 {
  char enc = encoder_state(); 
  if(enc != 0) 
   {
    podmenu3_number+=enc;
    if(podmenu3_number>PARAMS3_NUMBER-1) {podmenu3_number=PARAMS3_NUMBER-1;}
    else if(podmenu3_number<0) {podmenu3_number=0;}  
   }     
 }
 
//Вывод на экран пунктов подменю 1 modeState=2
void podmenu1(void)
 {
  podmenu1Navi();
  if(button_state()==1)
  {
   if(podmenu1_number==0) {modeState=1;podmenu1_number=1;return;}
   else {modeState=5;temp1=param1[podmenu1_number]; return;}
  }
  if(podmenu1_number>0)
  {
   lcd.setCursor(0,0);
   lcd.print(param1_name[podmenu1_number]);
   lcd.setCursor(0,1);
   lcd.print(param1[podmenu1_number]); 
  }
  else
  {
   lcd.setCursor(0,0);
   lcd.print(param1_name[podmenu1_number]); 
  }
  quit();
 }

//Вывод на экран пунктов подменю 2 modeState=3
void podmenu2(void)
 {
  podmenu2Navi();
  if(button_state()==1)
  {
   if(podmenu2_number==0) {modeState=1;podmenu2_number=1;return;}
   else {modeState=6;temp2=param2[podmenu2_number];return;}
  }

  if(podmenu2_number>0)
  {
   lcd.setCursor(0,0);
   lcd.print(param2_name[podmenu2_number]);
   lcd.setCursor(0,1);
   lcd.print(param2[podmenu2_number],1); 
  }
  else
  {
   lcd.setCursor(0,0);
   lcd.print(param2_name[podmenu2_number]); 
  }
  quit();
 }
 
//Вывод на экран пунктов подменю 3 modeState=4
void podmenu3(void)
 {
  podmenu3Navi();
  if(button_state()==1)
  {
   if(podmenu3_number==0) {modeState=1;podmenu3_number=1;return;}
   else {modeState=7;temp3=param3[podmenu3_number];return;}
  }
  if(podmenu3_number>0)
  {
   lcd.setCursor(0,0);
   lcd.print(param3_name[podmenu3_number]);
   lcd.setCursor(0,1);
   lcd.print(param3[podmenu3_number],2); 
  }
  else
  {
   lcd.setCursor(0,0);
   lcd.print(param3_name[podmenu3_number]); 
  }
  quit();
 }
