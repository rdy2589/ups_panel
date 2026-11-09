#include "gd32f30x.h"
#include "gd32f30x_rcu.h"
#include "gd32f30x_gpio.h"
#include "gd32f30x_i2c.h"
#include "gd32f30x_adc.h"

#include <stdio.h>
#include "gd32f30x_eval.h"
#include <stdint.h>


#include "systick.h"
#include "global.h"
#include "ui.h"
#include "lcd.h"
#include "lcd_font.h"



static const char* menu_items[] = {
    "1. Max Volt Thr",
    "2. Min Volt Thr",
    "3. Min Bat Volt",
    "4. Charger Cur ",
    "5. Backlight Tm",
    "6. Sound",
    "7. Exit Menu   "
};

#define MENU_ITEMS_COUNT (sizeof(menu_items) / sizeof(menu_items[0]))


// Настройки (хранимые параметры)
struct item_t settings_values[MENU_ITEMS_COUNT] = {
     { 215, 200, 220, ITEM_IS_NUMERIC, NULL, NULL},
     { 225, 220, 240, ITEM_IS_NUMERIC, NULL, NULL},
     { 21, 18, 27, ITEM_IS_NUMERIC, NULL, NULL },
     { 3, 1, 5, ITEM_IS_NUMERIC, NULL, NULL }, // "4. Charger Cur ",
     { 1, 0, 1, ITEM_IS_TEXT, "Off", "On" },
     { 1, 0, 1, ITEM_IS_TEXT, "Off", "On" },
     { 0, 0, 1, ITEM_IS_NUMERIC, NULL, NULL }
};


static ui_state_t ui_current_state = UI_STATE_MAIN;
static int8_t menu_cursor = 0;

void ui_init(void)
{
    // Инициализация пинов кнопок в режим Input Pull-Up
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOE);
    gpio_init(BT_MENU_gpio, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, BT_MENU_pin);
    gpio_init(BT_OFF_gpio, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, BT_OFF_pin);
}


/* Отрисовка Главного экрана */
void ui_render_main(uint16_t u_out, uint16_t p_out, uint16_t u_line, uint16_t u_bat)
{
    char buf[24];
    
    if(inverterState.mode == INV_MODE_BATTERY)
    {
        // Строка 0-1 (Большой шрифт): Напряжение и Мощность инвертора
        sprintf(buf, "%3uV  %4uW", u_out, p_out);
        lcd_puts16(0, 0, buf);
        
        // Строка 2 (Маленький шрифт): Напряжение батареи (с десятыми долями)
        sprintf(buf, "Ubat: %u.%1uV", u_bat / 10, u_bat % 10);
        lcd_puts(0, 2, buf);
        
        // Строка 3 (Маленький шрифт): Статус / Подсказка
        lcd_puts(0, 3, "MODE: INVERTER  [M]");

    }
    else
    {
        // Строка 0-1 (Большой шрифт): Напряжение и Мощность инвертора
        sprintf(buf, "%3uV     ", u_line);
        lcd_puts16(0, 0, buf);
        
        // Строка 2 (Маленький шрифт): Напряжение батареи (с десятыми долями)
        sprintf(buf, "Ubat: %u.%1uV", u_bat / 10, u_bat % 10);
        lcd_puts(0, 2, buf);
        
        // Строка 3 (Маленький шрифт): Статус / Подсказка
        lcd_puts(0, 3, "MODE: LINE  [M]");
    }

}

/* Отрисовка Экрана Навигации по Меню */
void ui_render_menu(uint8_t current_item)
{
    char buf[24];
    lcd_clear();
    
    // Заголовок меню
    lcd_puts(0, 0, "--- SETTINGS ---");
    
    // Отображаем текущий пункт и его значение
    sprintf(buf, ">%s", menu_items[current_item]);
    lcd_puts(0, 1, buf);
    
    if (current_item < MENU_ITEMS_COUNT - 1) {
        if(settings_values[current_item].type == ITEM_IS_TEXT)
        {    if(settings_values[current_item].value != 0) sprintf(buf, "   Val: %s", settings_values[current_item].text_if_1);
            else sprintf(buf, "   Val: %s", settings_values[current_item].text_if_0);
        }
        else    
            sprintf(buf, "   Val: %d", settings_values[current_item].value);
        lcd_puts(0, 2, buf);
    } else {
        lcd_puts(0, 2, " Click to return");
    }
    
    lcd_puts(0, 3, "[Enc]-Scroll [OK]-Sel");
}

