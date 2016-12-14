#ifndef TWI_H_INCLUDED
#define TWI_H_INCLUDED

#include "Arduino.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/// Variáveis do TWI adicionadas

#define TWI_BUFFER_SIZE 32
#define TWI_BIT_RATE    152

#define TWI_DEBUG       1

/// Classe de TWI

class TWI{
    
public:
    
    static void begin();
    static void begin(uint8_t SLA);
    static void onReceive(void (*function)(char*));
    static void onSend(void (*function)(void));
    static void onSendError(void (*function)(void));
    static void turn_pullUp();
    static void set_rate();
    static bool busy();
    static int set_SLA(uint8_t SLA);
    static void set_globalRespond();
    static void set_slaveR();
    static int send_msg(uint8_t SLA, char *msg, unsigned int msg_length);
    static void send_start();
    
    static void Interrupt_ISR();
    
private:

    // Variável que indica se estamos em DEBUG do TWI ou não

    static bool DEBUG;
    
    // Variàveis a serem usadas durante a execução
    
    static volatile unsigned char twi_buf[TWI_BUFFER_SIZE];
    static volatile unsigned int twi_msg_size;
    static volatile unsigned int twi_ptr;
    
    /* Significados do twi_status
     ** twi_status = 0 -> não contém informação, o TWI está à espera
     *   de ação
     ** twi_status = 1 -> há dados no buffer para enviar, foi emitido
     *   um start mas ainda não foi enviado o SLA+W.
     ** twi_status = 2 -> está-se a enviar dados
     ** twi_status = 3 -> está-se a receber dados
     */
    
    static volatile unsigned int twi_status;

    //Funções pré callback
    static void data_received();
    static void data_sent();
    static void data_errorSend();
    
    // Função de callback
    static void (*master_onSend)(void);
    static void (*master_onSendError)(void);
    static void (*slave_onReceive)(char*);
    
};

/// TWI State codes

/// General TWI Master status codes

#define TWI_START                  0x08  ///< START has been transmitted
#define TWI_REP_START              0x10  ///< Repeated START has been transmitted
#define TWI_ARB_LOST               0x38  ///< Arbitration lost

///
/// TWI Master Transmitter status codes
///

#define TWI_MTX_ADR_ACK            0x18  ///< SLA+W has been transmitted and ACK received
#define TWI_MTX_ADR_NACK           0x20  ///< SLA+W has been transmitted and NACK received
#define TWI_MTX_DATA_ACK           0x28  ///< Data byte has been transmitted and ACK received
#define TWI_MTX_DATA_NACK          0x30  ///< Data byte has been transmitted and NACK received

///
/// TWI Master Receiver status codes
///

#define TWI_MRX_ADR_ACK            0x40  ///< SLA+R has been transmitted and ACK received
#define TWI_MRX_ADR_NACK           0x48  ///< SLA+R has been transmitted and NACK received
#define TWI_MRX_DATA_ACK           0x50  ///< Data byte has been received and ACK transmitted
#define TWI_MRX_DATA_NACK          0x58  ///< Data byte has been received and NACK transmitted

///
/// TWI Slave Transmitter status codes
///

#define TWI_STX_ADR_ACK            0xA8  ///< Own SLA+R has been received; ACK has been returned
#define TWI_STX_ADR_ACK_M_ARB_LOST 0xB0  ///< Arbitration lost in SLA+R/W as Master;
///< own SLA+R has been received; ACK has been returned
#define TWI_STX_DATA_ACK           0xB8  ///< Data byte in TWDR has been transmitted; ACK has been received
#define TWI_STX_DATA_NACK          0xC0  ///< Data byte in TWDR has been transmitted; NOT ACK has been received
#define TWI_STX_DATA_ACK_LAST_BYTE 0xC8  ///< Last data byte in TWDR has been transmitted (TWEA = 0);
///<ACK has been received
///
/// TWI Slave Receiver status codes
///

#define TWI_SRX_ADR_ACK            0x60  ///< Own SLA+W has been received ACK has been returned
#define TWI_SRX_ADR_ACK_M_ARB_LOST 0x68  ///< Arbitration lost in SLA+R/W as Master; own SLA+W has been received;
///< ACK has been returned
#define TWI_SRX_GEN_ACK            0x70  ///< General call address has been received; ACK has been returned
#define TWI_SRX_GEN_ACK_M_ARB_LOST 0x78  ///< Arbitration lost in SLA+R/W as Master;
///< General call address has been received;
///< ACK has been returned
#define TWI_SRX_ADR_DATA_ACK       0x80  ///< Previously addressed with own SLA+W; data has been received;
///< ACK has been returned
#define TWI_SRX_ADR_DATA_NACK      0x88  ///< Previously addressed with own SLA+W; data has been received;
///< NOT ACK has been returned
#define TWI_SRX_GEN_DATA_ACK       0x90  ///< Previously addressed with general call; data has been received;
///< ACK has been returned
#define TWI_SRX_GEN_DATA_NACK      0x98  ///< Previously addressed with general call; data has been received;
///< NOT ACK has been returned
#define TWI_SRX_STOP_RESTART       0xA0  ///< A STOP condition or repeated START condition has been received
///< while still addressed as Slave
///
/// TWI Miscellaneous status codes
///

#define TWI_NO_STATE               0xF8  ///< No relevant state information available; TWINT = 0
#define TWI_BUS_ERROR              0x00  ///< Bus error due to an illegal START or STOP condition


#endif

