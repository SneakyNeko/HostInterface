/** ***************************************************************************
File Name:  main.c

Project:    Platform 4

Purpose:    Test program for the Platform 4 data channel

Program:    Host Interface

Compiler:   This program was developed using AtmelStudio 7

Author:     Tristan Losier, September 26, 2018

            Copyright (C) Ocean Sonics Ltd, Nova Scotia, Canada.
            Copying in whole or in part without prior written permission of
            Ocean Sonics is prohibited.

Modified:   $Id$

******************************************************************************/

/* System Include Files */
#include <string.h>

/* Local Include Files */
#include "asf.h"
#include "conf_board.h"
#include "conf_clock.h"
#include "conf_example.h"


/* Module Definitions */

/* print the received string out the USB COM port
	Note: If this is enabled, a serial console needs to be connected to the
	USB COM port, otherwise the program locks up */
#define USB_ENABLE 0

/* enable the down-stream power supply
	Note: DO NOT ENABLE if the TX/RX signals are connected together! */
#define DOWN_STREAM_POWER_ENABLE 0

/* character that marks the start of a data packet */
#define SYNC_CHAR '*'
/* length of the packet preamble/sync pattern */
#define SYNC_SEQ_LEN 3
/* size of the data block being sent */
#define BUFFER_SIZE sizeof(acTxBuffer)

#define TEST_DATA "  *Lorem ipsum dolor sit amet, consectetur adipiscing elit,"\
	" sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut en"\
	"im ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut ali"\
	"quip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in v"\
	"oluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sin"\
	"t occaecat cupidatat non proident, sunt in culpa qui officia deserunt mol"\
	"lit anim id est laborum.\r\n";


/* Module Type Definitions */

/* Module Function Declarations */

static void InitHardware(void);


/* Module Variable Declarations */

/* transmit buffer */
static const char acTxBuffer[] = TEST_DATA;
/* receive buffer */
static volatile char acRxBuffer[BUFFER_SIZE] = { 0 };

/* DMA configuration structures */
static xdmac_channel_config_t stTxConfig;
static xdmac_channel_config_t stRxConfig;

/* state/signaling variables */
static volatile char cLastRxSuccess = 0;
static volatile char cNewDataReceved = 0;


/* Global Variables (Must be justified!) */

/* Global Function Implementations */

/** ***************************************************************************
	Name:               main

	Creator:            Tristan Losier
	Last Changed By:    Tristan Losier

	Output:             1
	Caveats / Effect:   None

	Description:
	Program entry point.
*/
int main(void)
{
	/* system initialization */
	sysclk_init();
	board_init();
	SCB_DisableDCache();
	InitHardware();

	while(1)
	{
		char *pcBuffer;

		/* wait for a data packet to arrive */
		while(!cNewDataReceved) {};
		cNewDataReceved = 0;

		/* find the sync char in the RX buffer, which indicates the start of a
			message */
		pcBuffer = (char*)memchr((void*)acRxBuffer, SYNC_CHAR, BUFFER_SIZE) + 1;

		/* verify the sync char was found, and that the received message
			matches the sent message */
		if(pcBuffer
			&& !memcmp(acTxBuffer+SYNC_SEQ_LEN, pcBuffer, BUFFER_SIZE-SYNC_SEQ_LEN))
		{
			/* the message was good, signal success */
			cLastRxSuccess = 1;
			ioport_set_pin_level(LED0_GPIO, LED0_ACTIVE_LEVEL);
		}
		else
		{
			/* bad message, signal failure */
			cLastRxSuccess = 0;
			ioport_set_pin_level(LED0_GPIO, LED0_INACTIVE_LEVEL);
		}

		/* now the packet has been processed, prepare to receive the next */
		{
			/* ensure the USART Receive Holding Register is empty */
			uint32_t dummy;
			usart_read(USART1, &dummy);
			UNUSED(dummy);
		}
		/* clear the RX buffer */
		memset((void*)acRxBuffer, 0, BUFFER_SIZE);
		/* restart the RX DMA to receive the next incoming message */
		xdmac_configure_transfer(XDMAC, DMA_CHANNEL_RX, &stRxConfig);
		xdmac_channel_enable(XDMAC, DMA_CHANNEL_RX);

#if USB_ENABLE
		{
			/* if USB is enabled, print the received data out the USB virtual
				serial port */
			unsigned int i;
			for(i = 0; i < BUFFER_SIZE-3; i++)
			{
				while(!udi_cdc_is_tx_ready()) {};
				if(pcBuffer[i] != '\0')
				{
					udi_cdc_putc(pcBuffer[i]);
				}
			}
		}
#endif
	}

	/* we should never get here */
	return 1;
}

