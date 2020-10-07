/**
 * @file protocol.c
 * @author Stanislav Karpikov
 * @brief Process received messages from the panel
 */
 
/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "protocol.h"
#include "command_processor.h"
#include <string.h>

#include "BSP/bsp.h"
#include "BSP/uart.h"
#include "crc.h"
#include "stm32f7xx_hal.h"

/*--------------------------------------------------------------
                       DEFINES
--------------------------------------------------------------*/

#define PROTOCOL_START_BYTE ((uint8_t)0xAA)
#define PROTOCOL_STOP_BYTE  ((uint8_t)0xBB)
#define PROTOCOL_STATUS_MAX (31)  // 2**5bits - 1 = 31

#define PROTOCOL_CRC_IGNORE             (0)
#define PROTOCOL_UNKNOWN_COMMAND_IGNORE (1)

/*--------------------------------------------------------------
                       PRIVATE DATA
--------------------------------------------------------------*/

static protocol_context_t protocol;
extern UART_HandleTypeDef huart1;
static SciPort port;

/*--------------------------------------------------------------
                       PRIVATE FUNCTIONS
--------------------------------------------------------------*/

static void send_packet(uint8_t *data, uint32_t len)
{
	uart_interface_transmit(data, len);
}

static int get_byte(SciPort *port, unsigned char *b)
{
    if (b==0) return -2;
    /*
    if (port->rx_overflow) {
        port->rx_overflow = 0;
        port->rx_readed = port->rx_index;
        return -1;
    }

    if (port->rx_readed == port->rx_index)
        return 0;

    *b = port->rx_buffer[port->rx_readed];
    port->rx_readed++;
    if (port->rx_readed == RX_BUFFER_SIZE)
        port->rx_readed = 0;
    return 1;*/
    if (port->rx_buffer[port->rx_readed] != 0xFFFF)
    {
        *b = port->rx_buffer[port->rx_readed] & 0xFF;
        port->rx_buffer[port->rx_readed] = 0xFFFF;
        port->rx_readed++;
        if (port->rx_readed == RX_BUFFER_SIZE)
        {
            port->rx_readed = 0;
        }
        return 1;
    }
    return 0;
}

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    HAL_UART_DMAStop(huart);
    HAL_UART_DeInit(huart);
    HAL_UART_MspDeInit(huart);
    uart_init();
    memset(port.rx_buffer, 0xFF, sizeof(port.rx_buffer));
    HAL_GPIO_WritePin(RE_485_GPIO_Port, RE_485_Pin, GPIO_PIN_RESET);
    HAL_UART_Receive_DMA(huart, (uint8_t *)port.rx_buffer, RX_BUFFER_SIZE);
    port.rx_readed = 0;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    HAL_GPIO_WritePin(RE_485_GPIO_Port, RE_485_Pin, GPIO_PIN_RESET);
}

status_t protocol_hw_init(void)
{
	prothandlers_init(&port);
	memset(port.rx_buffer, 0xFF, sizeof(port.rx_buffer));
	HAL_GPIO_WritePin(RE_485_GPIO_Port, RE_485_Pin, GPIO_PIN_RESET);
	HAL_UART_Receive_DMA(&huart1, (uint8_t *)port.rx_buffer, RX_BUFFER_SIZE);
	port.rx_readed = 0;
	return PFC_SUCCESS;
}

void protocol_init(protocol_mode_t mode,
                   PFC_COMMAND_CALLBACK *handlers,
                   unsigned char handlersLen,
                   SciPort *_port)
{
    protocol.mode = mode;

    protocol.handlers = handlers;
    protocol.handlersLen = handlersLen;

    protocol.port = _port;
}

void protocol_send_package(protocol_context_t *pc)
{
    pc->state = P_START;
    pc->packageToSend.fields.start = PROTOCOL_START_BYTE;
    pc->packageToSend.fields.status.fields.owner = pc->mode;
    uint16_t crcsend = crc16(pc->packageToSend.data + 1,
                             pc->packageToSend.fields.len);

    pc->packageToSend.data[pc->packageToSend.fields.len + 1] = crcsend & 0xFF;
    pc->packageToSend.data[pc->packageToSend.fields.len + 2] = crcsend >> 8;
    pc->packageToSend.data[pc->packageToSend.fields.len + 3] = PROTOCOL_STOP_BYTE;
    send_packet(pc->packageToSend.data, pc->packageToSend.fields.len + MIN_PACKET_LEN);
}

void protocol_error_handle(protocol_context_t *pc, unsigned char command)
{
    PACKAGE *out;
    out = &pc->packageToSend;
    package_clear_status(out);
    package_set_error(out, 1);
    package_set_command(out, command);
    package_set_data_len(out, 0);
    protocol_send_package(pc);
}

void protocol_unknown_command_handle(protocol_context_t *pc)
{
    protocol_error_handle(pc, package_get_command(&pc->receivedPackage));
}

void protocol_reset(protocol_context_t *pc)
{
    memset(&pc->receivedPackage, 0, sizeof(PACKAGE));
    memset(&pc->packageToSend, 0, sizeof(PACKAGE));

    pc->state = P_START;
    pc->size = 0;
    pc->pdata = pc->receivedPackage.data;
}


