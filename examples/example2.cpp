#include <iostream>
#include <cstdlib>

#include <arpa/inet.h>

#include "../radix_tree.hpp"

class rtentry {
public:
    in_addr_t addr;
    int       prefix_len;

    rtentry() : addr(0), prefix_len(0) { }

    in_addr_t operator[] (int n) const {
        if (addr & (0x80000000 >> n))
            return 1;
        else
            return 0;
    }

    bool operator== (const rtentry &rhs) const {
        return prefix_len == rhs.prefix_len && addr == rhs.addr;
    }

    bool operator< (const rtentry &rhs) const {
        if (addr == rhs.addr)
            return prefix_len < rhs.prefix_len;
        else
            return addr < rhs.addr;
    }
};

rtentry radix_substr(const rtentry &entry, int begin, int num)
{
    rtentry   ret;
    in_addr_t mask;

    if (num == 32)
        mask = 0;
    else
        mask = 1 << num;

    mask  -= 1;
    mask <<= 32 - num - begin;

    ret.addr       = (entry.addr & mask) << begin;
    ret.prefix_len = num;

    return ret;
}

rtentry radix_join(const rtentry &entry1, const rtentry &entry2)
{
    rtentry ret;

    ret.addr        = entry1.addr;
    ret.addr       |= entry2.addr >> entry1.prefix_len;
    ret.prefix_len  = entry1.prefix_len + entry2.prefix_len;

    return ret;
}

int radix_length(const rtentry &entry)
{
    return entry.prefix_len;
}

radix_tree<rtentry, in_addr> rttable;

void add_rtentry(const char *network, int prefix_len, const char *dst)
{
    rtentry entry;
    in_addr nw_addr;
    in_addr dst_addr;
    in_addr_t mask;
    int       shift;

    if (prefix_len > 32)
        return;

    if (inet_aton(network, &nw_addr) == 0)
        return;

    if (inet_aton(dst, &dst_addr) == 0)
        return;

    shift = 32 - prefix_len;
    if (shift >= 32)
        mask = 0;
    else
        mask = ~((1 << shift) - 1);

    entry.addr       = ntohl(nw_addr.s_addr) & mask;
    entry.prefix_len = prefix_len;

    rttable[entry] = dst_addr;
}

void rm_rtentry(const char *network, int prefix_len)
{
    rtentry entry;
    in_addr nw_addr;
    in_addr_t mask;
    int       shift;

    if (prefix_len > 32)
        return;

    if (inet_aton(network, &nw_addr) == 0)
        return;

    shift = 32 - prefix_len;
    if (shift >= 32)
        mask = 0;
    else
        mask = ~((1 << shift) - 1);

    entry.addr       = ntohl(nw_addr.s_addr) & mask;
    entry.prefix_len = prefix_len;

    rttable.erase(entry);
}

void find_route(const char *dst)
{
    in_addr addr_dst;
    rtentry entry;

    if (inet_aton(dst, &addr_dst) == 0) {
        std::cout << "invalid address: dst = " << dst << std::endl;
        return;
    }

    entry.addr       = ntohl(addr_dst.s_addr);
    entry.prefix_len = 32;

    radix_tree<rtentry, in_addr>::iterator it;

    it = rttable.longest_match(entry);
    if (it == rttable.end()) {
        std::cout << "no route to " << dst << std::endl;
        return;
    }

    char *addr = inet_ntoa(it->second);

    std::cout << dst << " -> " << addr << std::endl;
}

int
main()
{
    // add entries to the routing table
    add_rtentry("0.0.0.0",      0, "192.168.0.1");  // default route
    add_rtentry("10.0.0.0",     8, "192.168.0.2");
    add_rtentry("172.16.0.0",  16, "192.168.0.3");
    add_rtentry("172.17.0.0",  16, "192.168.0.4");
    add_rtentry("172.18.0.0",  16, "192.168.0.5");
    add_rtentry("172.19.0.0",  16, "192.168.0.6");
    add_rtentry("192.168.1.0", 24, "192.168.0.7");
    add_rtentry("192.168.2.0", 24, "192.168.0.8");
    add_rtentry("192.168.3.0", 24, "192.168.0.9");
    add_rtentry("192.168.4.0", 24, "192.168.0.10");

    // lookup the routing table
    find_route("10.1.1.1");
    find_route("172.16.0.3");
    find_route("172.17.0.5");
    find_route("172.18.10.5");
    find_route("172.19.200.70");
    find_route("192.168.1.10");
    find_route("192.168.2.220");
    find_route("192.168.3.80");
    find_route("192.168.4.100");
    find_route("172.20.0.1");

    // remove an entry
    rm_rtentry("10.0.0.0", 8);
    find_route("10.1.1.1");

    return EXIT_SUCCESS;
}
