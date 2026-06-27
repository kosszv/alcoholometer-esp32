//Процедура записи параметров в еепром
 void eeprom()
 {
  //Запись первоначальных параметров в еепром
  EEFW(0, Tvoda);
  EEPROM.write(4, Time_sirena);
  EEFW(8, Tstop);
//  EEFW(12, dTD78_81);
  EEFW(16, dTD81_84);
  EEFW(20, dTD84_87);
  EEFW(24, dTD87_90);
  EEFW(28, dTD90_91);
  EEFW(32, dTD91_92);
  EEFW(36, dTD92_93);
  EEFW(40, dTD93_94);
  EEFW(44, dTD94_95);
  EEFW(48, dTD95_96);
  EEFW(52, dTD96_97);
  EEFW(56, dTD97_98);
  EEFW(60, dTD98_99);
  EEFW(64, dTD99_100);
  EEPROM.write(68, Kstop);
  EEPROM.write(69, Emkost);
  EEPROM.write(70, Water);
  EEPROM.write(71, Stop);
 }
 
 //Процедура записи в еепром числа с плавающей точкой
  void EEFW(int addr, double val) 
       { 
	byte *x = (byte *)&val;
	for(byte i = 0; i < 4; i++) EEPROM.write(i+addr, x[i]);
       }
       
//Процедура чтения из еепром числа с плавающей точкой
  double EEFR(int addr) // чтение из ЕЕПРОМ
       {   
	  byte x[4];
	  for(byte i = 0; i < 4; i++) x[i] = EEPROM.read(i+addr);
	  double *y = (double *)&x;
	  return y[0];
	}  
