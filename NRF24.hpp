#ifndef NRF24_HPP
#define NRF24_HPP

#include "hwlib.hpp"

// this file contains Doxygen lines
/// @file

/// \brief
/// object of nrf24l01+
/// \details
/// This object can be used to transmit and receive data over the nrf24l01+ chip
class NRF24 {
protected:
	hwlib::spi_bus & bus; 
	hwlib::pin_out & ce;
	hwlib::pin_out & csn;
	uint8_t payload_size;
	uint8_t addr_width;

public:
    
    /// \brief
    /// default constructor
    /// \details
    /// This constructor does initialize the default attributes and automatically
    /// gives a value to the payload_size(5) and to the addr_width(5).
    NRF24( hwlib::spi_bus & bus, hwlib::pin_out & ce, hwlib::pin_out & csn);

    /// \brief
    /// full constructor
    /// \details
    /// This constructor lets you initialize all the values instead of using the default values
    NRF24( hwlib::spi_bus & bus, hwlib::pin_out & ce, hwlib::pin_out & csn, uint8_t payload_size, uint8_t addr_width );

    /// \brief
    /// transfers one thing
    /// \details
    /// transfers only one thing, this is used when you want to use a command that does
    /// not require you to send some data after it. So in any case except for the W_REGISTER,
    /// the R_REGISTER and the W_ACK_PAYLOAD commands.
    void transfer( uint8_t reg );
    
    /// \brief
    /// reads one byte
    /// \details
    /// lets you read only one byte before stoping the SPI.
    /// This function isn't very usefull in most cases, but in some cases it might be
    /// usefull to use.
    uint8_t read_byte();

    /// \brief
    /// reads registers
    /// \details	
    /// It reads the registers through spi that are in the NRF24L01+
    /// It sends the command R_REGISTER with an additional 5 bits that are used to
    /// read any register that you want. the 0x1F in the code is used as buffer
    /// to make sure the register is only 5 bits long and doesn't mess with the first
    /// 3 bits.
    uint8_t read_register( uint8_t reg );

    /// \brief
    /// reads the register
    /// \details
    /// this is the same function as the other read_register function, however this
    /// function doesn't return a value. This function is used when you want to read
    /// more than 1 byte out of the register, for exemple if you want to read the pipe0
    /// address which is 5 bytes.
    /// You have to send an empty uint8_t array as value, in this empty array the data will
    /// be saved. Hold in mind that you are responsible for the right size of the array.
    /// if you are not sure how long the value must be use an array with a length of 5
    /// because the default address length is 5 bytes and the default payload size is also
    /// 5 bytes long.
    void read_register( uint8_t reg, uint8_t* value, uint8_t len);
	
    /// \brief
    /// writes to a register
    /// \details	
    /// This function is used to write a value to a register using the W_REGISTER.
    void write_register( uint8_t reg, uint8_t value );

    /// \brief
    /// writes to a register
    /// \details
    /// Just like with the read_register function, this is again an other version
    /// of the same function.
    /// It is used to write multiple bytes to a register (only if the register accepts more
    /// than one byte) This implies mostly to the pipe addressess and the rx FIFO
    void write_register( uint8_t reg, uint8_t* value, uint8_t len );
	
    /// \brief
    /// sets the frequency
    /// \details
    /// Sets the channel between a value of 0 and 125.
    void setChannel( uint8_t channel );
	
	/// \brief
    /// gets the frequency
    /// \details
    /// Gets the channel, the value is between 0 and 125
    /// To calculate the frequency just do f = 2400 + channel
    /// The f is in Mhz and not in Ghz.
    uint8_t getChannel();

    /// \brief
    /// sets the chip select not pin
    /// \details
    /// This function is used to set the value of the chip select not pin
    /// The not in the name implies that the SPI communication is started
    /// when the pin is LOW and not HIGH
    /// To start a SPI transaction the csn pin must first be HIGH, before
    /// it should be set to LOW.
    /// To stop a transaction simply set the pin to HIGH
    void set_csn( bool x );
   
    /// \brief
    /// sets the chip enable pin
    /// \details
    /// The Chip Enable pin is used to send data over the NRF chip to another
    /// NRF chip, if the CE is HIGH it will start transmitting or receiving data
    /// depending on what the PRIM_RX is set to.
    /// To transmit data this pin must be at least 130 microseconds HIGH, before setting
    /// it LOW again.
    void set_ce( bool x );
    
    /// \brief
    /// sets default values
    /// \details
    /// Before starting any communication between chips, there must be some values
    /// in the register set high or low. The channel is set to 100 which is 2500 Mhz
    /// the rx FIFO and the tx FIFO are flushed etc.
    void start();

    /// \brief
    /// writes which pipe to use
    /// \details
    /// The NRF chip has 6 pipes that can be used to receive data
    /// and has only one pipe to transmit data.
    /// To transmit and receive data the addresses of both chips must be the same
    /// Also to be able to receive ACK packets the transmitter must set a RX pipe
    /// the same address as the TX address pipe.
    void write_pipe( uint8_t *address );
    
    /// \brief
    /// sets which pipe to read
    /// \details
    /// Sets an address in the pipe that will be used to read.
    /// The read address must be the same as the transmit address.
    /// The address is set to 5 bytes in the start function, this can be changed
    /// to the values 3 and 4 using the setAddressWidth() function.
    /// The pipe that we are using is pipe0 and can't be changed yet.
    void read_pipe( uint8_t *address );

    /// \brief
    /// powers up to receive
    /// \details
    /// Sets the PRIM_RX register to 1 and the CE pin high to enter RX mode
    /// This function sets the registers to be able to receive data.
    void powerUp_rx();