/* Отрисовка Экрана Изменения Параметра */
void ui_render_edit(const char* param_name, int16_t value, uint8_t current_item)
{
    char buf[24];
    lcd_clear();
    
    lcd_puts(0, 0, "EDIT PARAMETER:");
    lcd_puts(0, 1, param_name);
    
    // Выводим изменяемое значение крупно
    if(settings_values[current_item].type == ITEM_IS_TEXT)
    {
        if(value != 0) sprintf(buf, "-> %s <-", settings_values[current_item].text_if_1);
        else sprintf(buf, "-> %s <-", settings_values[current_item].text_if_0);
    }
    else sprintf(buf, "-> %d <-", value);

    lcd_puts16(2, 2, buf); 
}

/* Главный обработчик интерфейса (вызывается циклически) */
void ui_update(int16_t enc_diff, uint8_t btn1, uint8_t btn2, uint8_t btn_sw)
{
    static uint32_t last_render_tick = 0;
    
    // Симуляция данных инвертора для главного экрана
    static uint16_t sim_u = 220
                , sim_p = 450
                , sim_vbat = 254
                , sim_l = 221;      //line voltage 

    switch (ui_current_state) 
    {
        case UI_STATE_MAIN:
            // Если нажата кнопка 2 или Кнопка Энкодера -> Переход в меню
            if (btn2 || btn_sw) {
                ui_current_state = UI_STATE_MENU;
                menu_cursor = 0;
                ui_render_menu(menu_cursor);
            } else {
                // Обновление главного экрана раз в 200мс
                ui_render_main(sim_u, sim_p, sim_l, sim_vbat);
            }
            break;
            
        case UI_STATE_MENU:
            if (btn1) { // Нажата кнопка Назад -> Возврат на главный экран
                ui_current_state = UI_STATE_MAIN;
                lcd_clear();
            }
            else if (enc_diff != 0) { // Прокрутка меню энкодером
                menu_cursor += enc_diff;
                if (menu_cursor < 0) menu_cursor = MENU_ITEMS_COUNT - 1;
                if (menu_cursor >= MENU_ITEMS_COUNT) menu_cursor = 0;
                ui_render_menu(menu_cursor);
            }
            else if (btn2 || btn_sw) { // Выбор пункта
                if (menu_cursor == MENU_ITEMS_COUNT - 1) { // Пункт "Exit"
                    ui_current_state = UI_STATE_MAIN;
                    lcd_clear();
                } else {
                    ui_current_state = UI_STATE_EDIT;
                    ui_render_edit(menu_items[menu_cursor], settings_values[menu_cursor].value, menu_cursor);
                }
            }
            break;
            
        case UI_STATE_EDIT:
            if (btn1) { // Отмена изменений
                ui_current_state = UI_STATE_MENU;
                ui_render_menu(menu_cursor);
            }
            else if (enc_diff != 0) { // Изменение значения энкодером
                settings_values[menu_cursor].value += enc_diff;
                norm_value(&settings_values[menu_cursor].value, settings_values[menu_cursor].min_value, settings_values[menu_cursor].max_value);
                ui_render_edit(menu_items[menu_cursor], settings_values[menu_cursor].value, menu_cursor);
            }
            else if (btn2 || btn_sw) { // Сохранение значения
                ui_current_state = UI_STATE_MENU;
                ui_render_menu(menu_cursor);
            }
            break;
    }
}

void norm_value(int16_t *value, int16_t min, int16_t max)
{
    if(*value < min) *value = min;
    if(*value > max) *value = max;
}