// включаем функцию для логарифмического формата
#define LOG_OUT           1
// задаем количество выходных отсчетов
#define FFT_N             128
// подключаем библиотеку FFT — быстрое преобразование Фурье
#include <FFT.h>
// подключаем библиотеку для работы с дисплеем
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>


// распределяем номера отчётов по частотам
// весь диипазон (FFT_N / 2)
#define FREQ_LOW_FFT      2
#define FREQ_MIDDLE_FFT   30
#define FREQ_HIGH_FFT     60
 
#define FREQ_LOW_LEVEL    38
#define FREQ_MIDDLE_LEVEL 18
#define FREQ_HIGH_LEVEL   15

Adafruit_PCD8544 display = Adafruit_PCD8544(9, 10, 11, 12, 13);

// номер пина датчика микрофона
#define MIC_PIN           A2
long previousMillis = 0;

void setup()
{
  
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);
  Serial.begin(9600);         
  display.begin();
  display.setContrast(40);
 
}
 
void loop()
{ 
// считываем заданное количество отсчётов
  for (int i = 0 ; i < FFT_N; i++) {
    // считываем показания микрофона и вычитаем отрицательную полу-волну
    int sample = analogRead(MIC_PIN) - 511;
    // игнорируем помехи АЦП
    if (sample < 5 && sample > -5) {
      sample = 0; }
    // сохраняем действительные значения в четные отсчеты
    fft_input[i++] = sample;
    // задаем нечетным отсчетам значение «0»
    fft_input[i] = 0;
  }
  // функция-окно, повышающая частотное разрешение
  fft_window();
  // реорганизовываем данные перед запуском FFT
  fft_reorder();
  // обрабатываем данные в FFT
  fft_run();
  // извлекаем данные, обработанные FFT
  fft_mag_log();
  // выводим значения преобразования Фурье
  Serial.print("<");
 Serial.print(FFT_N / 2);
 Serial.print(":");

  for (int curBin = 0; curBin < FFT_N / 2; ++curBin) {
    if (curBin > 0) {
      Serial.print(",");
    }
   Serial.print(fft_log_out[curBin]);
  }
  // если значение низких частот превысило предел
  if (fft_log_out[FREQ_LOW_FFT] > FREQ_LOW_LEVEL) {
    for (int i=1;i<50;++i)
  {
 display.clearDisplay();
 display.fillRect(0, 0, 5, i, 1);
  display.display();
  delay(50); }
  
for (int i=1;i>=1;--i) {
 display.clearDisplay();
 display.fillRect(0, 47, 5, i, 1);
  display.display();
 delay(50);
  }
  }
  // если значение средних частот превысило предел
  if (fft_log_out[FREQ_MIDDLE_FFT] > FREQ_MIDDLE_LEVEL) {
     for (int i=1;i<50;++i) {
  display.clearDisplay();
 display.fillRect(6, 0, 5, i, 1);
  display.display();
  delay(50); }
for (int i=1;i>=1;--i) {
 display.clearDisplay();
 display.fillRect(6, 47, 5, i, 1);
  display.display();
 delay(50);
  }
  }
  // если значение высоких частот превысило предел
  if (fft_log_out[FREQ_HIGH_FFT] > FREQ_HIGH_LEVEL) {
    for (int i=1;i<50;++i) {
 display.clearDisplay();
 display.fillRect(12, 0, 5, i, 1);
  display.display();
  delay(50); }
for (int i=1;i>=1;--i) {
 display.clearDisplay();
 display.fillRect(12, 47, 5, i, 1);
  display.display();
 delay(50);
  }
  Serial.println(">");
}
  }
