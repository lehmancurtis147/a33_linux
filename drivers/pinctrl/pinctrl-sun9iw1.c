/*
 * driver/pinctrl/pinctrl-sun9iw1.c
/ *
 * Copyright(c) 2013-2015 Allwinnertech Co., Ltd. 
 *         http://www.allwinnertech.com
 *
 * Author: sunny <sunny@allwinnertech.com>
 *
 * allwinner sunxi SoCs pinctrl driver.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/io.h>
#include <linux/clk.h>
#include <linux/gpio.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/pinctrl/consumer.h>
#include <linux/pinctrl/machine.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/pinconf-generic.h>
#include <linux/pinctrl/pinmux.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <mach/sys_config.h>
#include <mach/platform.h>
#include "core.h"
#include "pinctrl-sunxi.h"

static const struct sunxi_desc_pin sun9i_w1p1_pins[]={
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PA0,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "gmac0"),           /* grxd3 */		
		SUNXI_FUNCTION(0x4, "uart1"),		/* tX */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT0 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PA1,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "gmac0"),           /* grxd2 */
		SUNXI_FUNCTION(0x4, "uart1"),		/* RX */
		SUNXI_FUNCTION(0x5, "Vdevice"),		/*virtual device for pinctrl testing*/
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT1 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PA2,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "gmac0"),           /* grxd1 */
		SUNXI_FUNCTION(0x4, "uart1"),		/* RTS */
		SUNXI_FUNCTION(0x5, "Vdevice"),		/*virtual device for pinctrl testing*/
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT2 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PA3,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "gmac0"),           /* grxd0 */
		SUNXI_FUNCTION(0x4, "uart1"),		/* CTS */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT3 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PA4,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "gmac0"),           /* grxck */
		SUNXI_FUNCTION(0x4, "uart1"),		/* DTR */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT4 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PA5,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "gmac0"),           /* grxctl/erxdv */
		SUNXI_FUNCTION(0x4, "uart1"),		/* DSR */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT5 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PA6,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "gmac0"),           /* gnull/erxerr */
		SUNXI_FUNCTION(0x4, "uart1"),		/* DCD */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT6 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PA7,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "gmac0"),           /* gtxd3 */
		SUNXI_FUNCTION(0x4, "uart1"),		/* RING */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT7 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PA8,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "gmac0"),           /* gtxd2 */
		SUNXI_FUNCTION(0x4, "eclk"),		/* IN0 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT8 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PA9,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "gmac0"),           /* gtxd1 */
		SUNXI_FUNCTION(0x4, "eclk"),		/* IN1 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT9 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PA10,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "gmac0"),           /* gtxd0 */
		SUNXI_FUNCTION(0x4, "clka"),		/* OUT */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT10 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PA11,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "gmac0"),           /* gtxck/ecrs */
		SUNXI_FUNCTION(0x4, "clkb"),		/* OUT */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT11 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PA12,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "gmac0"),           /* gnull/etxck */
		SUNXI_FUNCTION(0x4, "pwm3"),		/* P */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT12 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PA13,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "gmac0"),           /* gtxctl/etxen */
		SUNXI_FUNCTION(0x4, "pwm3"),		/* N */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT13 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PA14,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "gmac0"),           /* gnull/etxerr */		
		SUNXI_FUNCTION(0x4, "spi1"),		/* CS0 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT14 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PA15,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "gmac0"),           /* gclkin/ecol */
		SUNXI_FUNCTION(0x4, "spi1"),		/* CLK */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT15 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PA16,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "gmac0"),           /* gmdc */
		SUNXI_FUNCTION(0x4, "spi1"),		/* MOSI */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT16 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PA17,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "gmac0"),           /* gmdio */
		SUNXI_FUNCTION(0x4, "spi1"),		/* MISO */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT17 */
		
	/* HOLE */
#ifdef CONFIG_FPGA_V4_PLATFORM 
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PB0,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "twi0"),		/* MCLK */
		SUNXI_FUNCTION(0x3, "uart3"),		/* CTS */		
		SUNXI_FUNCTION(0x4, "ac97"),		/* MCLK */
		SUNXI_FUNCTION(0x5, "trace0"),		/* DOUT0 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT0 */
