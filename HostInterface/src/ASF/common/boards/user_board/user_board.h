/**
 * \file
 *
 * \brief User board definition template
 *
 */

 /* This file is intended to contain definitions and configuration details for
 * features and devices that are available on the board, e.g., frequency and
 * startup time for an external crystal, external memory devices, LED and USART
 * pins.
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef USER_BOARD_H
#define USER_BOARD_H

#include <conf_board.h>

/* board oscillator settings */
#define BOARD_FREQ_SLCK_XTAL     32768U
#define BOARD_FREQ_SLCK_BYPASS   32768U
#define BOARD_FREQ_MAINCK_XTAL   16000000U
#define BOARD_FREQ_MAINCK_BYPASS 16000000U

/* master clock frequency */
#define BOARD_MCK (((16000000*CONFIG_PLL0_MUL)/CONFIG_PLL0_DIV)/CONFIG_SYSCLK_DIV)

/* board main clock startup time */
#define BOARD_OSC_STARTUP_US 50000

/* name of the board */
#define BOARD_NAME "HI"

/* USART1 pins definitions */
#define USART1_RXD_GPIO   PIO_PA21_IDX
#define USART1_RXD_FLAGS  IOPORT_MODE_MUX_A
#define USART1_TXD_GPIO   PIO_PB4_IDX
#define USART1_TXD_FLAGS  IOPORT_MODE_MUX_D
#define USART1_SCK_GPIO   PIO_PA23_IDX
#define USART1_SCK_FLAGS  IOPORT_MODE_MUX_A

/* LED definitions */
#define LED0_GPIO            PIO_PD8_IDX
#define LED0_ACTIVE_LEVEL    IOPORT_PIN_LEVEL_LOW
#define LED0_INACTIVE_LEVEL  IOPORT_PIN_LEVEL_HIGH

/* PPS control I/O */
#define PIN_PPS_SELECT       PIO_PD12_IDX
#define PPS_SELECT_GPS       IOPORT_PIN_LEVEL_HIGH
#define PPS_SELECT_EXT       IOPORT_PIN_LEVEL_LOW

#define PPS_LOCK_IND         PIO_PD11_IDX
#define PPS_LOCK_IND_ON      IOPORT_PIN_LEVEL_LOW
#define PPS_LOCK_IND_OFF     IOPORT_PIN_LEVEL_HIGH

/* power control signals */
#define PIN_EXT_PWR          PIO_PA2_IDX
#define EXT_PWR_OFF_LEVEL    IOPORT_PIN_LEVEL_LOW
#define EXT_PWR_ON_LEVEL     IOPORT_PIN_LEVEL_HIGH

/* power supply sync */
#define CLK_480_GPIO         PIO_PA1_IDX
#define CLK_480_FLAGS        IOPORT_MODE_MUX_B
#define CLK_600_GPIO         PIO_PA26_IDX
#define CLK_600_FLAGS        IOPORT_MODE_MUX_B
#define TC_CLK               TC0
#define TC_CLK_480_ID        ID_TC0
#define TC_CLK_480_CHAN      0
#define TC_CLK_600_ID        ID_TC2
#define TC_CLK_600_CHAN      2

/* 1 Hz timer */
#define TC_1HZ               TC1
#define TC_1HZ_ID            ID_TC3
#define TC_1HZ_CHAN          0
#define TC_1HZ_IRQn          TC3_IRQn

/* use TC Peripheral 2 */
#define TC_PPS               TC2
#define TC_PPS_CLK_IRQn      TC6_IRQn
#define TC_PPS_IN_IRQn       TC7_IRQn
#define TC_PPS_OUT_IRQn      TC8_IRQn
#define TC_PPS_CLK_Handler   TC6_Handler
#define TC_PPS_IN_Handler    TC7_Handler
#define TC_PPS_OUT_Handler   TC8_Handler

/* TC -- Timer Counter (PPS in/out) */
#define TC2_TIOA6_CHAN       0
#define PIN_TC2_TIOA7        PIO_PC8_IDX
#define PIN_TC2_TIOA7_MUX    IOPORT_MODE_MUX_B
#define PIN_TC2_TIOA7_FLAGS  IOPORT_MODE_MUX_B
#define TC2_TIOA7_CHAN       1
#define PIN_TC2_TIOA8        PIO_PC11_IDX
#define PIN_TC2_TIOA8_MUX    IOPORT_MODE_MUX_B
#define PIN_TC2_TIOA8_FLAGS  IOPORT_MODE_MUX_B
#define TC2_TIOA8_CHAN       2

/* configure TIO8 as PPS output */
#define TC_CHANNEL_PPS_OUT   TC2_TIOA8_CHAN
#define ID_TC_PPS_OUT        ID_TC8
#define PIN_TC_PPS_OUT       PIN_TC2_TIOA8
#define PIN_TC_PPS_OUT_MUX   PIN_TC2_TIOA8_MUX
/* configure TIO7 as PPS input */
#define TC_CHANNEL_PPS_IN    TC2_TIOA7_CHAN
#define ID_TC_PPS_IN         ID_TC7
#define PIN_TC_PPS_IN        PIN_TC2_TIOA7
#define PIN_TC_PPS_IN_MUX    PIN_TC2_TIOA7_MUX
/* configure TIO6 for additional PPS timing */
#define TC_CHANNEL_PPS_CLK   TC2_TIOA6_CHAN
#define ID_TC_PPS_CLK        ID_TC6


