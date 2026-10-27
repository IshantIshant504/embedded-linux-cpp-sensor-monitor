#include "sensor_decoder.hpp"
#include "oled_display.hpp"
#include <iostream>
#include <iomanip>
#include <cstdint>
#include <cstring>
#include <cerrno>
#include <ctime>
#include <fstream>
#include <csignal>
#include <atomic>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <string>
#include <arpa/inet.h>
#include <netinet/in.h>


std::atomic<bool> running{true};

void handle_signal(int signal)
{
    if (signal == SIGINT || signal == SIGTERM) {
        running.store(false);
    }
}

int open_can_socket()
{
    int sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    if (sock < 0) {
        perror("socket");
        return -1;
    }

    struct ifreq ifr {};
    std::strncpy(ifr.ifr_name, "can0", IFNAMSIZ - 1);

    if (ioctl(sock, SIOCGIFINDEX, &ifr) < 0) {
        perror("ioctl");
        close(sock);
        return -1;
    }

    struct sockaddr_can addr {};
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(sock, reinterpret_cast<struct sockaddr*>(&addr),
             sizeof(addr)) < 0) {
        perror("bind");
        close(sock);
        return -1;
    }

    struct can_filter filter {};
    filter.can_id = 0x123;
    filter.can_mask = CAN_SFF_MASK;

    if (setsockopt(sock, SOL_CAN_RAW, CAN_RAW_FILTER,
                   &filter, sizeof(filter)) < 0) {
        perror("setsockopt CAN_RAW_FILTER");
        close(sock);
        return -1;
    }

    return sock;
}
int connect_to_ubuntu()
{
    int tcp_sock = socket(AF_INET, SOCK_STREAM, 0);

    if (tcp_sock < 0) {
        perror("TCP socket");
        return -1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5000);

    if (inet_pton(AF_INET, "192.168.50.3",
                  &server_addr.sin_addr) != 1) {
        std::cerr << "Invalid Ubuntu IP address\n";
        close(tcp_sock);
        return -1;
    }

    if (connect(tcp_sock,
                reinterpret_cast<sockaddr*>(&server_addr),
                sizeof(server_addr)) < 0) {
        perror("TCP connect to Ubuntu");
        close(tcp_sock);
        return -1;
    }

    std::cout << "Connected to Ubuntu TCP receiver at "
              << "192.168.50.3:5000\n";

    return tcp_sock;
}
void log_sensor_data(std::ofstream& csv_file,
                     const SensorData& sensor)
{
    std::time_t now = std::time(nullptr);
    std::tm* local_time = std::localtime(&now);

    if (local_time == nullptr) {
        std::cerr << "Error: Could not obtain local time.\n";
        return;
    }

    std::cout << "[" 
              << std::put_time(local_time, "%Y-%m-%d %H:%M:%S")
              << "] "
              << std::fixed << std::setprecision(2)
              << "Temperature: " << sensor.temperature_c
              << " C | Pressure: " << sensor.pressure_hpa
              << " hPa | Humidity: " << sensor.humidity_rh
              << " %RH\n";

    csv_file << std::put_time(local_time, "%Y-%m-%d %H:%M:%S")
             << ","
             << std::fixed << std::setprecision(2)
             << sensor.temperature_c << ","
             << sensor.pressure_hpa << ","
             << sensor.humidity_rh << "\n";

    csv_file.flush();

    if (!csv_file) {
        std::cerr << "Error: Failed to write sensor_log.csv\n";
    }
}

int main()
{
std::signal(SIGINT, handle_signal);
std::signal(SIGTERM, handle_signal);

OLED oled;
bool oled_ready = false;

if (oled.openDevice()) {
    if (oled.initialize() && oled.clearDisplay()) {
        oled_ready = true;
        oled.drawText("STARTING...", 0, 0);
        std::cout << "OLED initialized successfully.\n";
    } else {
        std::cerr << "OLED initialization failed.\n";
        oled.closeDevice();
    }
} else {
    std::cerr << "OLED unavailable; continuing without display.\n";
}

    int sock = open_can_socket();
    int tcp_sock = connect_to_ubuntu();

    if (tcp_sock < 0) {
        std::cerr << "TCP connection failed. "
                  << "Continuing with CAN and CSV only.\n";
    }
if (sock < 0) {
    return 1;
}
    struct timeval timeout {};
timeout.tv_sec = 3;
timeout.tv_usec = 0;

if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,
               &timeout, sizeof(timeout)) < 0) {
    perror("setsockopt SO_RCVTIMEO");
    close(sock);
    return 1;
}
  std::ofstream csv_file("sensor_log.csv", std::ios::app);

if (!csv_file.is_open()) {
    std::cerr << "Error: Could not open sensor_log.csv\n";
    close(sock);
    return 1;
}

if (csv_file.tellp() == 0) {
    csv_file << "Timestamp,Temperature_C,Pressure_hPa,Humidity_RH\n";
    csv_file.flush();
}
    std::cout << "Waiting for BME280 CAN frames on can0...\n";

    while (running.load()) {
        struct can_frame frame {};

        ssize_t n = read(sock, &frame, sizeof(frame));
        if (n < 0) {
    if (errno == EINTR)
        continue;

    if (errno == EAGAIN || errno == EWOULDBLOCK) {
        std::cerr << "Warning: No CAN frame received "
                  << "within 3 seconds.\n";
        continue;
    }

    perror("read");
    break;
}

     if (n != sizeof(frame)) {
    continue;
}

SensorData sensor{};

if (!decode_sensor_frame(frame, sensor)) {
    continue;
}

log_sensor_data(csv_file, sensor);

if (oled_ready) {
    char line1[22];
    char line2[22];
    char line3[22];
    char line4[22];

    std::snprintf(line1, sizeof(line1),
                  "TEMP: %.2f C", sensor.temperature_c);

    std::snprintf(line2, sizeof(line2),
                  "HUM: %.2f %%", sensor.humidity_rh);

    std::snprintf(line3, sizeof(line3),
                  "PRESS: %.1f HPA", sensor.pressure_hpa);

    std::snprintf(line4, sizeof(line4),
                  "STATUS: CAN OK");

    oled.clearDisplay();

    oled.drawText(line1, 0, 0);
    oled.drawText(line2, 0, 1);
    oled.drawText(line3, 0, 2);
    oled.drawText(line4, 0, 3);
}

if (tcp_sock >= 0) {
    std::time_t now = std::time(nullptr);
    std::tm* local_time = std::localtime(&now);

    if (local_time != nullptr) {
        char timestamp[32];

        std::strftime(timestamp, sizeof(timestamp),
                      "%Y-%m-%d %H:%M:%S", local_time);

        char message[256];

        int length = std::snprintf(
            message, sizeof(message),
            "%s,Temperature_C=%.2f,Pressure_hPa=%.2f,Humidity_RH=%.2f\n",
            timestamp,
            sensor.temperature_c,
            sensor.pressure_hpa,
            sensor.humidity_rh
        );

        if (length > 0 &&
            static_cast<std::size_t>(length) < sizeof(message)) {
            ssize_t sent = send(tcp_sock, message,
                                static_cast<std::size_t>(length),
                                MSG_NOSIGNAL);

            if (sent < 0) {
                perror("TCP send");
                close(tcp_sock);
                tcp_sock = -1;
            }
        }
    }
}
}

    

    close(sock);
    if (tcp_sock >=0) {
      close(tcp_sock);
}
    return 0;
}


