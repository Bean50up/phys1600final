  #include "rgb_lcd.h"
  rgb_lcd lcd;

  float AverageValue()
  {
    int8_t readpin = 4;
    float average = 0;
    float total = 0;
    float num;
    float run_num = 500;
    for(uint16_t n = 0; n < run_num; n++)
    {
      num = analogRead(readpin);
      total = total + num;
    }
    average = total/run_num;
    return average;
  }

  float PercentSoil(float a)
  {
    float dry = 8191;
    float wet = 5000;
    float moist = 0;
    moist = ((dry-a)/(dry-wet));
    return moist;
  }
//pwm setup
  uint8_t REDPin = 7;
  uint8_t GRNPin = 6;
  uint8_t motorpin = 5;
  int PWMFreq = 7500;
  int REDChannel = 0;
  int GRNChannel = 1;
  int PWMResolution = 10;
  int MAX_DUTY_CYCLE = 1023;


  int alarmCycle = 0;
  int alarmDirection = +100;

void setup() {
  pinMode(motorpin,OUTPUT);
  pinMode(4,INPUT);
  Serial.begin(115200);  
  
  ledcSetup(REDChannel, PWMFreq, PWMResolution);
  ledcAttachPin(REDPin, REDChannel);

  ledcSetup(GRNChannel, PWMFreq, PWMResolution);
  ledcAttachPin(GRNPin, GRNChannel);


  
//intro sequence
  int redCycle = 1023;
  int greenCycle = 0;  
  lcd.begin(16,2);
  lcd.print("Initizaling...");
      while(greenCycle < MAX_DUTY_CYCLE)
    {
      redCycle--;
      greenCycle++;
      ledcWrite(GRNChannel, greenCycle);
      ledcWrite(REDChannel, redCycle);
      delay(2);
    }
      while(redCycle < MAX_DUTY_CYCLE)
    {
      greenCycle--;
      redCycle++;
      ledcWrite(GRNChannel, greenCycle);
      ledcWrite(REDChannel, redCycle);
      delay(2);
    }

  lcd.setCursor(0,0);
  lcd.print("               ");
  lcd.setCursor(0,0);
  lcd.print("Done!");
  delay(1000);
  lcd.setCursor(0,0);
  lcd.print("               ");
  lcd.setCursor(0,0);
  lcd.print("Moisture Status:");

}

void loop() {
  float average;
  float moist;
  //finds moisture level
  average = AverageValue();
  moist = PercentSoil(average);
  Serial.println(moist); 

  lcd.print("               ");
  lcd.setCursor(0,1);


  //screen msg array
  char msg[][17]=
  {
    "too much water",
    "Thats Enough!!!",
    "A Little Much",
    "Plent!!!",
    "Great!!",
    "Good!",
    "Passing",
    "Running Low",
    "Like A Desert",
    "0"
  //"1234567890123456"
  };

  char i = 0;
  for(float counter = 0.9; counter > 0; counter -= 0.1, i++)
  {
    //has moisture
    if(moist >= counter)
    {
      //screen msg
      
      lcd.print(msg[i]);
      //light change
      
      int redCycle = (1-moist*2)*1023;
      int greenCycle = moist*1023;
      ledcWrite(REDChannel, redCycle);
      ledcWrite(GRNChannel, greenCycle);  

      //pump
      digitalWrite(motorpin, LOW);
      break;
    }
    //if no moisture deteted
    if(moist < 0.1)
    {
      //screen msg
      
      lcd.print("No Moisture Dawg");
      //light change

      int greenCycle = 0;
      ledcWrite(GRNChannel, greenCycle);        

      alarmCycle += alarmDirection;
      ledcWrite(REDChannel, alarmCycle);
      if(alarmCycle > 999)
        alarmDirection = -100;
      if(alarmCycle < 1)
        alarmDirection = +100;
      //pump
      digitalWrite(motorpin, HIGH);
      break;

    }

  }


}