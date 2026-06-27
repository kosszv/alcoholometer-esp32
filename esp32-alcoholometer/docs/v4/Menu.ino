//Навигация по меню
void menuNavi(void)
 { 
  char enc = encoder_state(); 
  if(enc != 0) 
   {
    menu_number+=enc;
    if(menu_number>MENU_NUMBER-1) {menu_number=MENU_NUMBER-1;}
    else if(menu_number<0) {menu_number=0;}  
   }     
 }

//Вывод на экран пунктов меню modeState=1
void menu(void)
 {
  menuNavi();
  if(button_state()==1)
  {
   if(menu_number==0) {modeState=0;menu_number=1;screen_number=1;return;}
   if(menu_number==1) {modeState=2;return;}
   if(menu_number==2) {modeState=3;return;}
   if(menu_number==3) {modeState=4;return;}
  }
  lcd.setCursor(0,0);
  lcd.print(menu_name[menu_number]); 
  quit();
 }
