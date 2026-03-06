
#include "TBitField.h"

// Возвращает индекс элемента в массиве для бита n
// Использует сдвиг вправо на 5 (деление на 32) для скорости
int TBitField::GetMemIndex(const int n) const {
    return n >> 5;  // То же самое что n / 32
}

// Возвращает битовую маску для бита n
// 1 << (n & 31) - сдвигаем 1 влево на остаток от деления n на 32
TELEM TBitField::GetMemMask(const int n) const {
    return 1 << (n & 31);  // n & 31 - то же что n % 32, но быстрее
}

// Конструктор: создает битовое поле длины len
TBitField::TBitField(int len) : BitLen(len) {
    // Вычисляем, сколько элементов TELEM нужно для хранения len бит
    // (len + 31) >> 5 - это округление вверх при делении на 32
    MemLen = (len + 31) >> 5;
    
    // Выделяем динамическую память под массив
    pMem = new TELEM[MemLen];
    
    // Инициализируем все биты нулями
    if (pMem != nullptr) {  // Проверка, что память выделилась
        for (int i = 0; i < MemLen; i++) {
            pMem[i] = 0;  // 0 означает, что все 32 бита равны 0
        }
    }
}

// Конструктор копирования: создает копию битового поля bf
TBitField::TBitField(const TBitField& bf) {
    // Копируем параметры из исходного объекта
    BitLen = bf.BitLen;
    MemLen = bf.MemLen;
    
    // Выделяем новую память такого же размера
    pMem = new TELEM[MemLen];
    
    // Копируем данные из исходного объекта в новый
    if (pMem != nullptr) {
        for (int i = 0; i < MemLen; i++) {
            pMem[i] = bf.pMem[i];  // Копируем каждый элемент массива
        }
    }
}

// Деструктор: освобождает выделенную память
TBitField::~TBitField() {
    if (pMem != nullptr) {   // Проверяем, что память была выделена
        delete[] pMem;        // Освобождаем динамический массив
    }
}

// Устанавливает бит n в 1
void TBitField::SetBit(const int n) {
    // Проверяем, что n находится в допустимых пределах
    if ((n >= 0) && (n < BitLen)) {
        // OR с маской устанавливает нужный бит в 1
        // Другие биты в этом элементе не изменяются
        pMem[GetMemIndex(n)] |= GetMemMask(n);
    }
    // Если n вне диапазона - просто ничего не делаем
}

// Устанавливает бит n в 0 (очищает)
void TBitField::ClrBit(const int n) {
    // Проверяем, что n находится в допустимых пределах
    if ((n >= 0) && (n < BitLen)) {
        // AND с инвертированной маской очищает нужный бит
        // Например: если маска = 00000100, то ~маска = 11111011
        pMem[GetMemIndex(n)] &= ~GetMemMask(n);
    }
}

// Возвращает значение бита n (1 или 0)
int TBitField::GetBit(const int n) const {
    // Проверяем, что n находится в допустимых пределах
    if ((n >= 0) && (n < BitLen)) {
        // AND с маской даст 0, если бит не установлен, и не 0, если установлен
        // Затем сравниваем с 0, получая 1 (true) или 0 (false)
        return (pMem[GetMemIndex(n)] & GetMemMask(n)) != 0;
    }
    return 0;  // Для битов вне диапазона возвращаем 0
}

// Оператор присваивания: копирует битовое поле bf в текущий объект
TBitField& TBitField::operator=(const TBitField& bf) {
    // Копируем длину в битах
    BitLen = bf.BitLen;
    
    // Если размер памяти разный, нужно перевыделить память
    if (MemLen != bf.MemLen) {
        MemLen = bf.MemLen;  // Запоминаем новый размер
        
        // Освобождаем старую память
        if (pMem != nullptr) {
            delete[] pMem;
        }
        
        // Выделяем новую память
        pMem = new TELEM[MemLen];
    }
    
    // Копируем данные
    if (pMem != nullptr) {
        for (int i = 0; i < MemLen; i++) {
            pMem[i] = bf.pMem[i];  // Копируем каждый элемент
        }
    }
    
    return *this;  // Возвращаем ссылку на текущий объект (для цепочки присваиваний)
}

// Оператор сравнения: проверяет равенство двух битовых полей
int TBitField::operator==(const TBitField& bf) {
    int res = 1;  // Предполагаем, что равны (1 = истина)
    
    // Если разная длина - сразу не равны
    if (BitLen != bf.BitLen) { 
        res = 0; 
    }
    else {
        // Сравниваем каждый элемент массива
        for (int i = 0; i < MemLen; i++) {
            if (pMem[i] != bf.pMem[i]) {
                res = 0;  // Нашли отличие
                break;    // Дальше можно не проверять
            }
        }
    }
    return res;  // Возвращаем результат (1 - равны, 0 - не равны)
}

// Оператор И (побитовое И) для двух битовых полей
TBitField TBitField::operator&(const TBitField bf) {
    // Берем минимальную длину (результат не может быть длиннее меньшего поля)
    int min = BitLen;
    int mm = MemLen;
    if (bf.BitLen < min) {
        min = bf.BitLen;
        mm = bf.MemLen;
    }
    
    // Создаем временный объект минимальной длины
    TBitField tmp(min);
    
    // Применяем побитовое И к элементам массивов
    for (int i = 0; i < mm; i++) {
        tmp.pMem[i] = pMem[i] & bf.pMem[i];
    }
    
    return tmp;  // Возвращаем результат
}

// Оператор ИЛИ (побитовое ИЛИ) для двух битовых полей
TBitField TBitField::operator|(const TBitField bf) {
    // Берем максимальную длину (результат должен вместить все биты из обоих полей)
    int max = BitLen;
    if (bf.BitLen > max) {
        max = bf.BitLen;
    }
    
    // Создаем временный объект максимальной длины
    TBitField tmp(max);
    
    // Копируем данные из текущего объекта
    for (int i = 0; i < MemLen; i++) {
        tmp.pMem[i] = pMem[i];
    }
    
    // Применяем побитовое ИЛИ с элементами второго объекта
    for (int i = 0; i < bf.MemLen; i++) {
        tmp.pMem[i] |= bf.pMem[i];
    }
    
    return tmp;  // Возвращаем результат
}

// Оператор НЕ (побитовое отрицание) для битового поля
TBitField TBitField::operator~() {
    // Создаем объект такой же длины
    TBitField res(BitLen);
    
    // Применяем побитовое НЕ ко всем элементам
    for (int i = 0; i < MemLen; i++) {
        res.pMem[i] = ~pMem[i];  // Инвертируем все 32 бита в элементе
    }
    
    // Если длина не кратна 32, нужно обнулить лишние биты в последнем элементе
    if (BitLen % 32 != 0) {
        int bit = BitLen % 32;  // Сколько битов реально используется в последнем элементе
        TELEM mask = (1 << bit) - 1;  // Маска: например для bit=4 будет 1111 (15)
        res.pMem[MemLen - 1] &= mask;  // Обнуляем биты с позиции bit до 31
    }
    
    return res;  // Возвращаем результат
}

// Возвращает длину битового поля в битах
int TBitField::GetLen() const {
    return BitLen;
}