/**
 * \file
 *
 * \brief User board initialization template
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <asf.h>
#include <board.h>
#include <conf_board.h>

/**
 * \brief Set peripheral mode for one single IOPORT pin.
 * It will configure port mode and disable pin mode (but enable peripheral).
 * \param pin IOPORT pin to configure
 * \param mode Mode masks to configure for the specified pin (\ref ioport_modes)
 */
#define ioport_set_pin_peripheral_mode(pin, mode) \
	do {\
		ioport_set_pin_mode(pin, mode);\
		ioport_disable_pin(pin);\
	} while (0)

#ifdef CONF_BOARD_CONFIG_MPU_AT_INIT
/**
 *	Default memory map
 *	Address range        Memory region      Memory type   Shareability  Cache policy
 *	0x00000000- 0x1FFFFFFF Code             Normal        Non-shareable  WT
 *	0x20000000- 0x3FFFFFFF SRAM             Normal        Non-shareable  WBWA
 *	0x40000000- 0x5FFFFFFF Peripheral       Device        Non-shareable  -
 *	0x60000000- 0x7FFFFFFF RAM              Normal        Non-shareable  WBWA
 *	0x80000000- 0x9FFFFFFF RAM              Normal        Non-shareable  WT
 *	0xA0000000- 0xBFFFFFFF Device           Device        Shareable
 *	0xC0000000- 0xDFFFFFFF Device           Device        Non Shareable
 *	0xE0000000- 0xFFFFFFFF System           -                  -
 */

/**
 * \brief Set up a memory region.
 */
