/* LED blinky for XIAO nRF52840: cycles the on-board 3-in-1 RGB LED
 * (led0=red, led1=green, led2=blue) so a dead LED channel is obvious. */
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>

#define SLEEP_MS 400

static const struct gpio_dt_spec leds[] = {
	GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios),
};

int main(void)
{
	for (size_t i = 0; i < ARRAY_SIZE(leds); i++) {
		if (gpio_is_ready_dt(&leds[i])) {
			gpio_pin_configure_dt(&leds[i], GPIO_OUTPUT_INACTIVE);
		}
	}

	size_t idx = 0;

	while (1) {
		for (size_t i = 0; i < ARRAY_SIZE(leds); i++) {
			gpio_pin_set_dt(&leds[i], i == idx ? 1 : 0);
		}
		idx = (idx + 1) % ARRAY_SIZE(leds);
		k_msleep(SLEEP_MS);
	}

	return 0;
}
