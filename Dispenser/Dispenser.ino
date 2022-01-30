#include <IRremote.h>
#include <Servo.h>
Servo srv;

#define receivepin 3
#define validaddress 1234
#define servopin 2

void setup()
{
  Serial.begin(9600);
  IrReceiver.begin(receivepin, ENABLE_LED_FEEDBACK);
}

void loop()
{
  if (IrReceiver.decode())
  {
    if (IrReceiver.decodedIRData.protocol != UNKNOWN && IrReceiver.decodedIRData.address == validaddress)
    {
      Serial.println(IrReceiver.decodedIRData.command);
      if (IrReceiver.decodedIRData.command == 1)
      {
        srv.attach(servopin);
        srv.write(0);
        delay(1000);
        srv.write(180);
        delay(1000);
        srv.detach();
      }
    }
    IrReceiver.resume();
  }
}