/** ***************************************************************************
	Name:               TC3_Handler

	Creator:            Tristan Losier
	Last Changed By:    Tristan Losier

	Output:             None
	Caveats / Effect:   ISR

	Description:
	This is a 1 Hz ISR, driven by timer 1 channel 0. It re-starts the TX DMA
	channel and clears the status LED if no data has come in over the last
	second.
*/
void TC3_Handler(void)
{
	uint32_t const status = tc_get_status(TC1, 0);

	/* make sure we are servicing the right interrupt */
	if(status & TC_SR_CPCS)
	{
		/* was data successfully received over the last second? */
		if(!cLastRxSuccess)
		{
			/* last data packet wasn't received, clear the status LED */
			ioport_set_pin_level(LED0_GPIO, LED0_INACTIVE_LEVEL);
		}
		cLastRxSuccess = 0;
		/* start the TX DMA to begin the next packet transmission */
		xdmac_configure_transfer(XDMAC, DMA_CHANNEL_TX, &stTxConfig);
		xdmac_channel_enable(XDMAC, DMA_CHANNEL_TX);
	}
}

/** ***************************************************************************
	Name:               USART1_Handler

	Creator:            Tristan Losier
	Last Changed By:    Tristan Losier

	Output:             None
	Caveats / Effect:   ISR

	Description:
	This ISR fires when the USART RX timeout expires. This signals the end of
	the transmission, so we use the opportunity to stop the RX DMA transaction
	and signal the main program loop to deal with the received data.
*/
void USART1_Handler(void)
{
	uint32_t const ul_status = usart_get_status(USART1);

	/* is this a timeout interrupt? */
	if(ul_status & US_IER_TIMEOUT)
	{
		/* reset the RX timeout, it will reactivate when the next character is
			received */
		usart_start_rx_timeout(USART1);
		/* stop the RX DMA */
		xdmac_channel_disable(XDMAC, DMA_CHANNEL_RX);
		/* wait for the DMA to finish writing any buffered data */
		while(xdmac_channel_get_status(XDMAC) & XDMAC_GS_ST2) {};
		/* signal the main program loop to process the received packet */
		cNewDataReceved = 1;
	}
}

/** ***************************************************************************
	Name:               XDMAC_Handler

	Creator:            Tristan Losier
	Last Changed By:    Tristan Losier

	Output:             None
	Caveats / Effect:   ISR

	Description:
	This ISR fires when the RX DMA channel finishes a transaction. This won't
	normally happen as the first character sent is usually lost, causing the
	DMA channel to hang waiting for the lost byte (and thus the need for the
	USART timeout interrupt). But if the whole packet gets through, we can use
	this interrupt to restart the USART timeout and signal the main program
	loop to deal with the received data.
*/
void XDMAC_Handler(void)
{
	uint32_t const status =
		xdmac_channel_get_interrupt_status(XDMAC, DMA_CHANNEL_RX);

	/* is this the transaction complete interrupt? */
	if(status & XDMAC_CIS_BIS)
	{
		/* reset the RX timeout, it will reactivate when the next character is
			received */
		usart_start_rx_timeout(USART1);
		/* signal the main program loop to process the received packet */
		cNewDataReceved = 1;
	}
}


/* Module Function Implementations */

