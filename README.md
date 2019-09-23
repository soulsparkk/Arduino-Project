# Эквалайзер для Arduino Uno
### Комплектация
- Arduino Uno
- Дисплей LCD5110
- Модуль микрофона
### Используемые библиотеки
+ math.h
+ FFT.h
### Быстрое преобразование Фурье 
БПФ - алгоритм ускоренного вычисления дискретного преобразования Фурье, 
позволяющий получить результат за время, меньшее чем O(N^2) (требуемого для прямого, поформульного вычисления).
+ Пример кода для быстрого преобразования Фурье
```
void fft(CArray& x)//на вход массив комплексных чисел
{
    const size_t N = x.size();//размер массива
    if (N <= 1) return;//выход из рекурсии
    
    // срезаем на чет/нечет,
    CArray even = x[std::slice(0, N/2, 2)];
    CArray  odd = x[std::slice(1, N/2, 2)];
    
    fft(even);
    fft(odd);
    
    for (size_t k = 0; k < N/2; ++k)
    {// величина и угол фазы
        Complex t = std::polar(1.0, -2 * PI * k / N) * odd[k];
        x[k    ] = even[k] + t;
        x[k+N/2] = even[k] - t;
    }
}
```
