//Изменение параметра подменю 1
void edit1Navi(void)
 {
  char enc = encoder_state(); 
  if(enc != 0) 
   {
    //Редактирование параметров значения которых 0-1
    if(podmenu1_number>0 && podmenu1_number<=3)
     {
      temp1+=enc;
      if(temp1>1) {temp1=1;}
      else if(temp1<1) {temp1=0;}
     }
    //Редактирование параметра Kstop(0-100)
    if(podmenu1_number==4)
     { 
      temp1+=enc;
      if(temp1>PARAM1_1_MAX) {temp1=PARAM1_1_MAX;}
      else if(temp1<1) {temp1=0;}
     }
    //Редактирование параметра time_refresh и time_sirena(1-60)
    if(podmenu1_number==5||podmenu1_number==7)
     { 
      temp1+=enc;
      if(temp1>PARAM1_2_MAX) {temp1=PARAM1_2_MAX;}
      else if(temp1<2) {temp1=1;}
     }
    //Редактирование параметра Time bluetooth(0-60сек)(0-выключено)
    if(podmenu1_number==6)
     { 
      temp1+=enc;
      if(temp1>PARAM1_2_MAX) {temp1=PARAM1_2_MAX;}
      else if(temp1<1) {temp1=0;}
     }
   } 
 }

//Изменение параметра подменю 2
void edit2Navi(void)
 {
  char enc = encoder_state(); 
  if(enc != 0) 
   {
    temp2+=enc*0.1;
    if(temp2<PARAM2_MIN) {temp2=PARAM2_MIN;}
    else if(temp2>PARAM2_MAX) {temp2=PARAM2_MAX;}    
   } 
 }

//Изменение параметра подменю 3
void edit3Navi(void)
 {
  char enc = encoder_state(); 
  if(enc != 0) 
   {   
    temp3+=enc*0.05;
    if(temp3<PARAM3_MIN) {temp3=PARAM3_MIN;}
    else if(temp3>PARAM3_MAX) {temp3=PARAM3_MAX;}
   } 
 }
 
//Редактирование параметров подменю 1 modeState=5
void edit1(void)
 {
  edit1Navi();
  if(button_state()==1)
   {
    if(temp1!=param1[podmenu1_number])
     {
      param1[podmenu1_number]=temp1;
      EEPROM.write(podmenu1_number, param1[podmenu1_number]);
     }
  modeState=2;
  return;
 }
  lcd.setCursor(0,0);
  lcd.print(param1_name[podmenu1_number]);
  lcd.setCursor(0,1);
  lcd.print(temp1);
  lcd.setCursor(15,1);
  lcd.print("*");
 }
 
//Редактирование параметров подменю 2 modeState=6
void edit2(void)
 {
  edit2Navi();
  if(button_state()==1)
   {
    if(temp2!=param2[podmenu2_number])
     {
      param2[podmenu2_number]=temp2;
      EEFW(podmenu2_number*4+6, param2[podmenu2_number]);
     }
  modeState=3;
  return;
 }
  lcd.setCursor(0,0);
  lcd.print(param2_name[podmenu2_number]);
  lcd.setCursor(0,1);
  lcd.print(temp2,1); 
  lcd.setCursor(15,1);
  lcd.print("*");
 }
 
//Редактирование параметров подменю 3 modeState=7
void edit3(void)
 {
  edit3Navi();
  if(button_state()==1)
   {
    if(temp3!=param3[podmenu3_number])
     {
      param3[podmenu3_number]=temp3;
      EEFW(podmenu3_number*4+14, param3[podmenu3_number]);
     }
  modeState=4;
  return;
 }
  lcd.setCursor(0,0);
  lcd.print(param3_name[podmenu3_number]);
  lcd.setCursor(0,1);
  lcd.print(temp3,2); 
  lcd.setCursor(15,1);
  lcd.print("*");
 }
