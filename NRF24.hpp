#ifndef NRF24_HPP
#define NRF24_HPP

#include "hwlib.hpp"

class NRF24 {
protected:
	hwlib::spi_bus & bus; 
	uint16_t ce;
	uint16_t csn;

public:
    
    NRF24( hwlib::spi_bus & bus, uint16_t ce, uint16_t csn); 
	
    uint8_t read_register(uint8_t reg);
	
    uint8_t write_register(uint8_t register, uint8_t value);
	
    void setChannel(uint8_t channel);
	
    uint8_t getChannel();

};

#endif // NRF24_HPP
