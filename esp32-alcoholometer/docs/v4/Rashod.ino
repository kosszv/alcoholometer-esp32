void Rashod()
 {
  if(button_state()==1){modeState=0;return;}   
  NbTopsFan = 0;
  delay (1000);   
  rashod = (NbTopsFan * 60 / 86);
  lcd.setCursor(0, 0);
  lcd.print("PacxoD water   ");
  lcd.setCursor(0, 1);
  lcd.print(rashod);
  lcd.print(" l/h           ");
 }
 
ISR (ANALOG_COMP_vect) 
{
 NbTopsFan++;
}


