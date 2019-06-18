
//commando's
#define R_REGISTER           0x00        //Leest commando en status registers, 5 bit register map address
#define W_REGISTER           0x20        //Schrijft commando en status registers, 5 bit register map address. Uitvoerbaar in power down
#define R_RX_PAYLOAD         0x61        //Leest de RX-payload: 1 - 32 bytes
#define W_TX_PAYLOAD         0xA0        //Schrijft TX-payload 1 - 32 bytes
#define FLUSH_TX             0xE1        //Flush TX FIFO, in TX mode
#define FLUSH_RX             0xE2        //Flush RX FIFO, in RX mode
#define REUSE_TX_PL          0xE3        //Wordt gebruikt voor PTX apparaten, hergebruik laatst verstuurde payload
#define R_RX_PL_WID          0x60        //Leest RX payload wijdte
#define W_ACK_PAYLOAD        0xA8        //Schrijft payload om met ACK packet te worden verstuurd
#define W_TX_PAYLOAD_NO_ACK  0xB0        //Zet AUTOACK uit voor een specifiek packet
#define NOP                  0xFF        //Geen operatie, kan gebruikt worden om het STATUS register te lezen

//addresses
#define CONFIG               0x00        //Configuration register
#define EN_AA                0x01        //Enable 'Auto Acknowledgment' function
#define EN_RXADDR            0x02        //Enabled RX Addresses
#define SETUP_AW             0x03        //Setup of address widths
#define SETUP_RETR           0x04        //Setup of automatic retransmission
#define RF_CH                0x05        //RF channel (sets the frequency channel where nRF is operation on)
#define RF_SETUP             0x06        //RF setup register
#define STATUS               0x07        //Status register
#define OBSERVE_TX           0x08        //Transmit observe register
#define RPD                  0x09        //no information available
#define RX_ADDR_P0           0x0A        //Receive address data pipe 0, maximum 5 bytes length
#define RX_ADDR_P1           0x0B        //Receive address data pipe 1, maximum 5 bytes length
#define RX_ADDR_P2           0x0C        //Receive address data pipe 2, maximum 5 bytes length
#define RX_ADDR_P3           0x0D        //Receive address data pipe 3, maximum 5 bytes length
#define RX_ADDR_P4           0x0E        //Receive address data pipe 4, maximum 5 bytes length
#define RX_ADDR_P5           0x0F        //Receive address data pipe 5, maximum 5 bytes length
#define TX_ADDR              0x10        //Transmit address. Used for a PTX device only 
#define RX_PW_P0             0x11        
#define RX_PW_P1             0x12           
#define RX_PW_P2             0x13
#define RX_PW_P3             0x14
#define RX_PW_P4             0x15
#define RX_PW_P5             0x16
#define FIFO_STATUS          0x17        //FIFO status register
#define DYNPD                0x1C        //Enable dynamic payload length
#define FEATURE              0x1D        //Feature register

//mnemonic
#define
