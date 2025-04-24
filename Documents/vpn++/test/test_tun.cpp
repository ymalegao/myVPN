#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <core/tun_device.h>

int main() {
    TunOpenName tun_name;

    // Open a TUN device
    int fd = tunOpen(&tun_name, nullptr);
    if (fd < 0) {
        std::cerr << "Failed to open TUN device: " << strerror(errno) << std::endl;
        return 1;
    }

    std::cout << "Opened TUN device: " << tun_name.name << std::endl;

    // Set non-blocking for simple read test
    fcntl(fd, F_SETFL, O_NONBLOCK);

    // Try reading (will usually return nothing at first)
    char buffer[1500];
    int n = read(fd, buffer, sizeof(buffer));
    if (n > 0) {
        std::cout << "Read " << n << " bytes from TUN device." << std::endl;
    } else {
        std::cout << "No data (yet) or non-blocking read: " << strerror(errno) << std::endl;
    }

    close(fd);
    return 0;
}
