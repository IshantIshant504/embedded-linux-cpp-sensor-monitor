#ifndef OLED_DISPLAY_HPP
#define OLED_DISPLAY_HPP

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <cerrno>
#include <cstring>
#include <cstdint>
#include <vector>

class OLED {
private:
    int fd;
    static const int address = 0x3C;
        // Five-column font for digits 0-9.
    // Each number is represented by five columns of 8 pixels.
    const uint8_t digits[10][5] = {
        {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0
        {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
        {0x42, 0x61, 0x51, 0x49, 0x46}, // 2
        {0x21, 0x41, 0x45, 0x4B, 0x31}, // 3
        {0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
        {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
        {0x3C, 0x4A, 0x49, 0x49, 0x30}, // 6
        {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
        {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
        {0x06, 0x49, 0x49, 0x29, 0x1E}  // 9
    };

       // Uppercase letters A-Z, five columns per character.
    const uint8_t letters[26][5] = {
        {0x7E, 0x11, 0x11, 0x11, 0x7E}, // A
        {0x7F, 0x49, 0x49, 0x49, 0x36}, // B
        {0x3E, 0x41, 0x41, 0x41, 0x22}, // C
        {0x7F, 0x41, 0x41, 0x22, 0x1C}, // D
        {0x7F, 0x49, 0x49, 0x49, 0x41}, // E
        {0x7F, 0x09, 0x09, 0x09, 0x01}, // F
        {0x3E, 0x41, 0x49, 0x49, 0x7A}, // G
        {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
        {0x00, 0x41, 0x7F, 0x41, 0x00}, // I
        {0x20, 0x40, 0x41, 0x3F, 0x01}, // J
        {0x7F, 0x08, 0x14, 0x22, 0x41}, // K
        {0x7F, 0x40, 0x40, 0x40, 0x40}, // L
        {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // M
        {0x7F, 0x04, 0x08, 0x10, 0x7F}, // N
        {0x3E, 0x41, 0x41, 0x41, 0x3E}, // O
        {0x7F, 0x09, 0x09, 0x09, 0x06}, // P
        {0x3E, 0x41, 0x51, 0x21, 0x5E}, // Q
        {0x7F, 0x09, 0x19, 0x29, 0x46}, // R
        {0x46, 0x49, 0x49, 0x49, 0x31}, // S
        {0x01, 0x01, 0x7F, 0x01, 0x01}, // T
        {0x3F, 0x40, 0x40, 0x40, 0x3F}, // U
        {0x1F, 0x20, 0x40, 0x20, 0x1F}, // V
        {0x3F, 0x40, 0x38, 0x40, 0x3F}, // W
        {0x63, 0x14, 0x08, 0x14, 0x63}, // X
        {0x07, 0x08, 0x70, 0x08, 0x07}, // Y
        {0x61, 0x51, 0x49, 0x45, 0x43}  // Z
    };
  
public:
    OLED() : fd(-1) {}

    bool openDevice() {
        fd = open("/dev/i2c-1", O_RDWR);

        if (fd < 0) {
            std::cerr << "Cannot open I2C device: "
                      << strerror(errno) << "\n";
            return false;
        }

        if (ioctl(fd, I2C_SLAVE, address) < 0) {
            std::cerr << "Cannot select OLED address: "
                      << strerror(errno) << "\n";
            close(fd);
            fd = -1;
            return false;
        }

        return true;
    }
    bool sendCommand(uint8_t command) {
        uint8_t buffer[2] = {0x00, command};

        if (write(fd, buffer, 2) != 2) {
            std::cerr << "Command write failed: "
                      << strerror(errno) << "\n";
            return false;
        }

        return true;
    }

    bool initialize() {
        const uint8_t commands[] = {
            0xAE,       // Display OFF
            0xD5, 0x80, // Display clock
            0xA8, 0x1F, // Multiplex ratio: 32 rows
            0xD3, 0x00, // Display offset
            0x40,       // Start line
            0x8D, 0x14, // Enable charge pump
            0x20, 0x00, // Horizontal addressing mode
            0xA1,       // Segment remapping
            0xC8,       // COM scan direction
            0xDA, 0x02, // COM pins for 128x32
            0x81, 0x7F, // Contrast
            0xD9, 0xF1, // Pre-charge period
            0xDB, 0x40, // VCOMH level
            0xA4,       // Display RAM contents
            0xA6,       // Normal display
            0xAF        // Display ON
        };

        for (std::size_t i = 0;
             i < sizeof(commands) / sizeof(commands[0]); ) {

            uint8_t command = commands[i++];

            if (!sendCommand(command)) {
                return false;
            }

            // Commands with one parameter
            if (command == 0xD5 || command == 0xA8 ||
                command == 0xD3 || command == 0x8D ||
                command == 0x20 || command == 0xDA ||
                command == 0x81 || command == 0xD9 ||
                command == 0xDB) {

                if (!sendCommand(commands[i++])) {
                    return false;
                }
            }

            usleep(1000);
        }

        return true;
    }
    bool displayTestPattern() {
        // Select the entire 128x32 display area
        if (!sendCommand(0x21) ||  // Set column address
            !sendCommand(0x00) ||
            !sendCommand(0x7F) ||
            !sendCommand(0x22) ||  // Set page address
            !sendCommand(0x00) ||
            !sendCommand(0x03)) {
            return false;
        }

        // 4 pages x 128 columns = 512 display bytes
        uint8_t buffer[129];
        buffer[0] = 0x40; // OLED data mode

        for (int i = 1; i < 129; ++i) {
            buffer[i] = 0xAA;
        }

        for (int page = 0; page < 4; ++page) {
            if (write(fd, buffer, sizeof(buffer)) !=
                static_cast<ssize_t>(sizeof(buffer))) {
                std::cerr << "Display data write failed: "
                          << strerror(errno) << "\n";
                return false;
            }
        }

        return true;
    }
    bool drawDigit(int digit, int x, int page) {
        if (digit < 0 || digit > 9) {
            return false;
        }

        if (x < 0 || x > 122 || page < 0 || page > 3) {
            return false;
        }

        if (!sendCommand(0x21) ||
            !sendCommand(static_cast<uint8_t>(x)) ||
            !sendCommand(static_cast<uint8_t>(x + 4)) ||
            !sendCommand(0x22) ||
            !sendCommand(static_cast<uint8_t>(page)) ||
            !sendCommand(static_cast<uint8_t>(page))) {
            return false;
        }

        uint8_t buffer[6];
        buffer[0] = 0x40;

        for (int i = 0; i < 5; ++i) {
            buffer[i + 1] = digits[digit][i];
        }

        if (write(fd, buffer, sizeof(buffer)) !=
            static_cast<ssize_t>(sizeof(buffer))) {
            std::cerr << "Digit drawing failed: "
                      << strerror(errno) << "\n";
            return false;
        }

        return true;
    }
    bool drawText(const char* text, int x, int page) {
        if (text == nullptr || x < 0 || x > 127 ||
            page < 0 || page > 3) {
            return false;
        }

        while (*text != '\0') {
            if (x + 5 > 128) {
                break;
            }

            uint8_t buffer[6];
            buffer[0] = 0x40;

            if (*text >= '0' && *text <= '9') {
                int digit = *text - '0';

                for (int i = 0; i < 5; ++i) {
                    buffer[i + 1] = digits[digit][i];
                }
            } else if (*text >= 'A' && *text <= 'Z') {
                int letter = *text - 'A';

                for (int i = 0; i < 5; ++i) {
                    buffer[i + 1] = letters[letter][i];
                }
            } else if (*text == ' ') {
                for (int i = 0; i < 5; ++i) {
                    buffer[i + 1] = 0x00;
                }
            } else {
                // Unsupported characters are drawn as blank spaces.
                for (int i = 0; i < 5; ++i) {
                    buffer[i + 1] = 0x00;
                }
            }

            if (!sendCommand(0x21) ||
                !sendCommand(static_cast<uint8_t>(x)) ||
                !sendCommand(static_cast<uint8_t>(x + 4)) ||
                !sendCommand(0x22) ||
                !sendCommand(static_cast<uint8_t>(page)) ||
                !sendCommand(static_cast<uint8_t>(page))) {
                return false;
            }

            if (write(fd, buffer, sizeof(buffer)) !=
                static_cast<ssize_t>(sizeof(buffer))) {
                std::cerr << "Text drawing failed: "
                          << strerror(errno) << "\n";
                return false;
            }

            x += 6;
            ++text;
        }

        return true;
    }
    bool clearDisplay() {
        if (!sendCommand(0x21) ||
            !sendCommand(0x00) ||
            !sendCommand(0x7F) ||
            !sendCommand(0x22) ||
            !sendCommand(0x00) ||
            !sendCommand(0x03)) {
            return false;
        }

        uint8_t buffer[129];
        buffer[0] = 0x40;

        for (int i = 1; i < 129; ++i) {
            buffer[i] = 0x00;
        }

        for (int page = 0; page < 4; ++page) {
            if (write(fd, buffer, sizeof(buffer)) !=
                static_cast<ssize_t>(sizeof(buffer))) {
                std::cerr << "Display clear failed: "
                          << strerror(errno) << "\n";
                return false;
            }
        }

        return true;
    }

    void closeDevice() {
        if (fd >= 0) {
            close(fd);
            fd = -1;
        }
    }
};

#endif