int protocol_work(void)
{
		protocol_context_t *pc = &protocol;
    unsigned char b;
    int prot_r = 0;
    while ((prot_r = get_byte(pc->port, &b)) > 0)
    {
        switch (pc->state)
        {
            case P_START:
                if (b == PROTOCOL_START_BYTE)
                {
                    pc->state = P_STATUS;
                }
                break;
            case P_STATUS:
                if (b > PROTOCOL_STATUS_MAX)
                {
                    pc->state = P_START;
                }
                else
                {
                    pc->state = P_LEN;
                    pc->receivedPackage.fields.status.raw = b;
                }
                break;
            case P_LEN:
                pc->state = P_COMMAND;
                pc->receivedPackage.fields.len = b;
                if (b < MIN_PACKET_LEN)
                {
                    pc->state = P_START;
                }
                else
                {
                    if (b > (PROTOCOL_MAX_LENGTH - 4))
                    {
                        pc->state = P_START;
                    }
                    pc->size = b;
                    pc->pdata = pc->receivedPackage.data + MIN_PACKET_LEN;
                }
                break;
            case P_COMMAND:
                pc->receivedPackage.fields.command = b;
                if (pc->receivedPackage.fields.len == MIN_PACKET_LEN)
                {
                    pc->state = P_CRC;
                }
                else
                {
                    pc->state = P_DATA;
                }
                pc->size--;
                break;
            case P_DATA:
                *(pc->pdata) = b;
                pc->size--;
                pc->pdata++;
                if (pc->size > (PROTOCOL_MAX_LENGTH - 4))
                {
                    pc->state = P_START;
                }
                if (pc->size <= MIN_PACKET_LEN - 1)
                {
                    pc->state = P_CRC;
                }
                break;
            case P_CRC:
                pc->size--;
                *(pc->pdata) = b;
                pc->pdata++;
                if (pc->size == 1)
                {
                    pc->state = P_STOP;
                }
                break;
            case P_STOP:
                pc->size--;
                if (b == PROTOCOL_STOP_BYTE)
                {
                    *(pc->pdata) = b;
                    pc->pdata++;

                    pc->status.fields.connectionOk = 1;

                    /// if crc error
                    uint16_t crcget = package_get_crc(&pc->receivedPackage);
                    uint16_t crccalc = package_calculate_crc(&pc->receivedPackage);
                    if (crcget != crccalc)
                    {
#if PROTOCOL_CRC_IGNORE == 0
                        package_clear_status(&pc->packageToSend);
                        pc->packageToSend.fields.status.fields.crc_error = 1;
                        package_set_data_len(&pc->packageToSend, 0);
                        protocol_send_package(pc);
#endif /* PROTOCOL_CRC_IGNORE */
                        return 1;
                    }

#if PROTOCOL_CRC_IGNORE == 0
                    else if (pc->receivedPackage.fields.status.fields.crc_error)
                    {
                        protocol_send_package(pc);  //!< send last package again
                        return 1;
                    }
                    else
                    {
                        pc->receivedPackage.fields.status.fields.crc_error = 0;
                    }
#endif /* PROTOCOL_CRC_IGNORE */

                    if (pc->receivedPackage.fields.command >= pc->handlersLen)
                    {
#if PROTOCOL_UNKNOWN_COMMAND_IGNORE == 1
#else /* PROTOCOL_UNKNOWN_COMMAND_IGNORE */
                        protocol_unknown_command_handler(pc);
#endif /* PROTOCOL_UNKNOWN_COMMAND_IGNORE */
                    }
                    else
                    {
                        if (pc->handlers[pc->receivedPackage.fields.command])
                            pc->handlers[pc->receivedPackage.fields.command](pc);
                    }
                    ///
                    ///    --------- PROCESS PACKAGE --------- END      ------------------------------------------
                    ///    ----------------------------------------- ------------------------------------------
                    ///    -----------------------------------------  -----------------------------------------
                }
                else
                {
                    //pc->receivedPackage.fields.status.fields.wrong_stop = 1;
                }
                pc->state = P_START;
                return 1;
            default:
                pc->state = P_START;
                break;
        }
    }
    if (prot_r < 0)
    {
        protocol_reset(pc);
        return 1;
    }
    else
    {
        return 0;
    }
}

void package_set_data_len(PACKAGE *pac, uint32_t l)
{
	pac->fields.len = MIN_PACKET_LEN + l;
}

uint8_t package_get_command(PACKAGE *pac)
{
	return pac->fields.command;
}

uint16_t package_get_crc(PACKAGE *pac)
{
	return ((pac)->data[(pac)->fields.len] | ((pac)->data[(pac)->fields.len + 1] << 8));
}

uint16_t package_calculate_crc(PACKAGE *pac)
{
	return crc16((pac)->data + 1, (pac)->fields.len - 1);
}

void package_clear_status(PACKAGE *pac)
{
	(pac)->fields.status.raw = 0;
}

void package_set_error(PACKAGE *pac, uint8_t err)
{
	(pac)->fields.status.fields.error = err;
}

void package_set_command(PACKAGE *pac, uint8_t comm)
{
	(pac)->fields.command = comm;
}
