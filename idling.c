
int main(void)
{
    int pid;
    int led[4];
    int i, j;

#ifdef PYBOARD
# define LED0 "/dev/gpio_1_13"
# define LED1 "/dev/gpio_1_14"
# define LED2 "/dev/gpio_1_15"
# define LED3 "/dev/gpio_1_4"
#elif defined (STM32F4)
# define LED0 "/dev/gpio_3_12"
# define LED1 "/dev/gpio_3_13"
# define LED2 "/dev/gpio_3_14"
# define LED3 "/dev/gpio_3_15"
#elif defined (LPC17XX)
#if 0
/*LPCXpresso 1769 */
# define LED0 "/dev/gpio_0_22"
# define LED1 "/dev/null"
# define LED2 "/dev/null"
# define LED3 "/dev/null"
#else
/* mbed 1768 */
# define LED0 "/dev/gpio_1_18"
# define LED1 "/dev/gpio_1_20"
# define LED2 "/dev/gpio_1_21"
# define LED3 "/dev/gpio_1_23"
#endif
#else
# define LED0 "/dev/null"
# define LED1 "/dev/null"
# define LED2 "/dev/null"
# define LED3 "/dev/null"
#endif

    led[0] = open(LED0, O_RDWR, 0);
    led[1] = open(LED1, O_RDWR, 0);
    led[2] = open(LED2, O_RDWR, 0);
    led[3] = open(LED3, O_RDWR, 0);

    if (led[0] >= 0) {
        while(1) {
            for (i = 0; i < 9; i++) {
                if (i < 4) {
                    write(led[i], "0", 1);
                } else {
                    char val = (1 - (i % 2)) + '0';
                    for(j = 0; j < 4; j++)
                        write(led[j], &val, 1);
                }
                sleep(200);
            }
        }
    } else {
        while(1) { sleep(1000); } /* GPIO unavailable, just sleep. */
    }
}
