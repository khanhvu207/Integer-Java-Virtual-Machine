#include "net.h"
#include "state.h"
#include "instructions.h"
#include <stdio.h> 
#include <strings.h> 
#include <string.h>
#include <stdlib.h>

#include <netdb.h>
#include <unistd.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <netinet/in.h>
#include <arpa/inet.h> 

#define SA struct sockaddr
extern ijvm_t machine;

void NETBIND(){
	word_t port = pop();
	dprintf("NETBIND\n");

	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	
	if (server_fd < 0){
		fprintf(stderr, "Can not create a new socket\n");
		HALT();
	}

	struct sockaddr_in server_address, user;
	bzero(&server_address, sizeof(server_address)); 
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr(LOCALHOST); //inet_addr(LOCALHOST); // htonl(INADDR_ANY);
	server_address.sin_port = htons(port);
	
	if ((bind(server_fd, (SA*)&server_address, sizeof(server_address))) != 0){
		fprintf(stderr, "Bind failed\n");
		HALT();
	}

	dprintf("Bind to HOST=%X, PORT=%d\n", server_address.sin_addr.s_addr, port);
	if ((listen(server_fd, 10)) != 0){ //single connection
		fprintf(stderr, "Listen failed\n");
		HALT();
	}

	
	int len = sizeof(user);
	
	int netref = accept(server_fd, (SA*)&user, (socklen_t*)&len);
	if (netref < 0){
		fprintf(stderr, "NETBIND failed, push 0 on the stack\n");
		push(0);
	}
	else{
		fprintf(stderr, "NETBIND successfully, push netref=%d on the stack\n", netref);
		push(netref);
	}
	machine.pc++;
}

void NETCONNECT(){
	word_t port = pop();
	in_addr_t host_ip = pop();
	dprintf("NETCONNECT\n");
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (server_fd < 0){
		fprintf(stderr, "Can not create a new socket\n");
		HALT();
	}
	struct sockaddr_in server_address; 
	bzero(&server_address, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(host_ip);
	server_address.sin_port = htons(port);
	
	dprintf("CONNECT to HOST=%X, PORT=%d\n", host_ip, port);

	if (connect(server_fd, (SA*)&server_address, sizeof(server_address)) == 0){
		fprintf(stderr, "Establish a new connection on PORT=%d\n", port);
		push(server_fd);
	}
	else{
		fprintf(stderr, "Connection failed\n", port);
		push(0);
	}

	machine.pc++;
}

void NETIN(){
	word_t netref = pop();
	char buffer[1024];
	read(netref, buffer, 1024 * sizeof(char));
	fprintf(machine.out, "%s\n", buffer);
	dprintf("NETIN\n");
	machine.pc++;
}

void NETOUT(){
	word_t netref = pop();
	word_t c = pop();
	write(netref, &c, 1);
	dprintf("NETOUT\n");
	machine.pc++;
}

void NETCLOSE(){
	word_t netref = pop();
	close(netref);
	dprintf("NETCLOSE\n");
	machine.pc++;
}