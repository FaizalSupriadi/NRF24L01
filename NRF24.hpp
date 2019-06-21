#ifndef NRF24_HPP
#define NRF24_HPP

#include "hwlib.hpp"

class NRF24 {
protected:
	hwlib::spi_bus & bus; 
	hwlib::pin_out & ce;
	hwlib::pin_out & csn;

public:
    
    NRF24( hwlib::spi_bus & bus, hwlib::pin_out & ce, hwlib::pin_out & csn); 
	
    uint8_t read_register(uint8_t reg);
	
    void write_register(uint8_t reg, uint8_t value);
	
    void setChannel(uint8_t channel);
	
    uint8_t getChannel();

    void set_csn(bool x);

    void set_ce(bool x);

    void start();

    void write_pipe(uint8_t value);

    void read_pipe();

    void powerUp_tx();

    void powerUp_rx();

    void powerDown_rx();

    void powerup();

    void powerdown();

    void flush_rx();

    void flush_tx();

    uint8_t read_setup();

    void setDataRate(uint8_t speed);

};

#endif // NRF24_HPP