/* XDMAC channels */
#define DMA_CHANNEL_RX 1
#define DMA_CHANNEL_TX 2

/* SPI0 pins definition */
#define SPI0_MISO_GPIO       PIO_PD20_IDX
#define SPI0_MISO_FLAGS      IOPORT_MODE_MUX_B
#define SPI0_MOSI_GPIO       PIO_PD21_IDX
#define SPI0_MOSI_FLAGS      IOPORT_MODE_MUX_B
#define SPI0_NPCS0_GPIO      PIO_PB2_IDX
#define SPI0_NPCS0_FLAGS     IOPORT_MODE_MUX_D
#define SPI0_NPCS1_GPIO      PIO_PD25_IDX
#define SPI0_NPCS1_FLAGS     IOPORT_MODE_MUX_B
#define SPI0_NPCS2_GPIO      PIO_PD12_IDX
#define SPI0_NPCS2_FLAGS     IOPORT_MODE_MUX_C
#define SPI0_NPCS3_GPIO      PIO_PD27_IDX
#define SPI0_NPCS3_FLAGS     IOPORT_MODE_MUX_B
#define SPI0_SPCK_GPIO       PIO_PD22_IDX
#define SPI0_SPCK_FLAGS      IOPORT_MODE_MUX_B

/* board SDRAM size for AS4C32M16SB */
#define BOARD_SDRAM_SIZE     (64 * 1024 * 1024)

/* SDRAM memory location */
#define BOARD_SDRAM_ADDR     0x70000000UL

/* SDRAM pin definitions */
#define SDRAM_BA0_PIO        PIO_PA20_IDX
#define SDRAM_BA1_PIO        PIO_PA0_IDX
#define SDRAM_SDCK_PIO       PIO_PD23_IDX
#define SDRAM_SDCKE_PIO      PIO_PD14_IDX
#define SDRAM_SDCS_PIO       PIO_PC15_IDX
#define SDRAM_RAS_PIO        PIO_PD16_IDX
#define SDRAM_CAS_PIO        PIO_PD17_IDX
#define SDRAM_SDWE_PIO       PIO_PD29_IDX
#define SDRAM_NBS0_PIO       PIO_PC18_IDX
#define SDRAM_NBS1_PIO       PIO_PD15_IDX
#define SDRAM_A2_PIO         PIO_PC20_IDX
#define SDRAM_A3_PIO         PIO_PC21_IDX
#define SDRAM_A4_PIO         PIO_PC22_IDX
#define SDRAM_A5_PIO         PIO_PC23_IDX
#define SDRAM_A6_PIO         PIO_PC24_IDX
#define SDRAM_A7_PIO         PIO_PC25_IDX
#define SDRAM_A8_PIO         PIO_PC26_IDX
#define SDRAM_A9_PIO         PIO_PC27_IDX
#define SDRAM_A10_PIO        PIO_PC28_IDX
#define SDRAM_A11_PIO        PIO_PC29_IDX
#define SDRAM_SDA10_PIO      PIO_PD13_IDX
#define SDRAM_A13_PIO        PIO_PC31_IDX
#define SDRAM_A14_PIO        PIO_PA18_IDX
#define SDRAM_D0_PIO         PIO_PC0_IDX
#define SDRAM_D1_PIO         PIO_PC1_IDX
#define SDRAM_D2_PIO         PIO_PC2_IDX
#define SDRAM_D3_PIO         PIO_PC3_IDX
#define SDRAM_D4_PIO         PIO_PC4_IDX
#define SDRAM_D5_PIO         PIO_PC5_IDX
#define SDRAM_D6_PIO         PIO_PC6_IDX
#define SDRAM_D7_PIO         PIO_PC7_IDX
#define SDRAM_D8_PIO         PIO_PE0_IDX
#define SDRAM_D9_PIO         PIO_PE1_IDX
#define SDRAM_D10_PIO        PIO_PE2_IDX
#define SDRAM_D11_PIO        PIO_PE3_IDX
#define SDRAM_D12_PIO        PIO_PE4_IDX
#define SDRAM_D13_PIO        PIO_PE5_IDX
#define SDRAM_D14_PIO        PIO_PA15_IDX
#define SDRAM_D15_PIO        PIO_PA16_IDX

#define SDRAM_BA0_FLAGS      PIO_PERIPH_C
#define SDRAM_BA1_FLAGS      PIO_PERIPH_C
#define SDRAM_SDCK_FLAGS     PIO_PERIPH_C
#define SDRAM_SDCKE_FLAGS    PIO_PERIPH_C
#define SDRAM_SDCS_FLAGS     PIO_PERIPH_A
#define SDRAM_RAS_FLAGS      PIO_PERIPH_C
#define SDRAM_CAS_FLAGS      PIO_PERIPH_C
#define SDRAM_SDWE_FLAGS     PIO_PERIPH_C
#define SDRAM_NBS0_FLAGS     PIO_PERIPH_A
#define SDRAM_NBS1_FLAGS     PIO_PERIPH_C
#define SDRAM_A_FLAGS        PIO_PERIPH_A
#define SDRAM_A14_FLAGS      PIO_PERIPH_C
#define SDRAM_SDA10_FLAGS    PIO_PERIPH_C
#define SDRAM_D_FLAGS        PIO_PERIPH_A


#endif // USER_BOARD_H
