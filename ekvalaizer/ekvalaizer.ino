// включаем функцию для линейного формата

#define LIN_OUT           1
// задаем количество выходных отсчетов
#define FFT_N             256
// подключаем библиотеку FFT — быстрое преобразование Фурье
#include <FFT.h>
// подключаем библиотеку для работы с дисплеем
#include <Adafruit_PCD8544.h>
Adafruit_PCD8544 display = Adafruit_PCD8544(9, 10, 11, 12, 13);
//библиотека для сохранения настроек
#include <EEPROM.h>
// номер пина датчика микрофона
#define MIC_PIN           A2
//Нижнее и верхнее значение частоты
#define FREQ_HIGH_LEVEL 100
//пины джойстика
#define ANALOG_X_pin 0
#define ANALOG_Y_pin 1
#define RIGHT_pin 3
#define LEFT_pin 5
#define UP_pin 2
#define DOWN_pin 4
#define BUTTON_E 6
#define BUTTON_G 8

class Equalizer{
  private://переменные
    //Настройки: число столбцов, скорость
    int columns = 8;
    int curDelay = 0;
    //Выбранные в данный момент параметры в меню
    uint8_t selectedOption = 0;
    uint8_t menuArea = 0;
  public:
    uint8_t contrast = 20;//Настройка контраста
    void ShowMenu(){//главное меню
      while(true){
        //перемещение по главному меню
        if(menuArea==2){//перемещение по меню настроек
          if(digitalRead(UP_pin)==LOW){
            if(selectedOption == 0){
              selectedOption = 2;
            }else selectedOption -= 1;
          }
          if(digitalRead(DOWN_pin)==LOW){
            if(selectedOption == 2){
              selectedOption = 0;
            }else selectedOption += 1;
          }
        }else{//перемещение по главному меню
          if(digitalRead(UP_pin)==LOW||digitalRead(DOWN_pin)==LOW){
            if(selectedOption==0){
              selectedOption=1;
            }else selectedOption=0;
          }
        }
        display.clearDisplay();
        if(menuArea==0){//main menu
          // text for menu
          display.setTextSize(1);
          display.setTextColor(BLACK);
          display.setCursor(25,10);
          display.println("START!");
          display.setCursor(18,30);
          display.println("SETTINGS");
          display.drawLine(18, 17+(selectedOption*20), display.width()-18,  17+(selectedOption*20), BLACK);//x_start,y_start,x_end,y_end,color
          display.display();
          if(digitalRead(BUTTON_G)==LOW){//после нажатия на джойстик
            display.clearDisplay();
            if(selectedOption==0){//перейти к эквалайзеру
              menuArea=1;
              ShowSpectrum();
            }else{//перейти к настройкам
              menuArea=2;
              selectedOption=0;
              ShowSettings();
            }
          }
        }else if(menuArea==1){//если находимся в эквалайзере
          display.clearDisplay();
          ShowSpectrum();
        }else if(menuArea==2){//если находимся в настройках
          display.clearDisplay();
          ShowSettings();
        }
      }
    }
    //показать спектр частот
    void ShowSpectrum(){
      while(digitalRead(BUTTON_E)!=LOW){//Е выводит из эквалайзера
        // считываем заданное количество отсчётов
        for (int i = 0 ; i < FFT_N; i++) {
          // считываем показания
          int sample = analogRead(MIC_PIN)-511;
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
        fft_mag_lin();
        for (int i = 0; i < columns; i++) {
          //выкидываем самые низкие частоты
          short low_freq_range = FFT_N/(2*columns);
          //пересчитываем диапазоны
          short range = (FFT_N-low_freq_range)/(2*columns);
          //ищем максимальную амплитуду в диапазоне
          short x = fft_lin_out[low_freq_range + i*range];
          for (int j=i*range;j<(i+1)*range;++j)
            x = max(x,fft_lin_out[low_freq_range + j]);
          //мапим на максимальную высоту отображаемого столбца
          x = map(x, 0, FREQ_HIGH_LEVEL, 0, 47);
          //вывод значений, i - от низких к высоким, val - амплитуда
          PrintSpectrum(i,x);
        }
      }
      //возврат к меню
      menuArea=0;
      selectedOption=0;
      ShowMenu();
    }
    void PrintSpectrum(int i, int val){
      //расчет пр-ва между столбцами
      int modulo=(84-columns*4)%(columns+1);//остаток от деления 
      int space =(84-columns*4-modulo)/(columns+1);//пр-во между столбцами
      display.fillRect(space+(space+4)*i, 47, 4, -47, 0);//удаление столбца
      display.display();
      display.fillRect(space+(space+4)*i, 47, 4, -val, 1);//заполнение столбца
      display.display();
      delay(curDelay);
    }
    void ShowSettings(){//настройки
      display.setTextSize(1);
      display.setTextColor(BLACK);
      display.setCursor(0,5);
      display.print("Contrast: ");
      display.println(contrast);
      display.print("Delay: ");
      display.println(curDelay);
      display.print("Columns: ");
      display.println(columns);
      //полоса под выбранным
      display.drawLine(0, 12+(selectedOption*8), display.width()-38,  12+(selectedOption*8), BLACK);//x_start,y_start,x_end,y_end,color 
      display.display();
      if(digitalRead(RIGHT_pin)==LOW||digitalRead(LEFT_pin)==LOW){//при изменении чего-то
        if(selectedOption==0){
          if(digitalRead(RIGHT_pin)==LOW){//увеличить контраст
            if(contrast<=124){
              contrast+=2;
            }
            display.setContrast(contrast);
          }else if(digitalRead(LEFT_pin)==LOW){//уменьшить контраст
            if(contrast>=0){
              contrast-=2;
            }
            display.setContrast(contrast);
          }
          EEPROM.put(0, contrast);//сохранить в памяти устройства
        }else if(selectedOption==1){
          if(digitalRead(RIGHT_pin)==LOW){//увеличить задержку
            if(curDelay<=50){
              curDelay+=5;
            }
          }else if(digitalRead(LEFT_pin)==LOW){//уменьшить задержку
            if(curDelay>=5){
              curDelay-=5;
            }
          }
          EEPROM.put(9, curDelay);
        }else if(selectedOption==2){
          if(digitalRead(RIGHT_pin)==LOW){//увеличить число столбцов
            if(columns<=15){
              columns+=1;
            }
          }else if(digitalRead(LEFT_pin)==LOW){//уменьшить число столбцов
            if(columns>=5){
              columns-=1;
            }
          }
          EEPROM.put(18, columns);
        
        }      
      }
      if(digitalRead(BUTTON_E)==LOW){//назад в меню
        menuArea=0;
        selectedOption=0;
        ShowMenu();
      }
    }
    void SetValues(){//вывод настроек из памяти
      contrast=70;
      curDelay=0;
      columns=8;
      if (EEPROM.read(0) >= 0 && EEPROM.read(0) <= 124)
        EEPROM.get(0,contrast);
      if (EEPROM.read(9) >= 0 && EEPROM.read(9) <= 50)
        EEPROM.get(9,curDelay);
      if (EEPROM.read(18) >= 3 && EEPROM.read(18) <= 16)
        EEPROM.get(18,columns);
    }
};


//Обьявление класса
Equalizer equalizer;

void setup() {
  //joystick
  pinMode(UP_pin, INPUT_PULLUP);
  pinMode(DOWN_pin, INPUT_PULLUP);
  pinMode(RIGHT_pin, INPUT_PULLUP);
  pinMode(LEFT_pin, INPUT_PULLUP);
  pinMode(BUTTON_E, INPUT_PULLUP);
  pinMode(BUTTON_G, INPUT_PULLUP);
//NOKIA 5110
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);     
  display.begin();
  display.clearDisplay();
  display.setContrast(60);
  equalizer.SetValues();
  equalizer.ShowMenu();
}

void loop() {
  equalizer.ShowMenu();
}
