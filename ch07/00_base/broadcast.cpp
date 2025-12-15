
#include <stdio.h>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// 根据 ip 地址和网络掩码计算广播域

std::string broadcast(const std::string& ip, const std::string& netmask)
{
    struct in_addr ip_inaddr;
    struct in_addr netmask_inaddr;
    struct in_addr broadcast_inaddr;

    if (inet_aton(ip.c_str(), &ip_inaddr) == 0) {
        return "";
    }

    if (inet_aton(netmask.c_str(), &netmask_inaddr) == 0) {
        return "";
    }

    netmask_inaddr.s_addr = ~netmask_inaddr.s_addr;
    broadcast_inaddr.s_addr = ip_inaddr.s_addr | netmask_inaddr.s_addr;

    return inet_ntoa(broadcast_inaddr);
}

int main()
{
    {
        std::string ip = "192.168.43.134";
        std::string netmask = "255.255.255.0";
        printf("broadcast: (%s,%s) -> %s\n", ip.c_str(), netmask.c_str(), broadcast(ip, netmask).c_str());  // 192.168.43.255
    }

    {
        std::string ip = "172.16.1.45";
        std::string netmask = "255.255.254.0";
        printf("broadcast: (%s,%s) -> %s\n", ip.c_str(), netmask.c_str(), broadcast(ip, netmask).c_str());  // 172.16.1.255
    }

    {
        std::string ip = "172.20.10.2";
        std::string netmask = "255.255.255.240";
        printf("broadcast: (%s,%s) -> %s\n", ip.c_str(), netmask.c_str(), broadcast(ip, netmask).c_str());  // 172.20.10.15
    }

    return 0;
}