    /// \brief
    /// powers up the transmitter
    /// \details
    /// Sets the PRIM_RX register to 0, to enter TX mode as transmitting is started
    /// Sets certain registers to be able to transmit data
    void powerDown_rx();
    
    /// \brief
    /// powers up the device
    /// \details
    /// goes from power down mode to standby-I mode or standby-II mode
    /// must be powered up to be able to transmit or receive data
    /// doesn't need to be powered up to be able to change register values or
    /// to read them.
    void powerup();

    /// \brief
    /// powers the device down
    /// \details
    /// Can be turned of when you don't need the device at that moment.
    /// However in standby-I or standby-II mode you will almost save the same amount
    /// of energy.
    void powerdown();

    /// \brief
    /// Empties the rx FIFO
    /// \details
    /// sends the command that will empty the RX FIFO.
    /// Used in RX mode.
    /// should not be executed during transmission of acknowledge, because
    /// otherwise the acknowledge package will not be completed.
    void flush_rx();

    /// \brief
    /// Empties the tx FIFO
    /// \details
    /// Flushes the data in TX FIFO
    /// Used in TX mode
    void flush_tx();

    /// \brief
    /// reads the value of the rf setup
    /// \details
    /// reads the value of the rf setup.
    /// can be used in case you want to make sure certain values are turned
    /// on or off.
    uint8_t read_setup();

    /// \brief
    /// sets the data rate
    /// \details
    /// Sets the speed of the data rate.
    /// The data rate speed is by default 1 Mbps if you use the start() function.
    /// 0 = 250 Kbps, 1 = 1 Mbps, 2 = 2 Mbps.
    void setDataRate( uint8_t speed );

    /// \brief
    /// reads the data rate
    /// \details
    /// Reads the value of the data rate into a string.
    /// Gives an easier way of reading the data rate than getting some bits.
    hwlib::string<8> getDataRate();

    /// \brief
    /// sets the output power
    /// \details
    /// Sets the output power.
    /// How higher the value the further it can transmit data.
    /// 0 = -18dBm, 1 = -12dBm, 2 = -6dBm and 3 = 0dBm
    /// a value of 3 is the highest possible transmit value.
    /// in the start() function this function is set to -18dBm
    void setOutputPower( uint8_t value );

    /// \brief
    /// reads the output power
    /// \details
    /// This function is meant to make it easier to read the value.
    /// Instead of returning a number in bits, I decided to make it easier
    /// by sending a string with the readable value in it.
    hwlib::string<8> getOutputPower();

    /// \brief
    /// Transmit
    /// \details
    /// Is given an uint8_t array and a length of the array.
    /// This function will start to write a value to the FIFO and then
    /// transmit it. To set the value in the TX FIFO the function
    /// write_payload is used in this function.
    void write( uint8_t* value, uint8_t len );
    
    /// \brief
    /// Transmits long data
    /// \details
    /// To take you all the difficult work out of hands, I wrote this function
    /// to be able to send more than 32 bytes of data.
    /// this ofcourse means that is is still just a simple write function that is
    /// send multpiple times into parts.
    /// So now you can make extremly big arrays and put all your data at once in it.
    void writeLong( uint8_t* value, uint8_t len );

    /// \brief
    /// receive data
    /// \details
    /// This function will start reading the values that are saved automatically in
    /// the RX FIFO if the chip is in RX mode.
    /// again this will not return any value, instead you give an array of uint8_t.
    void read( uint8_t* value, uint8_t len );
    
    /// \brief
    /// writes the payload
    /// \details
    /// Writes the payload that is given by the function write() into the
    /// TX FIFO by using the command W_TX_PAYLOAD.
    void write_payload( uint8_t* value, uint8_t len );
    
    /// \brief
    /// reads the payload
    /// \details
    /// Reads the payload that is received into RX FIFo by using the command
    /// R_RX_PAYLOAD
    void read_payload( uint8_t* value, uint8_t len );

    /// \brief
    /// checks the fifo
    /// \details
    /// Checks what the value is of the FIFO_STATUS.
    uint8_t check_fifo();

    /// \brief
    /// sets the retries
    /// \details
    /// Sets the amount of retries before stopping to transmit.
    /// Stays sending the same value till the receiver has send a ACK packet back.
    void setRetries( uint8_t delay, uint8_t count );

    /// \brief
    /// reads the amount of retries
    /// \details
    /// Reads the amount of retries that has been set
    /// The default value is set to a delay of 5 and a amount of retries to 15
    void getRetries();

    /// \brief
    /// sets the address width
    /// \details
    /// This function sets the address width.
    /// The address width can be between 3 bytes and 5 bytes, not higher and not
    /// lower. This changes the addr_width value.
    void setAddressWidth( uint8_t width );

    /// \brief
    /// gets the address width
    /// \details
    /// Reads to what the address width has been set
    uint8_t getAddressWidth();

    /// \brief
    /// reads all registers
    /// \details
    /// This function is used as a check.
    /// it displays all the register values in bytes and a few in strings.
    void readAllRegisters( void );

    /// \brief
    /// checks if received
    /// \details
    /// Checks if there is any value in the RX FIFO.
    /// This is important to check before reading the FIFO, because otherwise it
    /// will just stay reading and can give some unpredictable values. Especially
    /// many zero values that can intefear in the code.
    bool checkRXfifo();

    /// \brief
    /// sets payload_size
    /// \details
    /// With this function you are able to set the size of the payload_size.
    /// Hold in mind that you can't change the payloadsize after you have called
    /// the functions read_pipe() and write_pipe().
    void setPayloadSize( uint8_t size );

    /// \brief
    /// gets the value of payload_size
    /// \details
    /// This function is used to read the value of the payload_size.
    uint8_t getPayloadSize();

};

#endif // NRF24_HPP
