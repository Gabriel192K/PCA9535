/* Dependencies */
#include "PCA9535.h"

/*!
 * @brief  PCA9535 constructor
 * @param  twi
 *         The TWI bus pointer
 * @param  address
 *         The TWI PCA9535 7 bit address
 */
PCA9535::PCA9535(__TWI__* twi, const uint8_t address)
{
    this->twi = twi;
    this->address = address;
}

/*!
 * @brief  PCA9535 destructor
 */
PCA9535::~PCA9535()
{
    this->twi = NULL;
    this->address = 0;
}

/*!
 * @brief  Begins PCA9535 implementation
 * @return Returns 0 if already began PCA9535 implementation, otherwise returns status of TWI bus connectivity
 */
const uint8_t PCA9535::begin(void)
{
    if (this->began)
        return (0);
    this->began = 1;

    this->twi->begin();
    return (this->isConnected());
}

/*!
 * @brief  Checks PCA9535 TWI bus connectivity status
 * @return Returns status of TWI bus connectivity
 */
const uint8_t PCA9535::isConnected(void)
{
    this->twi->beginTransmission(this->address);
    return (this->twi->endTransmission() == 1);
}

/*!
 * @brief  Setting mode of operation of GPIO's for entire PCA9535
 * @param  mode
 *         Mode of operation of PCA9535 IO's
 * @return Returns status of TWI bus transmission
 */
const uint8_t PCA9535::setMode(const uint16_t mode)
{
    this->modeBuffer = mode;
    this->twi->beginTransmission(this->address);
    this->twi->write(PCA9535_CONFIGURATION_PORT_0_ADDRESS);
    this->twi->write((const uint8_t)(this->modeBuffer & 0xFF));
    this->twi->write((const uint8_t)((this->modeBuffer >> 8) & 0xFF));
    return (this->twi->endTransmission() == 1);
}

/*!
 * @brief  Setting mode of operation for one bit of PCA9535
 * @param  bit
 *         Bit of PCA9535
 * @param  mode
 *         Mode of operation of bit
 * @return Returns 0 if bit is out of bounds or if mode is invalid
 *         Returns status of TWI bus transmission
 */
const uint8_t PCA9535::setMode(const uint8_t bit, const uint8_t mode)
{
    if (bit > 15)
        return (0);
    
    switch (mode)
    {
        case INPUT:
            this->modeBuffer = (bit < 8) ? (this->modeBuffer | (1 << bit)) : (this->modeBuffer | (1 << (bit + 8)));
            break;
        case OUTPUT:
            this->modeBuffer = (bit < 8) ? (this->modeBuffer & ~(1 << bit)) : (this->modeBuffer & ~(1 << (bit + 8)));
            break;
        default: // Invalid mode
            return (0);
            break;
    }
    this->twi->beginTransmission(this->address);
    this->twi->write((bit < 8) ? (PCA9535_CONFIGURATION_PORT_0_ADDRESS) : (PCA9535_CONFIGURATION_PORT_1_ADDRESS));
    this->twi->write((bit < 8) ? ((const uint8_t)(this->modeBuffer & 0xFF)) : ((const uint8_t)((this->modeBuffer >> 8) & 0xFF)));
    return (this->twi->endTransmission() == 1);
}

/*!
 * @brief  Reading entire PCF9535 input registers
 * @return Returns entire PCF9535 input registers
 *         (returns the actual physical bits state !!! NOT THE ONES FROM THE FLIP FLOP ON OUTPUT DRIVER !!!)
 *         (only valid if bits were configured as inputs)
 */
const uint16_t PCA9535::read(void)
{
    this->twi->beginTransmission(this->address);
    this->twi->write(PCA9535_INPUT_PORT_1_ADDRESS);
    this->twi->endTransmission();
    this->twi->requestFrom(PCA9535_INPUT_PORT_1_ADDRESS, (const uint8_t)2, (const uint8_t)1);
    this->readBuffer = this->twi->read();
    this->readBuffer = this->readBuffer | (this->twi->read() << 8);
    return (this->readBuffer);
}

/*!
 * @brief  Reading one bit from PCF9535 input register
 * @return Returns one bit from PCF9535 input register
 *         (returns the actual physical bit state !!! NOT THE ONE FROM THE FLIP FLOP ON OUTPUT DRIVER !!!)
 *         (only valid if bit was configured as input)
 */
const uint8_t PCA9535::read(const uint8_t bit)
{
    if (bit > 15)
        return (0);
    return ((this->read() >> bit) & 1);
}

/*!
 * @brief  Writing entire PCA9535 output registers
 * @return Returns status of TWI bus transmission
 *         (only valid if bits were configured as outputs)
 */
const uint8_t PCA9535::write(const uint16_t state)
{
    this->writeBuffer = state;
    this->twi->beginTransmission(this->address);
    this->twi->write(PCA9535_OUTPUT_PORT_0_ADDRESS);
    this->twi->write((const uint8_t)(this->writeBuffer & 0xFF));
    this->twi->write((const uint8_t)((this->writeBuffer >> 8) & 0xFF));
    return (this->twi->endTransmission() == 1);
}

/*!
 * @brief  Writing one bit of PCA9535 output registers
 * @return Returns status of TWI bus transmission
 *         (only valid if bit were configured as output)
 */
const uint8_t PCA9535::write(const uint8_t bit, const uint8_t state)
{
    if (bit > 15)
        return (0);

    switch (state)
    {
        case LOW:
            this->writeBuffer = (bit < 8) ? (this->writeBuffer & ~(1 << bit)) : (this->writeBuffer & ~(1 << (bit + 8)));
            break;
        case HIGH:
            this->writeBuffer = (bit < 8) ? (this->writeBuffer | (1 << bit)) : (this->writeBuffer | (1 << (bit + 8)));
            break;
        case TOGGLE:
            this->writeBuffer = (bit < 8) ? (this->writeBuffer ^ (1 << bit)) : (this->writeBuffer ^ (1 << (bit + 8)));
            break;
        default: // Invalid state
            return (0);
            break;
    }
    this->twi->beginTransmission(this->address);
    this->twi->write((bit < 8) ? (PCA9535_OUTPUT_PORT_0_ADDRESS) : (PCA9535_OUTPUT_PORT_1_ADDRESS));
    this->twi->write((bit < 8) ? ((const uint8_t)(this->writeBuffer & 0xFF)) : ((const uint8_t)((this->writeBuffer >> 8) & 0xFF)));
    return (this->twi->endTransmission() == 1);
}

/*!
 * @brief  Ends PCA9535 implementation
 * @return Returns 0 if already began PCA9535 implementation, otherwise returns status of TWI bus connectivity
 */
const uint8_t PCA9535::end(void)
{
    if (!this->began)
        return (0);
    this->began = 0;
    this->setMode((const uint16_t)0xFFFF); // Configure everything as input
    this->modeBuffer = 0x0000;
    this->writeBuffer = 0x0000;
    this->readBuffer = 0x0000;
    return (1);
}