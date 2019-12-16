// включаем функцию для логарифмического формата
#define LOG_OUT           1
// задаем количество выходных отсчетов
#define FFT_N             256
// подключаем библиотеку FFT — быстрое преобразование Фурье
#include <FFT.h>
// подключаем библиотеку для работы с дисплеем
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
// номер пина датчика микрофона
#define MIC_PIN           A2
#define FREQ_LOW_LEVEL 30

byte freq_offset[20] = {4, 6, 8, 10, 12, 14, 16, 18, 20, 22};

Adafruit_PCD8544 display = Adafruit_PCD8544(9, 10, 11, 12, 13);

void print(int j,int val){
      //display.clearDisplay();
      display.fillRect(8*j, 47, 5, -47, 0);
      display.display();
      display.fillRect(8*j, 47, 5, -val, 1);
      display.display();
      //delay(10); 
}
void setup() {
  // put your setup code here, to run once:
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);
  Serial.begin(9600);         
  display.begin();
  display.setContrast(500);
  display.clearDisplay();
}

void loop() {
  // put your main code here, to run repeatedly:
  // считываем заданное количество отсчётов
  for (int i = 0 ; i < FFT_N; i++) {
    // считываем показания
    int sample = analogRead(MIC_PIN);
    // сохраняем действительные значения в четные отсчеты
    fft_input[i] = sample;
  }
  // функция-окно, повышающая частотное разрешение
  fft_window();
  // реорганизовываем данные перед запуском FFT
  fft_reorder();
  // обрабатываем данные в FFT
  fft_run();
  // извлекаем данные, обработанные FFT
  fft_mag_log();
  for (int i = 0; i < 10; i++) {
    int val = map(fft_log_out[freq_offset[i]], FREQ_LOW_LEVEL, 80, 47, 0);//переворачиваем диапазон
    val = constrain(val, 0, 47);
    print(i,val);
  }
}