/*
 * Copyright (c) 2019 Actinius
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <gpio.h>

#define RED_LED LED0_GPIO_PIN
#define GREEN_LED LED1_GPIO_PIN
#define BLUE_LED LED2_GPIO_PIN
#define BUTTON SW0_GPIO_PIN

#define LED_ON 0
#define LED_OFF !LED_ON

typedef enum {
    RED,
    GREEN,
    BLUE,
    MAGENTA,
    CYAN,
    YELLOW
} led_color_t;

static struct device* gpio_dev;
static struct gpio_callback gpio_cb;

static bool button_pressed = false;

void button_pressed_callback(struct device *gpiob, struct gpio_callback *cb, u32_t pins)
{
    button_pressed = true;
}

void init_button(void) 
{ 
    gpio_pin_configure(gpio_dev, BUTTON, GPIO_DIR_IN | GPIO_PUD_PULL_UP | GPIO_INT | GPIO_INT_DEBOUNCE | GPIO_INT_EDGE); 
	gpio_init_callback(&gpio_cb, button_pressed_callback, BIT(BUTTON));

	gpio_add_callback(gpio_dev, &gpio_cb);
	gpio_pin_enable_callback(gpio_dev, BUTTON);
}

void init_leds(void)
{
    gpio_pin_configure(gpio_dev, RED_LED, GPIO_DIR_OUT);
    gpio_pin_configure(gpio_dev, BLUE_LED, GPIO_DIR_OUT);
    gpio_pin_configure(gpio_dev, GREEN_LED, GPIO_DIR_OUT);
}

void turn_leds_off(void)
{
    gpio_pin_write(gpio_dev, RED_LED, LED_OFF);
    gpio_pin_write(gpio_dev, BLUE_LED, LED_OFF);
    gpio_pin_write(gpio_dev, GREEN_LED, LED_OFF);
}

void turn_leds_on_with_color(led_color_t color) 
{   
    switch (color) {
    case RED:
        gpio_pin_write(gpio_dev, RED_LED, LED_ON);
        break;
    case GREEN:
        gpio_pin_write(gpio_dev, GREEN_LED, LED_ON);
        break;
    case BLUE:
        gpio_pin_write(gpio_dev, BLUE_LED, LED_ON);
        break;
    case MAGENTA:
        gpio_pin_write(gpio_dev, BLUE_LED, LED_ON);
        gpio_pin_write(gpio_dev, RED_LED, LED_ON);
        break;
    case CYAN:
        gpio_pin_write(gpio_dev, GREEN_LED, LED_ON);
        gpio_pin_write(gpio_dev, BLUE_LED, LED_ON);
        break;
    case YELLOW:
        gpio_pin_write(gpio_dev, GREEN_LED, LED_ON);
        gpio_pin_write(gpio_dev, RED_LED, LED_ON);
        break;
    }
}

int main(void)
{
    gpio_dev = device_get_binding(DT_GPIO_P0_DEV_NAME);

    if (!gpio_dev) {
		printk("Error getting " DT_GPIO_P0_DEV_NAME " device binding\r\n");
        
        return false;
	}
    
    init_button();
    init_leds();
    turn_leds_off();
    
    led_color_t color = RED;

	while (1) {
        if (button_pressed) {
            turn_leds_off();
            turn_leds_on_with_color(color);
        
            color++;
            if (color > 5) {
                color = 0;
            }

            button_pressed = false;        
        }

        k_sleep(100);
	}

	return 0;
}
