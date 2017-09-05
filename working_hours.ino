#include <EEPROM.h>

int hous_permission = 3; // Время до следующего обслуживания в часах
int num = 77; // Для последующего обновления (тоесть если отработало сколько то там часов и нужно обнулить - значение от фонаря, помоему до 255, но бери до 99 и не нуль)
int led = 13; // Вывод на индикатор работы
int relay = 2; // Вывод на реле
bool permission = false; // Разрешение на работу
int counter_time = 0; // Счетчик времени (сделал в часах - так как ЕЕПРОМ не сильно то и живучий, в плане переписывать каждую минуту значение)

void eeprom_clear()
{
    for (int i = 0; i < 512; i++) EEPROM.write(i, 0);
}

void EEPROMWriteInt(int address, int value)
{
    byte lowByte = ((value >> 0) & 0xFF);
    byte highByte = ((value >> 8) & 0xFF);
  
    EEPROM.write(address, lowByte);
    EEPROM.write(address + 1, highByte);
}

int EEPROMReadInt(int address)
{
    byte lowByte = EEPROM.read(address);
    byte highByte = EEPROM.read(address + 1);
    
    return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}
  
void setup() 
{
    pinMode(led, OUTPUT);
    digitalWrite(led, LOW);
    
    pinMode(relay, OUTPUT);
    digitalWrite(relay, LOW);
    
    if(EEPROM.read(255) != num)
    {
        eeprom_clear();
        EEPROM.write(255, num);
        EEPROMWriteInt(0, 0);
    }   

    // сравниваем с тем что установлено для разрешения в работе
    if( EEPROMReadInt(0) >= hous_permission - 1)
    {
        permission = false;
        
        digitalWrite(relay, LOW);
        digitalWrite(led, LOW);
    }
    else
    {
        permission = true;
        
        digitalWrite(relay, HIGH);
        digitalWrite(led, HIGH);
    }
}

void loop()
{
    // Если прошло 3600 секунд (тоесть час)
    if(counter_time >= 3600) 
    {
        // Переписываем значение в еепром
        int ht = EEPROMReadInt(0);
        
        // сравниваем с тем что установлено для разрешения в работе
        if(ht >= hous_permission - 1)
        {
            digitalWrite(relay, LOW);
            permission = false;
        }
        else
        {
            digitalWrite(relay, HIGH);
            ht++;
            EEPROMWriteInt(0, ht);
            permission = true;
        }
        
        // Обнуляем счетчик, заново пошел отсчет часа
        counter_time = 0;
    }

    if(permission)
    {
        digitalWrite(led, !digitalRead(led));
        counter_time++;
    }
    else
    {
        digitalWrite(led, LOW);
    }

    delay(1000);
}