static void _setup_memory_region( void )
{
	uint32_t dw_region_base_addr;
	uint32_t dw_region_attr;

	__DMB();

/**
 *	ITCM memory region --- Normal
 *	START_Addr:-  0x00000000UL
 *	END_Addr:-    0x00400000UL
 */
	dw_region_base_addr = ITCM_START_ADDRESS
		| MPU_REGION_VALID
		| MPU_DEFAULT_ITCM_REGION;

	dw_region_attr = MPU_AP_PRIVILEGED_READ_WRITE
		| mpu_cal_mpu_region_size(ITCM_END_ADDRESS - ITCM_START_ADDRESS)
		| MPU_REGION_ENABLE;

	mpu_set_region(dw_region_base_addr, dw_region_attr);

/**
 *	Internal flash memory region --- Normal read-only
 *	(update to Strongly ordered in write accesses)
 *	START_Addr:-  0x00400000UL
 *	END_Addr:-    0x00600000UL
 */
	dw_region_base_addr = IFLASH_START_ADDRESS
		| MPU_REGION_VALID
		| MPU_DEFAULT_IFLASH_REGION;

	dw_region_attr = MPU_AP_READONLY
		| INNER_NORMAL_WB_NWA_TYPE(NON_SHAREABLE)
		| mpu_cal_mpu_region_size(IFLASH_END_ADDRESS - IFLASH_START_ADDRESS)
		| MPU_REGION_ENABLE;

	mpu_set_region(dw_region_base_addr, dw_region_attr);

/**
 *	DTCM memory region --- Normal
 *	START_Addr:-  0x20000000L
 *	END_Addr:-    0x20400000UL
 */
	/* DTCM memory region */
	dw_region_base_addr = DTCM_START_ADDRESS
		| MPU_REGION_VALID
		| MPU_DEFAULT_DTCM_REGION;

	dw_region_attr = MPU_AP_PRIVILEGED_READ_WRITE
		| mpu_cal_mpu_region_size(DTCM_END_ADDRESS - DTCM_START_ADDRESS)
		| MPU_REGION_ENABLE;

	mpu_set_region(dw_region_base_addr, dw_region_attr);

/**
 *	SRAM Cacheable memory region --- Normal
 *	START_Addr:-  0x20400000UL
 *	END_Addr:-    0x2043FFFFUL
 */
	/* SRAM memory  region */
	dw_region_base_addr = SRAM_FIRST_START_ADDRESS
		| MPU_REGION_VALID
		| MPU_DEFAULT_SRAM_REGION_1;

	dw_region_attr = MPU_AP_FULL_ACCESS
		| INNER_NORMAL_WB_NWA_TYPE(NON_SHAREABLE)
		| mpu_cal_mpu_region_size(SRAM_FIRST_END_ADDRESS - SRAM_FIRST_START_ADDRESS)
		| MPU_REGION_ENABLE;

	mpu_set_region(dw_region_base_addr, dw_region_attr);

/**
 *	Internal SRAM second partition memory region --- Normal
 *	START_Addr:-  0x20440000UL
 *	END_Addr:-    0x2045FFFFUL
 */
	/* SRAM memory region */
	dw_region_base_addr = SRAM_SECOND_START_ADDRESS
		| MPU_REGION_VALID
		| MPU_DEFAULT_SRAM_REGION_2;

	dw_region_attr = MPU_AP_FULL_ACCESS
		| INNER_NORMAL_WB_NWA_TYPE(NON_SHAREABLE)
		| mpu_cal_mpu_region_size(SRAM_SECOND_END_ADDRESS - SRAM_SECOND_START_ADDRESS)
		| MPU_REGION_ENABLE;

	mpu_set_region(dw_region_base_addr, dw_region_attr);

#ifdef MPU_HAS_NOCACHE_REGION
	dw_region_base_addr = SRAM_NOCACHE_START_ADDRESS
		| MPU_REGION_VALID
		| MPU_NOCACHE_SRAM_REGION;

	dw_region_attr = MPU_AP_FULL_ACCESS
		| INNER_OUTER_NORMAL_NOCACHE_TYPE(SHAREABLE)
		| mpu_cal_mpu_region_size(NOCACHE_SRAM_REGION_SIZE)
		| MPU_REGION_ENABLE;

	mpu_set_region(dw_region_base_addr, dw_region_attr);
#endif

/**
 *	Peripheral memory region --- DEVICE Shareable
 *	START_Addr:-  0x40000000UL
 *	END_Addr:-    0x5FFFFFFFUL
 */
	dw_region_base_addr = PERIPHERALS_START_ADDRESS
		| MPU_REGION_VALID
		| MPU_PERIPHERALS_REGION;

	dw_region_attr = MPU_AP_FULL_ACCESS
		| MPU_REGION_EXECUTE_NEVER
		| SHAREABLE_DEVICE_TYPE
		| mpu_cal_mpu_region_size(PERIPHERALS_END_ADDRESS - PERIPHERALS_START_ADDRESS)
		| MPU_REGION_ENABLE;

	mpu_set_region(dw_region_base_addr, dw_region_attr);

/**
 *	External EBI memory  memory region --- Strongly Ordered
 *	START_Addr:-  0x60000000UL
 *	END_Addr:-    0x6FFFFFFFUL
 */
	dw_region_base_addr = EXT_EBI_START_ADDRESS
		| MPU_REGION_VALID
		| MPU_EXT_EBI_REGION;

	dw_region_attr = MPU_AP_FULL_ACCESS
		/* External memory Must be defined with 'Device' or 'Strongly Ordered' attribute for write accesses (AXI) */
		| STRONGLY_ORDERED_SHAREABLE_TYPE
		| mpu_cal_mpu_region_size(EXT_EBI_END_ADDRESS - EXT_EBI_START_ADDRESS)
		| MPU_REGION_ENABLE;

	mpu_set_region(dw_region_base_addr, dw_region_attr);

/**
 *	SDRAM cacheable memory region --- Normal
 *	START_Addr:-  0x70000000UL
 *	END_Addr:-    0x7FFFFFFFUL
 */
	dw_region_base_addr = SDRAM_START_ADDRESS
		| MPU_REGION_VALID
		| MPU_DEFAULT_SDRAM_REGION;

	dw_region_attr = MPU_AP_FULL_ACCESS
		| INNER_NORMAL_WB_RWA_TYPE(SHAREABLE)
		| mpu_cal_mpu_region_size(SDRAM_END_ADDRESS - SDRAM_START_ADDRESS)
		| MPU_REGION_ENABLE;

	mpu_set_region(dw_region_base_addr, dw_region_attr);

/**
 *	QSPI memory region --- Strongly ordered
 *	START_Addr:-  0x80000000UL
 *	END_Addr:-    0x9FFFFFFFUL
 */
	dw_region_base_addr = QSPI_START_ADDRESS
		| MPU_REGION_VALID
		| MPU_QSPIMEM_REGION;

	dw_region_attr = MPU_AP_FULL_ACCESS
		| STRONGLY_ORDERED_SHAREABLE_TYPE
		| mpu_cal_mpu_region_size(QSPI_END_ADDRESS - QSPI_START_ADDRESS)
		| MPU_REGION_ENABLE;

	mpu_set_region(dw_region_base_addr, dw_region_attr);

/**
 *	USB RAM Memory region --- Device
 *	START_Addr:-  0xA0100000UL
 *	END_Addr:-    0xA01FFFFFUL
 */
	dw_region_base_addr = USBHSRAM_START_ADDRESS
		| MPU_REGION_VALID
		| MPU_USBHSRAM_REGION;

	dw_region_attr = MPU_AP_FULL_ACCESS
		| MPU_REGION_EXECUTE_NEVER
		| SHAREABLE_DEVICE_TYPE
		| mpu_cal_mpu_region_size(USBHSRAM_END_ADDRESS - USBHSRAM_START_ADDRESS)
		| MPU_REGION_ENABLE;

	mpu_set_region(dw_region_base_addr, dw_region_attr);

	/* Enable the memory management fault , Bus Fault, Usage Fault exception */
	SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk
		| SCB_SHCSR_BUSFAULTENA_Msk
		| SCB_SHCSR_USGFAULTENA_Msk;

	/* Enable the MPU region */
	mpu_enable(MPU_ENABLE | MPU_PRIVDEFENA);

	__DSB();
	__ISB();
}
#endif