#else
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PB0,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "i2s0"),
		SUNXI_FUNCTION(0x3, "uart3"),
		SUNXI_FUNCTION(0x4, "ac97"),
		SUNXI_FUNCTION(0x5, "trace0"),
		SUNXI_FUNCTION(0x6, "eint")),
#endif

#ifdef CONFIG_FPGA_V4_PLATFORM
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PB1,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "twi0"),		/* BCLK */		
		SUNXI_FUNCTION(0x4, "ac97"),		/* BCLK */
		SUNXI_FUNCTION(0x5, "trace0"),		/* DOUT1 */
		SUNXI_FUNCTION(0x6, "eint")),
#else
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PB1,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "i2s0"),
		SUNXI_FUNCTION(0x4, "ac97"),
		SUNXI_FUNCTION(0x5, "trace0"),
		SUNXI_FUNCTION(0x6, "eint")),
#endif
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PB2,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "i2s0"),		/* LRCK */		
		SUNXI_FUNCTION(0x4, "ac97"),		/* SYNC */
		SUNXI_FUNCTION(0x5, "trace0"),		/* DOUT2 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT2 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PB3,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "i2s0"),		/* DO0 */		
		SUNXI_FUNCTION(0x4, "ac97"),		/* DO */
		SUNXI_FUNCTION(0x5, "trace0"),		/* DOUT3 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT3 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PB4,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "i2s0"),		/* DO1 */		
		SUNXI_FUNCTION(0x3, "uart3"),		/* RTS */
		SUNXI_FUNCTION(0x5, "trace0"),		/* DOUT4 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT4 */	
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PB5,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "i2s0"),		/* DO2 */		
		SUNXI_FUNCTION(0x3, "uart3"),		/* TX */
		SUNXI_FUNCTION(0x5, "trace0"),		/* DOUT5 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT5 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PB6,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "i2s0"),		/* DO3 */		
		SUNXI_FUNCTION(0x3, "uart3"),		/* RX */
		SUNXI_FUNCTION(0x5, "trace0"),		/* DOUT6 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT6 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PB7,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "i2s0"),		/* DI */		
		SUNXI_FUNCTION(0x4, "ac97"),		/* DI */
		SUNXI_FUNCTION(0x5, "trace0"),		/* DOUT7 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT7 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PB8,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "ulpi"),		/* D0 */		
		SUNXI_FUNCTION(0x3, "hsi_tx"),		/* DATA */
		SUNXI_FUNCTION(0x5, "trace0"),		/* DOUT8 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT8 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PB9,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "ulpi"),		/* D1 */		
		SUNXI_FUNCTION(0x3, "hsi_tx"),		/* FLG */
		SUNXI_FUNCTION(0x5, "trace0"),		/* DOUT9 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT9 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PB10,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "ulpi"),		/* D2 */		
		SUNXI_FUNCTION(0x3, "hsi_tx"),		/* RDY */
		SUNXI_FUNCTION(0x5, "trace0"),		/* DOUT10 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT10 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PB11,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "ulpi"),		/* D3 */		
		SUNXI_FUNCTION(0x3, "hsi_rx"),		/* DATA */
		SUNXI_FUNCTION(0x5, "trace0"),		/* DOUT11 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT11 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PB12,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "ulpi"),		/* D4 */		
		SUNXI_FUNCTION(0x3, "hsi_rx"),		/* FLG */
		SUNXI_FUNCTION(0x5, "trace0"),		/* DOUT12 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT12 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PB13,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "ulpi"),		/* D5 */		
		SUNXI_FUNCTION(0x3, "hsi_rx"),		/* RDY */
		SUNXI_FUNCTION(0x5, "trace0"),		/* DOUT13 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT13 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PB14,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "ulpi"),		/* D6 */		
		SUNXI_FUNCTION(0x3, "csi0"),		/* MCLK */
		SUNXI_FUNCTION(0x5, "trace0"),		/* DOUT14 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT14 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PB15,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "ulpi"),		/* D7 */		
		SUNXI_FUNCTION(0x3, "csi0"),		/* SCK */
		SUNXI_FUNCTION(0x4, "twi4"),		/* SCK */
		SUNXI_FUNCTION(0x5, "trace0"),		/* DOUT15 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT15 */	
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PB16,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "ulpi"),		/* CLK */		
		SUNXI_FUNCTION(0x3, "csi0"),		/* SDA */
		SUNXI_FUNCTION(0x4, "twi4"),		/* SDA */
		SUNXI_FUNCTION(0x5, "trace0"),		/* CLK */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT16 */	
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PB17,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "ulpi"),		/* DIR */		
		SUNXI_FUNCTION(0x5, "trace0"),		/* CTL */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT17 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PB18,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "ulpi"),		/* STP */		
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT18 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PB19,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "ulpi"),		/* NXT */		
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT19 */
		
	/*HOLE */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PC0,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* WE */		
		SUNXI_FUNCTION(0x3, "spi0")),		/* MOSI */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PC1,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* ALE */		
		SUNXI_FUNCTION(0x3, "spi0")),		/* MISO */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PC2,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* CLE */		
		SUNXI_FUNCTION(0x3, "spi0")),		/* CLK */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PC3,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0")),		/* CE1 */		
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PC4,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0")),		/* CE0 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PC5,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0")),		/* RE */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PC6,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* RB0 */		
		SUNXI_FUNCTION(0x3, "sdc2")),		/* CMD */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PC7,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* RB1 */		
		SUNXI_FUNCTION(0x3, "sdc2")),		/* CLK */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PC8,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* DQ0 */		
		SUNXI_FUNCTION(0x3, "sdc2")),		/* D0 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PC9,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* DQ1 */		
		SUNXI_FUNCTION(0x3, "sdc2")),		/* D1 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PC10,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* DQ2 */		
		SUNXI_FUNCTION(0x3, "sdc2")),		/* D2 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PC11,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* DQ3 */		
		SUNXI_FUNCTION(0x3, "sdc2")),		/* D3 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PC12,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* DQ4 */		
		SUNXI_FUNCTION(0x3, "sdc2")),		/* D4 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PC13,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* DQ5 */		
		SUNXI_FUNCTION(0x3, "sdc2")),		/* D5 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PC14,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* DQ6 */		
		SUNXI_FUNCTION(0x3, "sdc2")),		/* D6 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PC15,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* DQ7 */		
		SUNXI_FUNCTION(0x3, "sdc2")),		/* D7 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PC16,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* DQS */		
		SUNXI_FUNCTION(0x3, "sdc2")),		/* RST */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PC17,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* CE2 */		
		SUNXI_FUNCTION(0x3, "nand0")),		/* RE_B */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PC18,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* CE3 */		
		SUNXI_FUNCTION(0x3, "nand0")),		/* DQS_B */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PC19,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),		
		SUNXI_FUNCTION(0x3, "spi0")),		/* CS0 */

	/* HOLE */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PD0,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D0 */		
		SUNXI_FUNCTION(0x3, "lvds0")),		/* VP0 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PD1,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D1 */		
		SUNXI_FUNCTION(0x3, "lvds0")),		/* VN0 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PD2,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D2 */		
		SUNXI_FUNCTION(0x3, "lvds0")),		/* VP1 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PD3,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D3 */		
		SUNXI_FUNCTION(0x3, "lvds0")),		/* VN1 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PD4,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D4 */		
		SUNXI_FUNCTION(0x3, "lvds0")),		/* VP2 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PD5,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D5 */		
		SUNXI_FUNCTION(0x3, "lvds0")),		/* VN2 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PD6,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D6 */		
		SUNXI_FUNCTION(0x3, "lvds0")),		/* VPC */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PD7,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D7 */		
		SUNXI_FUNCTION(0x3, "lvds0")),		/* VNC */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PD8,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D8 */		
		SUNXI_FUNCTION(0x3, "lvds0")),		/* VP3 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PD9,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D9 */		
		SUNXI_FUNCTION(0x3, "lvds0")),		/* VN3 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PD10,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D10 */		
		SUNXI_FUNCTION(0x3, "lvds1")),		/* VP0 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PD11,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D11 */		
		SUNXI_FUNCTION(0x3, "lvds1")),		/* VN0 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PD12,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D12 */		
		SUNXI_FUNCTION(0x3, "lvds1")),		/* VP1 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PD13,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D13 */		
		SUNXI_FUNCTION(0x3, "lvds1")),		/* VN1 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PD14,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D14 */		
		SUNXI_FUNCTION(0x3, "lvds1")),		/* VP2 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PD15,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D15 */		
		SUNXI_FUNCTION(0x3, "lvds1")),		/* VN2 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PD16,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D16 */		
		SUNXI_FUNCTION(0x3, "lvds1")),		/* VPC */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PD17,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D17 */		
		SUNXI_FUNCTION(0x3, "lvds1")),		/* VNC */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PD18,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D18 */		
		SUNXI_FUNCTION(0x3, "lvds1")),		/* VP3 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PD19,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D19 */		
		SUNXI_FUNCTION(0x3, "lvds1")),		/* VN3 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PD20,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0")),		/* D20 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PD21,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0")),		/* D21 */	
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PD22,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0")),		/* D22 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PD23,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0")),		/* D23 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PD24,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0")),		/* CLK */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PD25,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0")),		/* DE */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PD26,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0")),		/* HSYNC */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PD27,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0")),		/* CSYNC */			
						
	/* HOLE */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PE0,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi1"),		/* PCLK */
		SUNXI_FUNCTION(0x3, "ts0"),		/* CLK */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT0 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PE1,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi1"),		/* MCLK */
		SUNXI_FUNCTION(0x3, "ts0"),		/* ERR */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT1 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PE2,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi1"),		/* HSYNC */
		SUNXI_FUNCTION(0x3, "ts0"),		/* SYNC */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT2 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PE3,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi1"),		/* VSYNC */
		SUNXI_FUNCTION(0x3, "ts0"),		/* DVLD */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT3 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PE4,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi1"),		/* D0 */
		SUNXI_FUNCTION(0x3, "spi2"),		/* CS0 */
		SUNXI_FUNCTION(0x4, "uart5"),		/* TX */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT4 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PE5,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi1"),		/* D1 */
		SUNXI_FUNCTION(0x3, "spi2"),		/* CLK */
		SUNXI_FUNCTION(0x4, "uart5"),		/* RX */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT5 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PE6,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi1"),		/* D2 */
		SUNXI_FUNCTION(0x3, "spi2"),		/* MOSI */
		SUNXI_FUNCTION(0x4, "uart5"),		/* RTS */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT6 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PE7,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi1"),		/* D3 */
		SUNXI_FUNCTION(0x3, "spi2"),		/* MISO */
		SUNXI_FUNCTION(0x4, "uart5"),		/* CTS */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT7 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PE8,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi1"),		/* D4 */
		SUNXI_FUNCTION(0x3, "ts0"),		/* D0 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT8 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PE9,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi1"),		/* D5 */
		SUNXI_FUNCTION(0x3, "ts0"),		/* D1 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT9 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PE10,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi1"),		/* D6 */
		SUNXI_FUNCTION(0x3, "ts0"),		/* D2 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT10 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PE11,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi1"),		/* D7 */
		SUNXI_FUNCTION(0x3, "ts0"),		/* D3 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT11 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PE12,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi1"),		/* D8 */
		SUNXI_FUNCTION(0x3, "ts0"),		/* D4 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT12 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PE13,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi1"),		/* D9 */
		SUNXI_FUNCTION(0x3, "ts0"),		/* D5 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT13 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PE14,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi1"),		/* D10 */
		SUNXI_FUNCTION(0x3, "ts0"),		/* D6 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT14 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PE15,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi1"),		/* D11 */
		SUNXI_FUNCTION(0x3, "ts0"),		/* D7 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT15 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PE16,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi1"),		/* SCK */
		SUNXI_FUNCTION(0x3, "twi4"),		/* SCK */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT16 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PE17,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi1"),		/* SDA */
		SUNXI_FUNCTION(0x3, "twi4"),		/* SDA */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT17 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PE18,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi0"),		/* MCLK */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT18 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PE19,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi0"),		/* SCK */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT19 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PE20,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi0"),		/* SDA */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT20 */
		
	/* HOLE */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PF0,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc0"),		/* D1 */
		SUNXI_FUNCTION(0x4, "jtag0"),		/* MS */
		SUNXI_FUNCTION(0x5, "gpu")),		/* TMS */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PF1,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc0"),		/* D0 */
		SUNXI_FUNCTION(0x4, "jtag0"),		/* DI */
		SUNXI_FUNCTION(0x5, "gpu")),		/* TDI */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PF2,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc0"),		/* CLK */
		SUNXI_FUNCTION(0x4, "uart0")),		/* TX */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PF3,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc0"),		/* CMD */
		SUNXI_FUNCTION(0x4, "jtag0"),		/* DO */
		SUNXI_FUNCTION(0x5, "gpu")),		/* TDO */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PF4,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc0"),		/* D3 */
		SUNXI_FUNCTION(0x4, "uart0"),		/* RX */
		SUNXI_FUNCTION(0x5, "gpu")),		/* TRSTN */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PF5,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc0"),		/* D2 */
		SUNXI_FUNCTION(0x4, "jtag0"),		/* CK */
		SUNXI_FUNCTION(0x5, "gpu")),		/* TCK */
	
	/* HOLE */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PG0,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc1"),		/* CLK */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT0 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PG1,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc1"),		/* CMD */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT1 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PG2,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc1"),		/* D0 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT2 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PG3,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc1"),		/* D1 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT3 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PG4,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc1"),		/* D2 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT4 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PG5,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc1"),		/* D3 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT5 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PG6,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart2"),		/* TX */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT6 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PG7,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart2"),		/* RX */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT7 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PG8,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart2"),		/* RTS */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT8 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PG9,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart2"),		/* CTS */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT9 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PG10,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "twi3"),		/* SCK */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT10 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PG11,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "twi3"),		/* SDA */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT11 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PG12,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart4"),		/* TX */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT12 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PG13,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart4"),		/* RX */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT13 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PG14,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart4"),		/* RTS */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT14 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PG15,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart4"),		/* CTS */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT15 */

	/* HOLE */
