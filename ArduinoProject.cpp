#include <FFT.h>
#include <math.h>

const int micPin = A0;     // номер порта к которому подключен потенциометр
int val = 0;           // переменная для хранения считываемого значения
 
void setup()
{
  Serial.begin(9600);              //  установка связи по serial
}
 
void loop()
{
  val = analogRead(analogPin);     // считываем значение
  Serial.println(val);             // выводим полученное значение
}
