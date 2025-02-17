// i2c-socket.cpp
#include "i2c-socket.h"

I2C_Socket::I2C_Socket(const char* name, int client_address,
            int master_address) :
    I2C_Device(name, client_address) {
    
    set_master(master_address);

    add_subdevice(new Subdevice("")); // 0
    add_subdevice(new Subdevice("set",true))->with_receive_cb(
        [&] (const Ustring& payload) {
            // TODO: check that correct i2c bus is selected
            Wire.beginTransmission(get_address());
            for( int i=0; i<payload.length(); i++) {
                Wire.write((uint8_t)(payload.as_cstr()[i]));
            }
            Wire.endTransmission();
            return true;
        }
    );
}

void I2C_Socket::i2c_start() {
    _started = true;
}

bool I2C_Socket::measure() {

    // notable is that this already reads the answer, no need for any delays after this
    if(Wire.requestFrom((uint8_t) get_address(), 
            (uint8_t) IOTEMPOWER_I2C_CONNECTOR_BUFFER_SIZE, 
            (uint8_t) false) == 0) {
        ulog("Reading from i2c not successful. Dev with address %d connected?", get_address());
        return false;
    }

    if(Wire.available() < 3) {
        ulog("No answer received");
        return false; // not even header
    }

    uint16_t count = (Wire.read() & 0xff) << 8;
    count += Wire.read() & 0xff;

    uint16_t buf_len = Wire.read(); // should be length
    
    if(count == 0xffff) {
        ulog("No data after request received.", last_count);
        return false;
    }
    if(count == last_count) return false; // ignore same package TODO: enable
    if(count - last_count > 255) {
        last_count =  count;
        ulog("Received far off package with nr: %d", last_count);
        return false; // too much distance
    }
    last_count = count;

    Ustring& v = measured_value();
    v.clear();
    int left = buf_len;
    while(left>0 && Wire.available()>0) {
        v.add((uint8_t)Wire.read()); // TODO: a bit inefficient, optimize if necessary
        left--;
    }
//    ulog("count: %d len: %d content: %s", count, buf_len, v.as_cstr());
    return true;
}