#ifdef CONFIG_FPGA_V4_PLATFORM
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PH0,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out")),
#else
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PH0,
		SUNXI_FUNCTION(0x0,"gpio_in"),
		SUNXI_FUNCTION(0x1,"gpio_out"),
		SUNXI_FUNCTION(0x2, "twi0")),		/* SCK */
#endif
#ifdef CONFIG_FPGA_V4_PLATFORM
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PH1,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out")),
#else
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PH1,
		SUNXI_FUNCTION(0x0,"gpio_in"),
		SUNXI_FUNCTION(0x1,"gpio_out"),
		SUNXI_FUNCTION(0x2, "twi0")),		/* SDA */
#endif
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PH2,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "twi1")),		/* SCK */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PH3,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "twi1")),		/* SDA */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PH4,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "twi2")),		/* SCK */	
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PH5,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "twi2")),		/* SDA */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PH6,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "pwm0")),		
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PH7,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "cir"),		/* TX */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT0 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PH8,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "jtag0"),		/* MS0 */
		SUNXI_FUNCTION(0x3, "pwm1"),		/* P */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT1 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PH9,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "jtag0"),		/* CK0 */
		SUNXI_FUNCTION(0x3, "pwm1"),		/* N */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT2 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PH10,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "jtag0"),		/* DO0 */
		SUNXI_FUNCTION(0x3, "pwm2"),		/* P */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT3 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PH11,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "jtag0"),		/* DI0 */
		SUNXI_FUNCTION(0x3, "pwm2"),		/* n */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT4 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PH12,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart0"),		/* TX */
		SUNXI_FUNCTION(0x3, "spi3"),		/* CS2 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT5 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PH13,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart0"),		/* RX */
		SUNXI_FUNCTION(0x3, "spi3"),		/* CS3 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT6 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PH14,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "spi3"),		/* CLK */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT7 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PH15,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "spi3"),		/* MOSI */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT8 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PH16,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "spi3"),		/* MISO */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT9 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PH17,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "spi3"),		/* CS0 */
		SUNXI_FUNCTION(0x3, "spdif0"),		/* DIN */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT10 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PH18,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "spi3"),		/* CS1 */
		SUNXI_FUNCTION(0x3, "spdif0"),		/* DOUT */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT11 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PH19,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "hscl")),	
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PH20,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "hsda")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PH21,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "hcec")),			
	
	/* HOLE */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PJ0,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand1")),		/* WE */	
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PJ1,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand1")),		/* ALE */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PJ2,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand1")),		/* CLE */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PJ3,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand1")),		/* CE1 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PJ4,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand1")),		/* CE0 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PJ5,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand1")),		/* RE */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PJ6,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand1"),		/* RB0 */
		SUNXI_FUNCTION(0x3, "sdc3")),		/* CMD */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PJ7,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand1"),		/* RB1 */
		SUNXI_FUNCTION(0x3, "sdc3")),		/* CLK */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PJ8,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand1"),		/* DQ0 */
		SUNXI_FUNCTION(0x3, "sdc3"),		/* D0 */
		SUNXI_FUNCTION(0x4, "nand0")),		/* DQ8 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PJ9,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand1"),		/* DQ1 */
		SUNXI_FUNCTION(0x3, "sdc3"),		/* D1 */
		SUNXI_FUNCTION(0x4, "nand0")),		/* DQ9 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PJ10,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand1"),		/* DQ2 */
		SUNXI_FUNCTION(0x3, "sdc3"),		/* D2 */
		SUNXI_FUNCTION(0x4, "nand0")),		/* DQ10 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PJ11,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand1"),		/* DQ3 */
		SUNXI_FUNCTION(0x3, "sdc3"),		/* D3 */
		SUNXI_FUNCTION(0x4, "nand0")),		/* DQ11 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PJ12,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand1"),		/* DQ4 */
		SUNXI_FUNCTION(0x3, "sdc3"),		/* D4 */
		SUNXI_FUNCTION(0x4, "nand0")),		/* DQ12 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PJ13,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand1"),		/* DQ5 */
		SUNXI_FUNCTION(0x3, "sdc3"),		/* D5 */
		SUNXI_FUNCTION(0x4, "nand0")),		/* DQ13 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PJ14,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand1"),		/* DQ6 */
		SUNXI_FUNCTION(0x3, "sdc3"),		/* D6 */
		SUNXI_FUNCTION(0x4, "nand0")),		/* DQ14 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PJ15,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand1"),		/* DQ7 */
		SUNXI_FUNCTION(0x3, "sdc3"),		/* D7 */
		SUNXI_FUNCTION(0x4, "nand0")),		/* DQ15 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PJ16,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand1"),		/* DQS */
		SUNXI_FUNCTION(0x3, "sdc3")),		/* RST */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PJ17,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand1"),		/* CE2 */
		SUNXI_FUNCTION(0x3, "nand1")),		/* RE_B */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PJ18,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand1"),		/* CE3 */
		SUNXI_FUNCTION(0x3, "nand1")),		/* DQS_B */
		
	/* HOLE */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PL0,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x3, "s_uart0"),		/* TX */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT0 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PL1,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x3, "s_uart0"),		/* RX */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT1 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PL2,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x3, "s_jtag0"),		/* TMS */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT2 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PL3,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x3, "s_jtag0"),		/* TCK */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT3 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PL4,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x3, "s_jtag0"),		/* TD0 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT4 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PL5,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x3, "s_jtag0"),		/* TDI */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT5 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PL6,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x3, "s_cir0"),		/* RX */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT6 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PL7,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x3, "1wire0"),
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT7 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PL8,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x3, "s_ps2"),		/* SCK1 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT8 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PL9,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x3, "s_ps2"),		/* SDA1 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT9 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PL10,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x3, "s_ps2"),		/* SCK2 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT10 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PL11,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x3, "s_ps2"),		/* SDA2 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT11 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PL12,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT12 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PL13,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT13 */
		
	/* HOLE */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PM0,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x6, "eint")),	/* EINT0 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PM1,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x6, "eint")),	/* EINT1 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PM2,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x6, "eint")),	/* EINT2 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PM3,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x6, "eint")),	/* EINT3 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PM4,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x3, "s_i2s1"),		/* LRCKR */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT4 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PM5,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x3, "s_i2s1"),		/* DOUT1 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT5 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PM6,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x3, "s_i2s1"),		/* DOUT2 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT6 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PM7,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x3, "s_i2s1"),		/* DOUT3 */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT7 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PM8,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x3, "s_twi1"),		/* SCK */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT8 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PM9,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x3, "s_twi1"),		/* SDA */
		SUNXI_FUNCTION(0x6, "eint")),		/* EINT9 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PM10,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "s_i2s0"),		/* MCLK */
		SUNXI_FUNCTION(0x3, "s_i2s1")),		/* MCLK */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PM11,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "s_i2s0"),		/* BCLK */
		SUNXI_FUNCTION(0x3, "s_i2s1")),		/* BCLK */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PM12,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "s_i2s0"),		/* LRCK */
		SUNXI_FUNCTION(0x3, "s_i2s1")),		/* LRCKR */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PM13,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "s_i2s0"),		/* DIN */
		SUNXI_FUNCTION(0x3, "s_i2s1")),		/* DIN */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PM14,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "s_i2s0"),		/* DOUT */
		SUNXI_FUNCTION(0x3, "s_i2s1")),		/* DOUT0 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PM15,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x6, "eint")),	/* EINT10 */
		
	/* HOLE */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PN0,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "s_twi0"),		/* SCK */
		SUNXI_FUNCTION(0x3, "s_rsb0")),		/* SCK */
	SUNXI_PIN(SUNXI_PINCTRL_PIN_PN1,
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "s_twi0"),		/* SDA */
		SUNXI_FUNCTION(0x3, "s_rsb0")),		/* SDA */
};

