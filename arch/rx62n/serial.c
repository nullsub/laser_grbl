/*
   serial.c - Low level functions for sending and recieving bytes via the serial port
   Part of Grbl

   Copyright (c) 2009-2011 Simen Svale Skogsrud
   Copyright (c) 2011 Sungeun K. Jeon

   Grbl is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Grbl is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Grbl.  If not, see <http://www.gnu.org/licenses/>.
   */

/* This code was initially inspired by the wiring_serial module by David A. Mellis which
   used to be a part of the Arduino project. */ 

#include "config.h"
#include "dev_misc.h"
#include "stdint.h"
#include "shell.h" 
#include "serial.h" 

#include <iodefine.h>
#include <board.h>

#include "FreeRTOS.h"
#include "task.h"
//#include "semphr.h"

#define RX_BUFFER_SIZE 2048
#define TX_BUFFER_SIZE 128
char tx_buffer[RX_BUFFER_SIZE];
int tx_bffr_i = 0x00;

void serial_init(long baud)
{
	// already initialised before the start of the FreeRTOS scheduler
}

void serial_write(uint8_t data) {	
#if 0 // dont use the shell
	if(tx_bffr_i > TX_BUFFER_SIZE-2) {
		shell_send_str("buffer to small!\n");
	}

	tx_buffer[tx_bffr_i] = data;
	tx_bffr_i++;
	if(data =='\n' ) {
		if(tx_bffr_i > 0) {
			tx_buffer[tx_bffr_i-1] = 0x00;
			shell_send_str((char*)tx_buffer);	
			tx_bffr_i = 0;
		}
	}
#endif
	sci2_putchar(data);
}

uint8_t rx_buffer[RX_BUFFER_SIZE];
uint8_t rx_buffer_head = 0;
uint8_t rx_buffer_tail = 0;

//xSemaphoreHandle serial_mutex;

uint8_t serial_read()
{       
#if 0 
	if(IR(SCI2, RXI2) == 1) { // polling
		/* Read character */
		uint8_t ret = SCI2.RDR;

		/* Clear flag to receive next byte */
		IR(SCI2, RXI2) = 0;
		return ret;
	} 
	return SERIAL_NO_DATA;
#endif

	if (rx_buffer_head == rx_buffer_tail) { // via buffer
		return SERIAL_NO_DATA;
	} else {
		//vSemaphoreMutexTake(serial_mutex, portMAX_DELAY);	
		uint8_t data = rx_buffer[rx_buffer_tail];
		if (rx_buffer_tail == RX_BUFFER_SIZE)  
			rx_buffer_tail = 0; 
		else
			rx_buffer_tail++;
		//vSemaphoreMutexGive(serial_mutex);	
		return data;
	}
}




void serial_receive_c(char c)
{
	uint8_t data = c;
	uint8_t next_head = rx_buffer_head + 1;
	if (next_head == RX_BUFFER_SIZE) { next_head = 0; }

	// Write data to buffer unless it is full.
	if (next_head != rx_buffer_tail) {
		rx_buffer[rx_buffer_head] = data;
		rx_buffer_head = next_head;
	}
}

void serial_receive(char *str) 
{
	while(*str) serial_receive_c(*str++); // add check whether the buffer is full.
	if(*(str-1) != '\n')
		serial_receive_c('\n');
}

void serial_poll_task(void *ptr)
{
	//vSemaphoreCreateMutex(&serial_mutex);
	while(1) {
		if(IR(SCI2, RXI2) == 1){
			char c = SCI2.RDR; 
	//		vSemaphoreMutexTake(serial_mutex, portMAX_DELAY);	
			serial_receive_c(c);
	//		vSemaphoreMutexGive(serial_mutex);	
			IR(SCI2, RXI2) = 0; // clear flag
		}
		taskYIELD();
	}
}
