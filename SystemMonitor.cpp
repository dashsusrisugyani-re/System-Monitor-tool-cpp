#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <cstdlib>

// CPU Info from /proc/stat
void display_cpu_info() {
    std::ifstream stat_file("/proc/stat");
    std::string line;
    if (std::getline(stat_file, line)) {
        std::istringstream iss(line);
        std::string cpu;
        long user, nice, system, idle, iowait, irq, softirq;
        iss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq;
        long total = user + nice + system + idle + iowait + irq + softirq;
        long used = total - idle - iowait;
        std::cout << "CPU (since boot): " << (used * 100.0 / total) << "% used\n";
    }
}

// RAM and Swap Info
void display_mem_info() {
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        double total_ram = info.totalram / (1024.0*1024*1024);
        double free_ram = info.freeram / (1024.0*1024*1024);
        double total_swap = info.totalswap / (1024.0*1024*1024);
        double free_swap = info.freeswap / (1024.0*1024*1024);
        std::cout << "RAM: Total " << total_ram << " GB, Free " << free_ram << " GB\n";
        std::cout << "Swap: Total " << total_swap << " GB, Free " << free_swap << " GB\n";
    }
}

// Disk Info using system command
void display_disk_info() {
    std::cout << "Disk Usage:\n";
    system("df -h | grep '^/dev/'");
}

// Top 5 CPU Processes
void display_process_info() {
    std::cout << "Top 5 processes by CPU usage:\n";
    system("ps -eo pid,comm,%cpu --sort=-%cpu | head -n 6");
}

// Network Usage (from /proc/net/dev)
void display_network_info() {
    std::ifstream netdev_file("/proc/net/dev");
    std::string line;
    std::cout << "Network Usage (bytes):\n";
    // Skip 2 header lines
    std::getline(netdev_file, line); std::getline(netdev_file, line);
    while (std::getline(netdev_file, line)) {
        std::istringstream iss(line);
        std::string iface;
        long rx_bytes, tx_bytes;
        iss >> iface >> rx_bytes;
        for (int i=0; i<7; ++i) iss >> tx_bytes;
        size_t colon = iface.find(':');
        if (colon != std::string::npos) iface = iface.substr(0, colon);
        std::cout << "  " << iface << ": RX " << rx_bytes << " bytes, TX " << tx_bytes << " bytes\n";
    }
}

int main() {
    while (true) {
        std::cout << "====== Linux System Monitor ======\n";
        display_cpu_info();
        display_mem_info();
        display_disk_info();
        display_process_info();
        display_network_info();
        std::cout << "\nPress Ctrl+C to exit." << std::endl;
        sleep(5);
        system("clear");
    }
    return 0;
}
