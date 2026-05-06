#include "gd32f30x.h"
#include "sin_table.h"
#include "gd32f30x_gpio.h"
#include "gd32f30x_timer.h"
#include <stdio.h>
#include "gd32f30x_eval.h"
#include <stdint.h>
#include <math.h>



float sin_table[SIN_TABLE_SIZE];
uint16_t voltageRMSBuffer[SIN_TABLE_SIZE];

void generate_sin_table(void)
{
    for(int i = 0; i < SIN_TABLE_SIZE; i++)
    {
        // Амплитуда 1.0
        sin_table[i] = /* 0.95f **/ sinf(1.0f * PI * i / SIN_TABLE_SIZE);
        
        // Для ШИМ с насыщением (чтобы избежать перемодуляции)
        // sin_table[i] = 0.95f * sinf(2.0f * PI * i / SIN_TABLE_SIZE);
    }
}