/** ***************************************************************************
	Name:               InitHardware

	Creator:            Tristan Losier
	Last Changed By:    Tristan Losier

	Output:             None
	Caveats / Effect:   None

	Description:
	Initializes the system peripherals being used.
*/
static void InitHardware(void)
{
	/* switch SLCK to external crystal */
	osc_enable(OSC_SLCK_32K_XTAL);
	osc_wait_ready(OSC_SLCK_32K_XTAL);

	/* init USB */
#if USB_ENABLE
	udc_start();
#endif

#if DOWN_STREAM_POWER_ENABLE
	/* configure timer 0, channel 0, to produce a 480 kHz synchronization
		signal for the down-stream power supply */
	sysclk_enable_peripheral_clock(TC_CLK_480_ID);
	tc_init(TC_CLK, TC_CLK_480_CHAN, TC_CMR_TCCLKS_TIMER_CLOCK2|TC_CMR_WAVE|TC_CMR_WAVSEL_UP_RC|TC_CMR_BCPC_CLEAR|TC_CMR_BCPB_SET);
	tc_write_rc(TC_CLK, TC_CLK_480_CHAN, 30);
	tc_write_rb(TC_CLK, TC_CLK_480_CHAN, 15);
	tc_start(TC_CLK, TC_CLK_480_CHAN);

	/* turn on the power supply */
	ioport_set_pin_level(PIN_EXT_PWR, EXT_PWR_ON_LEVEL);
#endif

	/* configure timer 0, channel 2, to produce a 600 kHz synchronization
		signal for the main power supply */
	sysclk_enable_peripheral_clock(TC_CLK_600_ID);
	tc_init(TC_CLK, TC_CLK_600_CHAN, TC_CMR_TCCLKS_TIMER_CLOCK2|TC_CMR_WAVE|TC_CMR_WAVSEL_UP_RC|TC_CMR_ACPC_CLEAR|TC_CMR_ACPA_SET);
	tc_write_rc(TC_CLK, TC_CLK_600_CHAN, 24);
	tc_write_ra(TC_CLK, TC_CLK_600_CHAN, 12);
	tc_start(TC_CLK, TC_CLK_600_CHAN);

	/* initialize and enable DMA controller */
	pmc_enable_periph_clk(ID_XDMAC);
	xdmac_channel_set_descriptor_control(XDMAC, DMA_CHANNEL_RX, XDMAC_CNDC_NDE_DSCR_FETCH_DIS);
	xdmac_channel_set_descriptor_control(XDMAC, DMA_CHANNEL_TX, XDMAC_CNDC_NDE_DSCR_FETCH_DIS);
	xdmac_enable_interrupt(XDMAC, DMA_CHANNEL_RX);
	xdmac_channel_enable_interrupt(XDMAC, DMA_CHANNEL_RX, XDMAC_CIE_BIE);
	NVIC_EnableIRQ(XDMAC_IRQn);

	/* XDMAC USART transmission channel config */
	stTxConfig.mbr_ubc = BUFFER_SIZE;
	stTxConfig.mbr_sa  = (uint32_t)acTxBuffer;
	stTxConfig.mbr_da  = (uint32_t)&USART1->US_THR;
	stTxConfig.mbr_cfg = XDMAC_CC_TYPE_PER_TRAN
		| XDMAC_CC_DSYNC_MEM2PER
		| XDMAC_CC_SWREQ_HWR_CONNECTED
		| XDMAC_CC_CSIZE_CHK_1
		| XDMAC_CC_DWIDTH_BYTE
		| XDMAC_CC_SIF_AHB_IF1
		| XDMAC_CC_DIF_AHB_IF1
		| XDMAC_CC_SAM_INCREMENTED_AM
		| XDMAC_CC_DAM_FIXED_AM
		| XDMAC_CC_PERID(XDMAC_CHANNEL_HWID_USART1_TX);
	stTxConfig.mbr_bc  = 0;
	stTxConfig.mbr_ds  = 0;
	stTxConfig.mbr_sus = 0;
	stTxConfig.mbr_dus = 0;

	/* XDMAC USART reception channel config */
	stRxConfig.mbr_ubc = BUFFER_SIZE;
	stRxConfig.mbr_sa  = (uint32_t)&USART1->US_RHR;
	stRxConfig.mbr_da  = (uint32_t)acRxBuffer;
	stRxConfig.mbr_cfg = XDMAC_CC_TYPE_PER_TRAN
		| XDMAC_CC_DSYNC_PER2MEM
		| XDMAC_CC_SWREQ_HWR_CONNECTED
		| XDMAC_CC_CSIZE_CHK_1
		| XDMAC_CC_DWIDTH_BYTE
		| XDMAC_CC_SIF_AHB_IF1
		| XDMAC_CC_DIF_AHB_IF0
		| XDMAC_CC_SAM_FIXED_AM
		| XDMAC_CC_DAM_INCREMENTED_AM
		| XDMAC_CC_PERID(XDMAC_CHANNEL_HWID_USART1_RX);
	stRxConfig.mbr_bc  = 0;
	stRxConfig.mbr_ds  = 0;
	stRxConfig.mbr_sus = 0;
	stRxConfig.mbr_dus = 0;

	/* start the RX DMA */
	xdmac_configure_transfer(XDMAC, DMA_CHANNEL_RX, &stRxConfig);
	xdmac_channel_enable(XDMAC, DMA_CHANNEL_RX);

	/* configure USART */
	{
		sam_usart_opt_t usart_console_settings = {
			14400000UL,
			US_MR_CHRL_8_BIT,
			US_MR_PAR_NO,
			US_MR_NBSTOP_1_BIT,
			US_MR_CHMODE_NORMAL,
			0
		};

		sysclk_enable_peripheral_clock(ID_USART1);
		usart_init_rs232(USART1, &usart_console_settings, sysclk_get_peripheral_hz());
		USART1->US_MR |= US_MR_MAN; // Enable Manchester encoder
		USART1->US_MAN = US_MAN_RXIDLEV|US_MAN_ONE|US_MAN_RX_PL(0)|US_MAN_TX_PL(0); // Disable RX/TX preamble
		usart_set_rx_timeout(USART1, 0xFFFF);
		usart_enable_interrupt(USART1, US_IER_TIMEOUT);
		usart_enable_tx(USART1);
		usart_enable_rx(USART1);
		usart_start_rx_timeout(USART1);
		NVIC_EnableIRQ(USART1_IRQn);
	}

	/* setup timer 1, channel 0, to produce a 1 second interrupt */
	sysclk_enable_peripheral_clock(TC_1HZ_ID);
	tc_init(TC_1HZ, TC_1HZ_CHAN, TC_CMR_TCCLKS_TIMER_CLOCK5|TC_CMR_WAVE|TC_CMR_WAVSEL_UP_RC);
	tc_write_rc(TC_1HZ, TC_1HZ_CHAN, 32768);
	tc_enable_interrupt(TC_1HZ, TC_1HZ_CHAN, TC_IER_CPCS);
	NVIC_EnableIRQ(TC_1HZ_IRQn);
	tc_start(TC_1HZ, TC_1HZ_CHAN);
}


/***********************  E N D   O F   F I L E  *****************************/