#ifdef CONF_BOARD_ENABLE_TCM_AT_INIT
extern char _code_tcm_lma, _sitcm, _eitcm;
extern char _data_tcm_lma, _sdtcm, _edtcm;
extern char _svector, _evector;

/** \brief  TCM memory enable
* The function enables TCM memories
*/
static inline void tcm_enable(void)
{
	__DSB();
	__ISB();

	SCB->ITCMCR = SCB_ITCMCR_EN_Msk
		| SCB_ITCMCR_RMW_Msk
		| SCB_ITCMCR_RETEN_Msk;
	SCB->DTCMCR = SCB_DTCMCR_EN_Msk
		| SCB_DTCMCR_RMW_Msk
		| SCB_DTCMCR_RETEN_Msk;

	__DSB();
	__ISB();
}
#else
/** \brief  TCM memory Disable

	The function enables TCM memories
 */
static inline void tcm_disable(void)
{
	__DSB();
	__ISB();

	SCB->ITCMCR &= ~(uint32_t)(1UL);
	SCB->DTCMCR &= ~(uint32_t)SCB_DTCMCR_EN_Msk;

	__DSB();
	__ISB();
}
#endif

void board_init(void)
{
	/* This function is meant to contain board-specific initialization code
	 * for, e.g., the I/O pins. The initialization can rely on application-
	 * specific board configuration, found in conf_board.h.
	 */
	ioport_init();

#ifndef CONF_BOARD_KEEP_WATCHDOG_AT_INIT
	/* disable the watchdog */
	wdt_disable(WDT);
#else
	{
		/* configure the watchdog */
		uint32_t const ulReloadValue = wdt_get_timeout_value(
			CONF_BOARD_WATCHDOG_PERIOD*1000000, BOARD_FREQ_SLCK_XTAL);
		uint32_t const ulMode = WDT_MR_WDRSTEN|WDT_MR_WDDBGHLT|WDT_MR_WDIDLEHLT;

		if(WDT_INVALID_ARGUMENT != ulReloadValue)
		{
			wdt_init(WDT, ulMode, (uint16_t)ulReloadValue, 0xFFF);
		}
		else
		{
			printf("Error: Invalid watchdog timer period\r\n");
		}
	}
#endif

#ifdef CONF_BOARD_CONFIG_MPU_AT_INIT
	_setup_memory_region();
#endif

	/* enable the cache */
	SCB_EnableICache();
	SCB_EnableDCache();

#ifdef CONF_BOARD_ENABLE_TCM_AT_INIT
	/* TCM configuration */
	EFC->EEFC_FCR = EEFC_FCR_FKEY_PASSWD
		| EEFC_FCR_FCMD_SGPB
		| EEFC_FCR_FARG(8);
	EFC->EEFC_FCR = EEFC_FCR_FKEY_PASSWD
		| EEFC_FCR_FCMD_CGPB
		| EEFC_FCR_FARG(7);
	tcm_enable();

	{
		volatile char *dst = &_sitcm;
		volatile char *src = &_code_tcm_lma;

		/* copy code_tcm from flash to ITCM */
		while(dst < &_eitcm)
		{
			*dst++ = *src++;
		}

		/* copy data_tcm from flash to DTCM */
		dst = &_sdtcm;
		src = &_data_tcm_lma;
		while(dst < &_edtcm)
		{
			*dst++ = *src++;
		}

		/* copy the interrupt vector table to ITCM */
		dst = &_sitcm;
		src = &_svector;
		while(src < &_evector)
		{
			*dst++ = *src++;
		}
	}

	/* Set the vector table base address */
	__DSB();
	SCB->VTOR = ((uint32_t)&_sitcm & SCB_VTOR_TBLOFF_Msk);
#else
	/* TCM configuration */
	EFC->EEFC_FCR = EEFC_FCR_FKEY_PASSWD
		| EEFC_FCR_FCMD_CGPB
		| EEFC_FCR_FARG(8);
	EFC->EEFC_FCR = EEFC_FCR_FKEY_PASSWD
		| EEFC_FCR_FCMD_CGPB
		| EEFC_FCR_FARG(7);
	tcm_disable();
#endif

	/* configure USB PLL to use 16 MHz reference clock */
	UTMI->UTMI_CKTRIM = UTMI_CKTRIM_FREQ_XTAL16;

	/* configure status LED */
	ioport_enable_pin(LED0_GPIO);
	ioport_set_pin_dir(LED0_GPIO, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(LED0_GPIO, LED0_INACTIVE_LEVEL);

	/* configure USART1 pins */
	ioport_set_pin_peripheral_mode(USART1_RXD_GPIO, USART1_RXD_FLAGS);
	MATRIX->CCFG_SYSIO |= CCFG_SYSIO_SYSIO4;
	ioport_set_pin_peripheral_mode(USART1_TXD_GPIO, USART1_TXD_FLAGS);
	ioport_set_pin_peripheral_mode(USART1_SCK_GPIO, USART1_SCK_FLAGS);

	/* configure SPI pins */
	ioport_set_pin_peripheral_mode(SPI0_MISO_GPIO, SPI0_MISO_FLAGS);
	ioport_set_pin_peripheral_mode(SPI0_MOSI_GPIO, SPI0_MOSI_FLAGS);
	ioport_set_pin_peripheral_mode(SPI0_SPCK_GPIO, SPI0_SPCK_FLAGS);
	ioport_set_pin_peripheral_mode(SPI0_NPCS1_GPIO, SPI0_NPCS1_FLAGS);

	/* startup power control IO's */
	ioport_enable_pin(PIN_EXT_PWR);
	ioport_set_pin_dir(PIN_EXT_PWR, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(PIN_EXT_PWR, EXT_PWR_OFF_LEVEL);
	ioport_set_pin_peripheral_mode(CLK_480_GPIO, CLK_480_FLAGS);
	ioport_set_pin_peripheral_mode(CLK_600_GPIO, CLK_600_FLAGS);

	/* configure SDRAM pins */
	pio_configure_pin(SDRAM_BA0_PIO, SDRAM_BA0_FLAGS);
	pio_configure_pin(SDRAM_BA1_PIO, SDRAM_BA1_FLAGS);
	pio_configure_pin(SDRAM_SDCK_PIO, SDRAM_SDCK_FLAGS);
	pio_configure_pin(SDRAM_SDCKE_PIO, SDRAM_SDCKE_FLAGS);
	pio_configure_pin(SDRAM_SDCS_PIO, SDRAM_SDCS_FLAGS);
	pio_configure_pin(SDRAM_RAS_PIO, SDRAM_RAS_FLAGS);
	pio_configure_pin(SDRAM_CAS_PIO, SDRAM_CAS_FLAGS);
	pio_configure_pin(SDRAM_SDWE_PIO, SDRAM_SDWE_FLAGS);
	pio_configure_pin(SDRAM_NBS0_PIO, SDRAM_NBS0_FLAGS);
	pio_configure_pin(SDRAM_NBS1_PIO, SDRAM_NBS1_FLAGS);
	pio_configure_pin(SDRAM_A2_PIO, SDRAM_A_FLAGS);
	pio_configure_pin(SDRAM_A3_PIO, SDRAM_A_FLAGS);
	pio_configure_pin(SDRAM_A4_PIO, SDRAM_A_FLAGS);
	pio_configure_pin(SDRAM_A5_PIO, SDRAM_A_FLAGS);
	pio_configure_pin(SDRAM_A6_PIO, SDRAM_A_FLAGS);
	pio_configure_pin(SDRAM_A7_PIO, SDRAM_A_FLAGS);
	pio_configure_pin(SDRAM_A8_PIO, SDRAM_A_FLAGS);
	pio_configure_pin(SDRAM_A9_PIO, SDRAM_A_FLAGS);
	pio_configure_pin(SDRAM_A10_PIO, SDRAM_A_FLAGS);
	pio_configure_pin(SDRAM_A11_PIO, SDRAM_A_FLAGS);
	pio_configure_pin(SDRAM_SDA10_PIO, SDRAM_SDA10_FLAGS);
	pio_configure_pin(SDRAM_A13_PIO, SDRAM_A_FLAGS);
	pio_configure_pin(SDRAM_A14_PIO, SDRAM_A14_FLAGS);
	pio_configure_pin(SDRAM_D0_PIO, SDRAM_D_FLAGS);
	pio_configure_pin(SDRAM_D1_PIO, SDRAM_D_FLAGS);
	pio_configure_pin(SDRAM_D2_PIO, SDRAM_D_FLAGS);
	pio_configure_pin(SDRAM_D3_PIO, SDRAM_D_FLAGS);
	pio_configure_pin(SDRAM_D4_PIO, SDRAM_D_FLAGS);
	pio_configure_pin(SDRAM_D5_PIO, SDRAM_D_FLAGS);
	pio_configure_pin(SDRAM_D6_PIO, SDRAM_D_FLAGS);
	pio_configure_pin(SDRAM_D7_PIO, SDRAM_D_FLAGS);
	pio_configure_pin(SDRAM_D8_PIO, SDRAM_D_FLAGS);
	pio_configure_pin(SDRAM_D9_PIO, SDRAM_D_FLAGS);
	pio_configure_pin(SDRAM_D10_PIO, SDRAM_D_FLAGS);
	pio_configure_pin(SDRAM_D11_PIO, SDRAM_D_FLAGS);
	pio_configure_pin(SDRAM_D12_PIO, SDRAM_D_FLAGS);
	pio_configure_pin(SDRAM_D13_PIO, SDRAM_D_FLAGS);
	pio_configure_pin(SDRAM_D14_PIO, SDRAM_D_FLAGS);
	pio_configure_pin(SDRAM_D15_PIO, SDRAM_D_FLAGS);
	MATRIX->CCFG_SMCNFCS = CCFG_SMCNFCS_SDRAMEN;
}
