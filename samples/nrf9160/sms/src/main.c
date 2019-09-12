/*
 * Copyright (c) 2019 Actinius
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <gpio.h>
#include <at_cmd.h>
#include <stdio.h>
#include <string.h>
#include <lte_lc.h>
#include "pdu.h"

#define BUTTON SW0_GPIO_PIN

// #define SMS_DESTINATION_NUMBER "31648999999"

#if !defined(SMS_DESTINATION_NUMBER)
#error "Please uncomment and fill in the mentioned variable"
#endif

static struct device *gpio_dev;
static struct gpio_callback gpio_cb;
static char modem_buffer[384]; // this is used to receive the encoded SMS from the modem and to send the AT command to the modem
static uint8_t modem_buffer_pdu_bytes[192];
static char decoded_sms_message[192];
static uint8_t encoded_pdu[192];
static bool button_pressed = false;

void button_pressed_callback(struct device *gpiob, struct gpio_callback *cb, u32_t pins)
{
	button_pressed = true;
}

void init_button(void)
{
	gpio_dev = device_get_binding(DT_GPIO_P0_DEV_NAME);

	if (!gpio_dev) {
		printk("Error getting " DT_GPIO_P0_DEV_NAME " device binding\r\n");

		return;
	}

	gpio_pin_configure(gpio_dev, BUTTON, GPIO_DIR_IN | GPIO_PUD_PULL_UP | GPIO_INT | GPIO_INT_DEBOUNCE | GPIO_INT_EDGE);
	gpio_init_callback(&gpio_cb, button_pressed_callback, BIT(BUTTON));

	gpio_add_callback(gpio_dev, &gpio_cb);
	gpio_pin_enable_callback(gpio_dev, BUTTON);
}

static uint8_t convert_hex_char_to_byte_value(char c)
{
	if (c >= 'A' && c <= 'F') {
		return ((uint8_t)c - 55);
	} else {
		return ((uint8_t)c - 48);
	}

	return 0;
}

static uint8_t convert_hex_string_to_byte_array(char *buffer, uint8_t *byte_array)
{
	uint8_t index = 0;

	for (int i = 0; i < strlen(buffer); i += 2) {
		byte_array[index] = 16 * convert_hex_char_to_byte_value(buffer[i]) + convert_hex_char_to_byte_value(buffer[i + 1]);
		index++;
	}

	return index;
}

static void sms_received_handler(char *response)
{
	memset(modem_buffer, 0, sizeof(modem_buffer));

	if (sscanf(response, "+CMT: \"%*[^\"]\",%*d\r%s", modem_buffer) != 1) {
		printk("Could not parse modem response\n");

		return;
	}

	memset(modem_buffer_pdu_bytes, 0, sizeof(modem_buffer_pdu_bytes));
	uint8_t modem_buffer_pdu_bytes_nr = convert_hex_string_to_byte_array(modem_buffer, modem_buffer_pdu_bytes);

	time_t decoded_sms_time;
	char decoded_sms_number[20] = { '\0', };
	memset(decoded_sms_message, 0, sizeof(decoded_sms_message));

	if (pdu_decode(modem_buffer_pdu_bytes, modem_buffer_pdu_bytes_nr, &decoded_sms_time, decoded_sms_number,
		       sizeof(decoded_sms_number), decoded_sms_message, sizeof(decoded_sms_message)) < 0) {
		printk("Could not decode sms message\n");

		return;
	}

	printk("SMS message \"%s\" received from \"%s\"\n", decoded_sms_message, decoded_sms_number);
}

static void at_callback(char *response)
{
	if (strlen(response) == 0) {
		return;
	}

	printk("Modem response: %s\n", response);
}

bool sms_client_init(void)
{
	enum at_cmd_state state;

	at_cmd_write_with_callback("AT+CNMI=3,2,0,1", at_callback, &state);

	return true;
}

void sms_client_send(const char *sms_text, const char *destination_number)
{
	memset(encoded_pdu, 0, sizeof(encoded_pdu));
	int encode_size = pdu_encode(CONFIG_SMSC_NUMBER, destination_number, sms_text, encoded_pdu, sizeof(encoded_pdu));

	uint8_t smsc_length = encoded_pdu[0] + 1;
	uint8_t tpdu_length = encode_size - smsc_length;

	memset(modem_buffer, 0, sizeof(modem_buffer));
	snprintk(modem_buffer, sizeof(modem_buffer), "AT+CMGS=%d\r", tpdu_length);

	for (int i = 0; i < encode_size; i++) {
		char byte[3] = { '\0', };
		snprintk(byte, sizeof(byte), "%02x", encoded_pdu[i]);
		strcat(modem_buffer, byte);
	}

	strcat(modem_buffer, "\x1a");

	enum at_cmd_state state;
	at_cmd_write_with_callback(modem_buffer, at_callback, &state);
}

void sms_client_register_receive(void)
{
	at_cmd_set_notification_handler(sms_received_handler);
}

void sms_client_unregister_receive(void)
{
	at_cmd_set_notification_handler(NULL);
}

void connect_to_network(void)
{
#if defined(CONFIG_LTE_LINK_CONTROL)
	if (IS_ENABLED(CONFIG_LTE_AUTO_INIT_AND_CONNECT)) {
		/* Do nothing, modem is already turned on
		 * and connected.
		 */
	} else {
		int err;

		printk("LTE Link Connecting ...\n");
		err = lte_lc_init_and_connect();
		__ASSERT(err == 0, "LTE link could not be established.");
		printk("LTE Link Connected!\n");
	}
#endif
}

int main(void)
{
	printk("Starting SMS sample\n");

	init_button();
	connect_to_network();
	sms_client_init();
	sms_client_register_receive();

	while (1) {
		if (button_pressed) {
			printk("Sending SMS message\n");
			sms_client_send("Button was pressed", SMS_DESTINATION_NUMBER);

			button_pressed = false;
		}

		k_sleep(200);
	}

	return 0;
}
