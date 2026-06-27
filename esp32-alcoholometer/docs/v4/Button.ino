//--------------------------------------------Кнопка-------------------------------------------------------//
//Обработчик события нажатия
void button(void)
 {
  button_pressed = 1; 
 }
 
//Функция состояния кнопки 1-была нажата
byte button_state()
{
 if (button_pressed == 1)
 {
  button_pressed = 0;
  flag_quit=0;
  lcd.clear();
  return 1;
 }
 else
 {
  return 0;
 }
}