static struct sunxi_pin_bank sun9i_w1_banks[] = {
	SUNXI_PIN_BANK(SUNXI_PIO_VBASE,   SUNXI_PA_BASE, 18, SUNXI_EINT_TYPE_GPIO, "PA", SUNXI_IRQ_EINTA),
	SUNXI_PIN_BANK(SUNXI_PIO_VBASE,   SUNXI_PB_BASE, 20, SUNXI_EINT_TYPE_GPIO, "PB", SUNXI_IRQ_EINTB),
	SUNXI_PIN_BANK(SUNXI_PIO_VBASE,   SUNXI_PC_BASE, 20, SUNXI_EINT_TYPE_NONE, "PC", SUNXI_IRQ_MAX  ),
	SUNXI_PIN_BANK(SUNXI_PIO_VBASE,   SUNXI_PD_BASE, 28, SUNXI_EINT_TYPE_NONE, "PD", SUNXI_IRQ_MAX  ),
	SUNXI_PIN_BANK(SUNXI_PIO_VBASE,   SUNXI_PE_BASE, 21, SUNXI_EINT_TYPE_GPIO, "PE", SUNXI_IRQ_EINTE),
	SUNXI_PIN_BANK(SUNXI_PIO_VBASE,   SUNXI_PF_BASE,  6, SUNXI_EINT_TYPE_NONE, "PF", SUNXI_IRQ_MAX  ),
	SUNXI_PIN_BANK(SUNXI_PIO_VBASE,   SUNXI_PG_BASE, 16, SUNXI_EINT_TYPE_GPIO, "PG", SUNXI_IRQ_EINTG),
	SUNXI_PIN_BANK(SUNXI_PIO_VBASE,   SUNXI_PH_BASE, 22, SUNXI_EINT_TYPE_GPIO, "PH", SUNXI_IRQ_EINTH),
	SUNXI_PIN_BANK(SUNXI_PIO_VBASE,   SUNXI_PJ_BASE, 19, SUNXI_EINT_TYPE_NONE, "PJ", SUNXI_IRQ_MAX  ),
	SUNXI_PIN_BANK(SUNXI_R_PIO_VBASE, SUNXI_PL_BASE, 14, SUNXI_EINT_TYPE_GPIO, "PL", SUNXI_IRQ_R_EINTL),
	SUNXI_PIN_BANK(SUNXI_R_PIO_VBASE, SUNXI_PM_BASE, 16, SUNXI_EINT_TYPE_GPIO, "PM", SUNXI_IRQ_R_EINTM),
	SUNXI_PIN_BANK(SUNXI_R_PIO_VBASE, SUNXI_PN_BASE,  2, SUNXI_EINT_TYPE_NONE, "PN", SUNXI_IRQ_MAX	),
};

struct sunxi_pinctrl_desc sun9i_w1_pinctrl_data = {
	.pins   = sun9i_w1p1_pins,
	.npins  = ARRAY_SIZE(sun9i_w1p1_pins),
	.banks  = sun9i_w1_banks,
	.nbanks = ARRAY_SIZE(sun9i_w1_banks),
};
