#include "sensor_decoder.hpp"
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

    int sock = open_can_socket();

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
}

    

    close(sock);
    return 0;
}


