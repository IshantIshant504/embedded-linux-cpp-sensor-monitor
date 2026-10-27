#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <cerrno>
#include <cstring>
int main() {
    const char* device = "/dev/i2c-1";
    int address = 0x3C;

    int fd = open(device, O_RDWR);
    if (fd < 0) {
        std::cerr << "Cannot open " << device
                  << ": " << strerror(errno) << "\n";
        return 1;
    }

    if (ioctl(fd, I2C_SLAVE, address) < 0) {
        std::cerr << "Cannot select OLED address 0x3C: "
                  << strerror(errno) << "\n";
        close(fd);
        return 1;
    }   
     unsigned char command[] = {0x00, 0xAE};

    if (write(fd, command, sizeof(command)) != sizeof(command)) {
        std::cerr << "I2C transaction failed: "
                  << strerror(errno) << "\n";
        close(fd);
        return 1;
    }

    std::cout << "I2C transaction succeeded at 0x3C\n";

    close(fd);
    return 0;
}

