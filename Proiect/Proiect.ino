#include <IRremote.h>
#include <SoftwareSerial.h>

SoftwareSerial blu(8, 9); // RX | TX

#define mpin00 5
#define mpin01 6
#define mpin10 3
#define mpin11 10
#define otlpin 13
#define otcpin 12
#define otrpin 11
#define sendpin 2
#define validaddress 1234
#define command 1
#define repeats 0

int speed1 = 128;
int speed2 = 128;
int speedturn1 = 128;
int speedturn2 = 128;
int otl;
int otc;
int otr;
int prevotc = -1;
int duration = 500;
int gotodispenser = 0;
int active = 0;
String blucommand = "";

void setup() {
  digitalWrite(mpin00, 0);
  digitalWrite(mpin01, 0);
  digitalWrite(mpin10, 0);
  digitalWrite(mpin11, 0);
  pinMode (mpin00, OUTPUT);
  pinMode (mpin01, OUTPUT);
  pinMode (mpin10, OUTPUT);
  pinMode (mpin11, OUTPUT);
  pinMode (otlpin, INPUT);
  pinMode (otcpin, INPUT);
  pinMode (otrpin, INPUT);
  Serial.begin(9600);
  blu.begin(9600);
  IrSender.begin(sendpin, DISABLE_LED_FEEDBACK);
  delay(2000);
}

void StartMotor (int m1, int m2, int forward, int speed)
{
  if (speed == 0)
  {
    digitalWrite(m1, 0);
    digitalWrite(m2, 0);
  }
  else
  {
    if (forward)
    {
      digitalWrite(m2, 0);
      analogWrite(m1, speed);
    }
    else
    {
      digitalWrite(m1, 0);
      analogWrite(m2, speed);
    }
  }
}

void stopMotors(int duration)
{
  StartMotor (mpin00, mpin01, 0, 0);
  StartMotor (mpin10, mpin11, 0, 0);
  delay(duration);
}

void goForward(int speed1, int speed2, int duration)
{
  StartMotor (mpin00, mpin01, 1, speed1);
  StartMotor (mpin10, mpin11, 1, speed2);
  delay (duration);
}

void goBackward(int speed1, int speed2, int duration)
{
  StartMotor (mpin00, mpin01, 0, speed1);
  StartMotor (mpin10, mpin11, 0, speed2);
  delay (duration);
}

void goLeft(int speed1, int speed2, int duration)
{
  StartMotor (mpin00, mpin01, 0, speed1);
  StartMotor (mpin10, mpin11, 1, speed2);
  delay (duration);
}

void goRight(int speed1, int speed2, int duration)
{
  StartMotor (mpin00, mpin01, 1, speed1);
  StartMotor (mpin10, mpin11, 0, speed2);
  delay (duration);
}

void loop() {
  otl = digitalRead(otlpin);
  otc = digitalRead(otcpin);
  otr = digitalRead(otrpin);
  Serial.print(otl);
  Serial.print(' ');
  Serial.print(otc);
  Serial.print(' ');
  Serial.println(otr);
  if (blu.available())
  {
    char character = blu.read();
    blucommand.concat(character);
    if (character == '\n')
    {
      Serial.print("Received: ");
      Serial.println(blucommand);
      if (blucommand[0] == '1')
      {
        active = 1;
        gotodispenser = 1;
        while (otc == 0 && otl == 0 && otr == 0)
        {
          otl = digitalRead(otlpin);
          otc = digitalRead(otcpin);
          otr = digitalRead(otrpin);
          goForward(speed1, speed2, 0);
        }
      }
      blucommand = "";
    }
  }
  if (active)
  {
    if (otc == 0 && otl == 0 && otr == 0)
    {
      stopMotors(100);
      otl = digitalRead(otlpin);
      otc = digitalRead(otcpin);
      otr = digitalRead(otrpin);
      if (otc == 0 && otl == 0 && otr == 0)
      {
        if (gotodispenser)
        {
          IrSender.sendNEC(validaddress, command, repeats);
          delay(5000);
          while (otc == 0 && otl == 0 && otr == 0)
          {
            otl = digitalRead(otlpin);
            otc = digitalRead(otcpin);
            otr = digitalRead(otrpin);
            goForward(speed1, speed2, 0);
          }
          gotodispenser = 0;
        }
        else
        {
          active = 0;
        }
      }
    }
    else
    {
      if (otc == 1 && otl == 1 && otr == 1 && prevotc == 0)
      {
        goForward(speed1, speed2, 0);
      }
      else
      {
        if (otc == 0)
        {
          goForward(speed1, speed2, 0);
        }
        else
        {
          if (otl == 0)
          {
            goLeft(speedturn1, speedturn2, 0);
          }
          else
          {
            if (otr == 0)
            {
              goRight(speedturn1, speedturn2, 0);
            }
          }
        }
      }
    }
    prevotc = otc;
  }
}
