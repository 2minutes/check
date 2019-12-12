#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <errno.h> 
#include <string.h> 
#include <netdb.h>
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <sys/wait.h>
#include <iostream>
#include <cstring>
#include <boost/algorithm/string.hpp>
#include <algorithm>

using namespace std;
using namespace boost;

#define PORT "3000"
#define PORT2 "3001"
#define SERV_IP "127.0.0.1"


void createSend();
void createReceive();

string str;
int main(){
	
	while(1){
		createSend();
		createReceive();
	}
	
	return 0;

}

void createSend(){
	struct addrinfo hints, *addrInfo;
	int addr_status;
	int socketFileDescriptor;
	int send_data_len;
	char send_data[100];

	memset(&hints, 0, sizeof hints);
	hints.ai_family=AF_INET;
	hints.ai_socktype=SOCK_DGRAM;

	if((addr_status=getaddrinfo(SERV_IP, PORT, &hints, &addrInfo))!=0){
		cout<< "Client: Error in getting addr into from "<< SERV_IP<<endl;
		cout<< gai_strerror(addr_status)<< endl;
		exit(1);
	}

	if((socketFileDescriptor=socket(addrInfo -> ai_family, addrInfo -> ai_socktype, addrInfo ->ai_protocol))==-1){
		cout<< "Client: Error in socket creation" << endl;
		exit(1);
	}

	int len=str.length();
    str.copy(send_data, len, 0);

	send_data_len=sendto(socketFileDescriptor, &send_data, sizeof(send_data), 0, addrInfo -> ai_addr, addrInfo -> ai_addrlen);

	if(send_data_len==-1){
		cout<<"Client: sending error"<<endl;
		exit(1);
	}

	freeaddrinfo(addrInfo);

	int recv_data_len;
	char recv_data[100];
	sockaddr_in recv_addr;
	socklen_t recv_addr_len;

	recv_addr_len= sizeof(recv_addr);

	close(socketFileDescriptor);

	return;
}

void createReceive(){
	struct addrinfo hints, *addrInfo;
	int addr_status;
	int socketFileDescriptor;
	int recv_data_len;
	char recv_data[100];
	sockaddr_in recv_addr;
	socklen_t recv_addr_len;


	memset(&hints, 0, sizeof hints);
	hints.ai_family=AF_INET;
	hints.ai_socktype=SOCK_DGRAM;

	if((addr_status=getaddrinfo(SERV_IP, PORT2, &hints, &addrInfo))!=0){
		cout<< "Server: Error in getting addr into from "<< SERV_IP<<endl;
		cout<< gai_strerror(addr_status)<< endl;
		exit(1);
	}

	if((socketFileDescriptor=socket(addrInfo -> ai_family, addrInfo -> ai_socktype, addrInfo ->ai_protocol))==-1){
		cout<< "Server: Error in socket creation" << endl;
		exit(1);
	}

	if((bind(socketFileDescriptor, addrInfo -> ai_addr, addrInfo -> ai_addrlen))==-1){
		cout<< "Server: Error in binding server socket"<< endl;
		close(socketFileDescriptor);
		exit(1);
	}

	freeaddrinfo(addrInfo);

	recv_addr_len= sizeof(recv_addr);

	cout<<"Server: waiting to receive.."<<endl;


	recv_data_len=recvfrom(socketFileDescriptor, &recv_data, sizeof(recv_data), 0, (struct sockaddr *)&recv_addr, &recv_addr_len);

	if(recv_data_len==-1){
		cout<<"Server: sending error"<<endl;
		exit(1);
	}

	char recv_addr_string[INET_ADDRSTRLEN];
	inet_ntop(recv_addr.sin_family, &recv_addr.sin_addr, recv_addr_string, sizeof(recv_addr_string));

	cout<< "Server: got packet from"<< recv_addr_string<<endl;
	cout<< "Server: packet is"<<recv_data_len<<"byte(s) long"<<endl;
	cout<<"Server: packet contains the following message: "<< recv_data<<endl;
	str=recv_data;

	string str=recv_data;
	vector<string> fields;
    split(fields, str, is_any_of("$"));
	
	close(socketFileDescriptor);
}