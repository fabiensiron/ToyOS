#ifndef KERNEL_MOD_NET_H
#define KERNEL_MOD_NET_H

typedef uint8_t* (*get_mac_func)(void);
typedef struct ethernet_packet* (*get_packet_func)(void);
typedef void (*send_packet_func)(uint8_t*, size_t);

struct netif {
	void *extra;

	get_mac_func get_mac;
	get_packet_func get_packet;
	send_packet_func send_packet;

	uint8_t hwaddr[6];
	uint32_t source;
};

struct dns_header {
	unsigned short id; // identification number

    unsigned char recursion;
    unsigned char truncated;
    unsigned char authoritive;
    unsigned char opcode;
    unsigned char query_response;

    unsigned char response_code;
    unsigned char checking;
    unsigned char authenticated;
    unsigned char z;
    unsigned char recursion_available;

    unsigned short qestion_count; // number of question entries
    unsigned short answer_count; // number of answer entries
    unsigned short authority_count; // number of authority entries
    unsigned short add_count; // number of resource entries
};

extern void init_netif_funcs(get_mac_func mac_func, get_packet_func get_func, send_packet_func send_func);
extern void net_handler(void * data, char * name);
extern size_t write_dhcp_packet(uint8_t * buffer);
extern void lookup_host(char * address);

extern struct socket* net_open(uint32_t type);
extern int net_send(struct socket* socket, uint8_t* payload, size_t payload_size, int flags);
extern size_t net_recv(struct socket* socket, uint8_t* buffer, size_t len);
extern int net_connect(struct socket* socket, uint32_t dest_ip, uint16_t dest_port);
extern int net_close(struct socket* socket);
#endif
