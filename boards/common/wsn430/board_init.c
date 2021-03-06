/*
 * Copyright (C) 2013 Milan Babel <babel@inf.fu-berlin.de>
 *               2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

 /**
  * @ingroup     boards_common_wsn430
  * @{
  *
  * @file
  * @brief       Board initialization for WSN430
  *
  * @author      Milan Babel <babel@inf.fu-berlin.de>
  * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
  *
  * @}
  */

#include "cpu.h"
#include "irq.h"
#include "assert.h"
#include "stdio_uart.h"
#include "periph_conf.h"

enum {
    MCLK_2MHZ_SCLK_1MHZ = (SELM_2 | DIVM_2 | SELS | DIVS_3),
    MCLK_4MHZ_SCLK_1MHZ = (SELM_2 | DIVM_1 | SELS | DIVS_3),
    MCLK_8MHZ_SCLK_1MHZ = (SELM_2 | SELS | DIVS_3),
    MCLK_8MHZ_SCLK_8MHZ = (SELM_2 | SELS),
};

static void msb_ports_init(void)
{
    /* Port 1: GDO, Flash, BSL TX */
    P1SEL = 0x02;    /* Port1 Select: 00000010 = 0x02 */
    P1OUT = 0x00;    /* Port1 Output: 00000000 = 0x00 */
    P1DIR = 0x87;    /* Port1 Direction: 10000111 = 0x87 */

    /* Port 2: GPIO, BSL RX, 1wire */
    P2SEL = 0x04;    /* Port2 Select: 00000100 = 0x04 */
    P2OUT = 0x00;    /* Port2 Output: 00000000 = 0x00 */
    P2DIR = 0xFF;    /* Port2 Direction: 11111111 = 0xFF */


    /* Port 3: UART */
    P3SEL = 0xFE;    /* Port3 Select: 11111110 = 0xFE */
    P3OUT = 0x00;    /* Port3 Output: 00000000 = 0x00 */
    P3DIR = 0xFF;    /* Port3 Direction: 11111111 = 0xFF */


    /* Port 4: CS */
    P4SEL = 0x00;    /* Port4 Select: 00000000 = 0x00 */
    P4OUT = 0x14;    /* Port4 Output: 00010100 = 0x14 */
    P4DIR = 0xFF;    /* Port4 Direction: 11111111 = 0xFF */

    /* Port 5: SPI, LED */
    P5SEL = 0x0E;    /* Port5 Select: 00001110 = 0x0E */
    P5OUT = 0x70;    /* Port5 Output: 01110000 = 0x70 */
    P5DIR = 0x70;    /* Port5 Direction: 01110000 = 0x70 */


    P6SEL = 0xFF;    /* Port6 Select: 11111111 = 0xFF */
    P6OUT = 0x00;    /* Port6 Output: 00000000 = 0x00 */
    P6DIR = 0xFF;    /* Port6 Direction: 11111000 = 0xF8 */

}

static void msp430_init_cpuclk(uint8_t speed)
{
    /* stop watchdog */
    WDTCTL = WDTPW + WDTHOLD;
    BCSCTL1 = RSEL2;

    /* wait for XTAL to stabilize */
    do {
        /* clear oscillator fault flag */
        IFG1 &= ~OFIFG;
        /* time for flag to set */
        for (uint16_t i = 0xFF; i > 0; i--) {}
    } while ((IFG1 & OFIFG) != 0);

    /* apply clock config */
    BCSCTL2 = speed;
}

void board_init(void)
{
    msp430_cpu_init();
    msb_ports_init();

    /* initialize CPU clock */
    unsigned state = irq_disable();
    msp430_init_cpuclk(CLOCK_MODE);
    irq_restore(state);

    /* initialize STDIO over UART */
    stdio_init();
}
