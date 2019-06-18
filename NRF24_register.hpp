
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