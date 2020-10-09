/**
 * @file protocol.c
 * @author Stanislav Karpikov
 * @brief Process received messages from the panel
 */

/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "protocol.h"

#include <string.h>

#include "BSP/bsp.h"
#include "BSP/uart.h"
#include "command_processor.h"
#include "crc.h"
#include "stm32f7xx_hal.h"

/*--------------------------------------------------------------
                       DEFINES
--------------------------------------------------------------*/

#define PROTOCOL_START_BYTE ((uint8_t)0x55)
#define PROTOCOL_STOP_BYTE  ((uint8_t)0x77)
#define PROTOCOL_STATUS_MAX (31)  // 2**5bits - 1 = 31

#define PROTOCOL_IGNORE_CRC             (0)
#define PROTOCOL_IGNORE_UNKNOWN_COMMAND (1)

/*--------------------------------------------------------------
                       PRIVATE DATA
--------------------------------------------------------------*/

/** Internal protocol context */
static protocol_context_t protocol;

/*--------------------------------------------------------------
                       PRIVATE FUNCTIONS
--------------------------------------------------------------*/

/**
 * @brief Send a packet to the interface adapter
 * 
 * @param data Pointer to the data block
 * @param len  Data block size
 */
static void adapter_send_packet(uint8_t *data, uint32_t len)
{
    uart_interface_transmit(data, len);
}

/**
 * @brief Read a byte from the interface adapter
 * 
 * @param[out] byte Pointer to the data byte to read
 * @return Status of the operation
 */
static status_t adapter_get_byte(uint8_t *byte)
{
    return uart_interface_get_byte(byte);
}

/**
 * @brief Initialize interface adapter
 * 
 * @return Status of the operation
 */
static status_t adapter_rx_init(void)
{
    return uart_interface_rx_init();
}

/**
 * @brief Reset the protocol context
 * 
 * @param pc The protocol context structure
 */
static void protocol_reset(protocol_context_t *pc)
{
    memset(&pc->packet_received, 0, sizeof(packet_t));
    memset(&pc->packet_to_send, 0, sizeof(packet_t));

    pc->stage = PROTOCOL_START;
    pc->size = 0;
    pc->data_pointer = pc->packet_received.data;
}
/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

/*
 * @brief Initialize potocol interface
 * 
 * @return Status of the operation
 */
status_t protocol_hw_init(void)
{
    status_t status;

    status = command_handlers_init();
    if (status != PFC_SUCCESS) return status;

    status = adapter_rx_init();

    return status;
}

/*
 * @brief Initialize potocol interface with handlers
 * 
 * @param handlers Pointer to handlers array
 * @param handlers_count Handlers array size
 * 
 * @return Status of the operation
 */
status_t protocol_init(PFC_COMMAND_CALLBACK *handlers,
                       uint8_t handlers_count)
{
    protocol.handlers = handlers;
    protocol.handlers_count = handlers_count;

    return PFC_SUCCESS;
}

/*
 * @brief Send a packet to the panel
 * 
 * @param pc Protocol context structure
 * 
 * @return Status of the operation
 */
status_t protocol_send_packet(protocol_context_t *pc)
{
    pc->stage = PROTOCOL_START;
    pc->packet_to_send.fields.start = PROTOCOL_START_BYTE;
    uint16_t crcsend = crc16(pc->packet_to_send.data + 1,
                             pc->packet_to_send.fields.len);

    pc->packet_to_send.data[pc->packet_to_send.fields.len + 1] = crcsend & 0xFF;
    pc->packet_to_send.data[pc->packet_to_send.fields.len + 2] = crcsend >> 8;
    pc->packet_to_send.data[pc->packet_to_send.fields.len + 3] = PROTOCOL_STOP_BYTE;
    adapter_send_packet(pc->packet_to_send.data, pc->packet_to_send.fields.len + MINIMUM_PACKET_LENGTH);
    return PFC_SUCCESS;
}

/*
 * @brief Send an error packet to the panel
 * 
 * @param pc The protocol context structure
 * @param command The received command with error
 * 
 * @return The status of the operation
 */
void protocol_error_handle(protocol_context_t *pc, uint8_t command)
{
    packet_t *out;
    out = &pc->packet_to_send;
    packet_clear_status(out);
    packet_set_error(out, 1);
    packet_set_command(out, command);
    packet_set_data_len(out, 0);
    protocol_send_packet(pc);
}

/*
 * @brief Unknown command handler
 * 
 * @param pc The protocol context structure
 * 
 * @return The status of the operation
 */
void protocol_unknown_command_handle(protocol_context_t *pc)
{
    protocol_error_handle(pc, packet_get_command(&pc->packet_received));
}

/*
 * @brief Process messages from the panel. Called in a cycle
 * 
 * @return The status of the operation
 */
