#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <unistd.h>

int main(void)
{
    struct pollfd pfd;
    char buffer;
    // Use GPIO 17 (BCM numbering)
    system("sudo echo 17     > /sys/class/gpio/export");
    system("sudo echo rising > /sys/class/gpio/gpio17/edge");
    system("sudo echo in     > /sys/class/gpio/gpio17/direction");
    pfd.fd = open("/sys/class/gpio/gpio17/value", O_RDONLY);
    if(pfd.fd < 0)
    {
        puts("Error opening /sys/class/gpio/gpio17/value");
        puts("Run this program as root.");
        return 1;
    }
    // According to the documentation of the Sysfs interface for GPIO
    // (https://www.kernel.org/doc/Documentation/gpio/sysfs.txt),
    // it is necessary to read the "value" file before polling it
    read(pfd.fd, &buffer, 1);
    // Also according to the documentation of the Sysfs interface for GPIO
    // (https://www.kernel.org/doc/Documentation/gpio/sysfs.txt),
    // the poll() should consider the POLLPRI and POLLERR signals
    pfd.events = POLLPRI | POLLERR;
    pfd.revents = 0;
    puts("Waiting for rising edge...");
    poll(&pfd, 1, -1);
    puts("Rising edge detected!");
    close(pfd.fd);
    system("echo 17 > /sys/class/gpio/unexport");
    return 0;
}
