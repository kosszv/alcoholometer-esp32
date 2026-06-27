void sirena_start()
 {
  switch(sirena_state)
   {
    case 0:     
     timer_sirena_state=millis();
     sirena_state=1;
    break;
    
    case 1:
     tone(PIN_SIRENA, 800);
     if(millis()-timer_sirena_state>1000)
      {       
       timer_sirena_state=millis(); 
       sirena_state=2;
      }
    break;
  
    case 2:
     noTone(PIN_SIRENA);  
     if(millis()-timer_sirena_state>500)
      {
       timer_sirena_state=millis(); 
       sirena_state=1;  
      }
    break;  
   }
 }
 

void sirena_stop()
 {
  noTone(PIN_SIRENA);
 }
