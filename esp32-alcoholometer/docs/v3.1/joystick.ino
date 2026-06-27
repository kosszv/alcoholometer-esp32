byte joystick() //0 - джойстик не двигается, 1 - влево, 2 - вправо, 3 - вверх, 4 - вниз              
 {  
  // Работа джойстика
  X = analogRead(pin_X);                             // Читаем положение джойстика по оси X
  Y = analogRead(pin_Y);                             // Читаем положение джойстика по оси Y
  if(X<400) return 1;
  if(X>600) return 2;
  if(Y<400) return 3;
  if(Y>600) return 4;
  else return 0;
 }
 
void button()
 {
  Button = digitalRead(pin_Z);                    // Читаем нажата ли кнопка на джойстике
  Button = Button ^ 1;                                 // Инвертируем сигнал нажатия кнопки джойстика 
 }
