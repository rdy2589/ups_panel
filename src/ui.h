#ifndef __UI_H__
#define __UI_H__


#define BT_MENU_gpio    GPIOB
#define BT_MENU_pin     GPIO_PIN_13

#define BT_OFF_gpio     GPIOE
#define BT_OFF_pin      GPIO_PIN_15

typedef enum {
    UI_STATE_MAIN = 0,
    UI_STATE_MENU,
    UI_STATE_EDIT
} ui_state_t;

enum item_type {
    ITEM_IS_NUMERIC = 0,
    ITEM_IS_TEXT
};

struct item_t {
    int16_t value;
    int16_t min_value;
    int16_t max_value;
    enum item_type type;
    char* text_if_0;
    char* text_if_1;
};





void ui_init(void);
void ui_update(int16_t enc_diff, uint8_t btn1, uint8_t btn2, uint8_t btn_sw);
void ui_render_main(uint16_t u_out, uint16_t p_out, uint16_t u_line, uint16_t u_bat);
void ui_render_menu(uint8_t current_item);
void ui_render_edit(const char* param_name, int16_t value, uint8_t current_item);
void norm_value(int16_t *value, int16_t min, int16_t max);


#endif
