#ifndef __LCD_FONT_H__
#define __LCD_FONT_H__

#include <stdint.h>

#define FONT_WIDTH        5U                    /* столбцов в глифе */
#define FONT_CELL_WIDTH   6U                    /* глиф + 1 столбец интервала */
#define FONT_HEIGHT       8U                    /* высота = 1 банк (8 точек) */
#define FONT_FIRST_CHAR   0x20U                 /* ' ' */
#define FONT_LAST_CHAR    0x7EU                 /* '~' */

#define LCD_WIDTH_PX      128U
#define LCD_ROWS          4U                    /* 4 банка по 8 точек */
#define LCD_COLS          (LCD_WIDTH_PX / FONT_CELL_WIDTH)   /* 21 символ */

/* Смещение Y-адреса банка. В вашем lcd_test() используется 0x41,
   т.е. Y = 1. Если картинка смещена - поменяйте на 0 или 1. */
#define LCD_Y_OFFSET      0U

/* Шрифт: [символ - 0x20][столбец], бит0 = верхняя точка */
extern const uint8_t font5x8[FONT_LAST_CHAR - FONT_FIRST_CHAR + 1U][FONT_WIDTH];

void lcd_set_cursor(uint8_t col, uint8_t row);          /* col: 0..20, row: 0..3 */
void lcd_putc(char c);                                  /* в текущую позицию */
void lcd_puts(uint8_t col, uint8_t row, const char *s); /* строка одним пакетом I2C */
void lcd_clear(void);

#endif