status_t protocol_work(void)
{
    uint8_t byte;
    status_t receive_status = PFC_NULL;
    while ((receive_status = adapter_get_byte(&byte)) == PFC_SUCCESS)
    {
        switch (protocol.stage)
        {
            case PROTOCOL_START:
                if (byte == PROTOCOL_START_BYTE)
                {
                    protocol.stage = PROTOCOL_STATUS;
                }
                break;
            case PROTOCOL_STATUS:
                if (byte > PROTOCOL_STATUS_MAX)
                {
                    protocol.stage = PROTOCOL_START;
                }
                else
                {
                    protocol.stage = PROTOCOL_LEN;
                    protocol.packet_received.fields.status.raw = byte;
                }
                break;
            case PROTOCOL_LEN:
                protocol.stage = PROTOCOL_COMMAND;
                protocol.packet_received.fields.len = byte;
                if (byte < MINIMUM_PACKET_LENGTH)
                {
                    protocol.stage = PROTOCOL_START;
                }
                else
                {
                    if (byte > (MAXIMUM_PACKET_LENGTH - 4))
                    {
                        protocol.stage = PROTOCOL_START;
                    }
                    protocol.size = byte;
                    protocol.data_pointer = protocol.packet_received.data + MINIMUM_PACKET_LENGTH;
                }
                break;
            case PROTOCOL_COMMAND:
                protocol.packet_received.fields.command = byte;
                if (protocol.packet_received.fields.len == MINIMUM_PACKET_LENGTH)
                {
                    protocol.stage = PROTOCOL_CRC;
                }
                else
                {
                    protocol.stage = PROTOCOL_DATA;
                }
                protocol.size--;
                break;
            case PROTOCOL_DATA:
                *(protocol.data_pointer) = byte;
                protocol.size--;
                protocol.data_pointer++;
                if (protocol.size > (MAXIMUM_PACKET_LENGTH - 4))
                {
                    protocol.stage = PROTOCOL_START;
                }
                if (protocol.size <= MINIMUM_PACKET_LENGTH - 1)
                {
                    protocol.stage = PROTOCOL_CRC;
                }
                break;
            case PROTOCOL_CRC:
                protocol.size--;
                *(protocol.data_pointer) = byte;
                protocol.data_pointer++;
                if (protocol.size == 1)
                {
                    protocol.stage = PROTOCOL_STOP;
                }
                break;
            case PROTOCOL_STOP:
                protocol.size--;
                if (byte == PROTOCOL_STOP_BYTE)
                {
                    *(protocol.data_pointer) = byte;
                    protocol.data_pointer++;

                    uint16_t crcget = packet_get_crc(&protocol.packet_received);
                    uint16_t crccalc = packet_calculate_crc(&protocol.packet_received);
                    if (crcget != crccalc)
                    {
#if PROTOCOL_IGNORE_CRC == 0
                        packet_clear_status(&protocol.packet_to_send);
                        protocol.packet_to_send.fields.status.fields.crc_error = 1;
                        packet_set_data_len(&protocol.packet_to_send, 0);
                        protocol_send_packet(&protocol);
#endif /* PROTOCOL_IGNORE_CRC */
                        return PFC_WARNING;
                    }

#if PROTOCOL_IGNORE_CRC == 0
                    else if (protocol.packet_received.fields.status.fields.crc_error)
                    {
                        protocol_send_packet(&protocol);
                        return PFC_WARNING;
                    }
                    else
                    {
                        protocol.packet_received.fields.status.fields.crc_error = 0;
                    }
#endif /* PROTOCOL_IGNORE_CRC */

                    if (protocol.packet_received.fields.command >= protocol.handlers_count)
                    {
#if PROTOCOL_IGNORE_UNKNOWN_COMMAND == 1
#else /* PROTOCOL_IGNORE_UNKNOWN_COMMAND */
                        protocol_unknown_command_handler(&protocol);
#endif /* PROTOCOL_IGNORE_UNKNOWN_COMMAND */
                    }
                    else
                    {
                        PFC_COMMAND_CALLBACK handler = protocol.handlers[protocol.packet_received.fields.command];
                        if (handler) handler(&protocol);
                    }
                }
                else
                {
                    //protocol.packet_received.fields.status.fields.unexpected_stop = 1;
                }
                protocol.stage = PROTOCOL_START;
                return PFC_SUCCESS;
            default:
                protocol.stage = PROTOCOL_START;
                break;
        }
    }
    if (receive_status == PFC_WARNING)
    {
        protocol_reset(&protocol);
        return PFC_WARNING;
    }
    else
    {
        return PFC_SUCCESS;
    }
}

/*
 * @brief Write data len into a packet
 *
 * @param packet A pointer to the packet structure
 * @param len The length to write
 */
void packet_set_data_len(packet_t *packet, uint32_t len)
{
    packet->fields.len = MINIMUM_PACKET_LENGTH + len;
}

/*
 * @brief Extract a command value from a packet
 *
 * @param packet A pointer to the packet structure
 *
 * @return The command value
 */
uint8_t packet_get_command(packet_t *packet)
{
    return packet->fields.command;
}

/*
 * @brief Extract a CRC value from a packet
 *
 * @param packet A pointer to the packet structure
 *
 * @return The CRC value
 */
uint16_t packet_get_crc(packet_t *packet)
{
    return (packet->data[packet->fields.len] | (packet->data[packet->fields.len + 1] << 8));
}

/*
 * @brief Return the CRC value for a packet
 *
 * @param packet A pointer to the packet structure
 *
 * @return The CRC value
 */
uint16_t packet_calculate_crc(packet_t *packet)
{
    return crc16(packet->data + 1, packet->fields.len - 1);
}

/*
 * @brief Clear the status in a packet
 *
 * @param packet A pointer to the packet structure
 */
void packet_clear_status(packet_t *packet)
{
    packet->fields.status.raw = 0;
}

/*
 * @brief Set the error field in a packet
 *
 * @param error The error value
 * @param packet A pointer to the packet structure
 */
void packet_set_error(packet_t *packet, uint8_t error)
{
    packet->fields.status.fields.error = error;
}

/*
 * @brief Set the command field in a packet
 *
 * @param error The command value
 * @param packet A pointer to the packet structure
 */
void packet_set_command(packet_t *packet, uint8_t command)
{
    packet->fields.command = command;
}
