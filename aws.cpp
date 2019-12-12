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
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>
#include <algorithm>

#define PORT "21816"
#define PORT2 "22816"
#define PORT3 "23816"
#define SERV_IP "127.0.0.1"

using namespace std;
using namespace boost;


void createSend();
void createSend2();
void createReceive();
void createReceive2();


string str;
vector<string> resFromA;
// UDP
int createUDPSender(string &str){
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

	// cout<<"Client: input message to send"<<endl;
	// cin>>send_data;

	send_data_len=sendto(socketFileDescriptor, &str, sizeof(str), 0, addrInfo -> ai_addr, addrInfo -> ai_addrlen);

	if(send_data_len==-1){
		cout<<"Client: sending error"<<endl;
		exit(1);
	}

	freeaddrinfo(addrInfo);

	close(socketFileDescriptor);

	return 0;

}

int createUDPSender2(string &str){
	struct addrinfo hints, *addrInfo;
	int addr_status;
	int socketFileDescriptor;
	int send_data_len;
	char send_data[100];

	memset(&hints, 0, sizeof hints);
	hints.ai_family=AF_INET;
	hints.ai_socktype=SOCK_DGRAM;

	if((addr_status=getaddrinfo(SERV_IP, PORT3, &hints, &addrInfo))!=0){
		cout<< "Client: Error in getting addr into from "<< SERV_IP<<endl;
		cout<< gai_strerror(addr_status)<< endl;
		exit(1);
	}

	if((socketFileDescriptor=socket(addrInfo -> ai_family, addrInfo -> ai_socktype, addrInfo ->ai_protocol))==-1){
		cout<< "Client: Error in socket creation" << endl;
		exit(1);
	}

	// cout<<"Client: input message to send"<<endl;
	// cin>>send_data;

	send_data_len=sendto(socketFileDescriptor, &str, sizeof(str), 0, addrInfo -> ai_addr, addrInfo -> ai_addrlen);

	if(send_data_len==-1){
		cout<<"Client: sending error"<<endl;
		exit(1);
	}

	freeaddrinfo(addrInfo);

	close(socketFileDescriptor);

	return 0;

}


void createReceive2(){
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

	if((addr_status=getaddrinfo(SERV_IP, PORT3, &hints, &addrInfo))!=0){
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

	// cout<<"The AWS is up and running."<<endl;


	recv_data_len=recvfrom(socketFileDescriptor, &recv_data, sizeof(recv_data), 0, (struct sockaddr *)&recv_addr, &recv_addr_len);

	if(recv_data_len==-1){
		cout<<"Server: sending error"<<endl;
		exit(1);
	}

	char recv_addr_string[INET_ADDRSTRLEN];
	inet_ntop(recv_addr.sin_family, &recv_addr.sin_addr, recv_addr_string, sizeof(recv_addr_string));

	// cout<< "The AWS has received map ID"<< recv_addr_string<<endl;
	// cout<< "Server: packet is"<<recv_data_len<<"byte(s) long"<<endl;
	// cout<<"Server: packet contains the following message: "<< recv_data<<endl;
	str=recv_data;
	
	cout<<"The AWS has received shortest path from server A:"<<endl;
    cout<<"------------------------------"<<endl;
    cout<<"Destination 			Min Length"<<endl;
	
    split(resFromA, str, is_any_of("$"));
    int ll=resFromA.size();
	for(int i=0;i<ll;i++){
		string temp=resFromA[i];
		int size1=temp.length();
		string val1,val2;
		int be=0;
		for(int j=be;j<size1;j++){
			if(temp.at(j)==' '){
				val1=temp.substr(be,j);
				be=j+1;
				break;
			}
		}
		val2=temp.substr(be,size1);
		cout<<val1<<" 					"<<val2<<endl;
	}
	cout<<"------------------------------"<<endl;
    

	string str=recv_data;
	vector<string> fields;
    split(fields, str, is_any_of("$"));
	
	close(socketFileDescriptor);
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

	cout<<"The AWS is up and running."<<endl;


	recv_data_len=recvfrom(socketFileDescriptor, &recv_data, sizeof(recv_data), 0, (struct sockaddr *)&recv_addr, &recv_addr_len);

	if(recv_data_len==-1){
		cout<<"Server: sending error"<<endl;
		exit(1);
	}

	char recv_addr_string[INET_ADDRSTRLEN];
	inet_ntop(recv_addr.sin_family, &recv_addr.sin_addr, recv_addr_string, sizeof(recv_addr_string));

	// cout<< "The AWS has received map ID"<< recv_addr_string<<endl;
	// cout<< "Server: packet is"<<recv_data_len<<"byte(s) long"<<endl;
	// cout<<"Server: packet contains the following message: "<< recv_data<<endl;
	str=recv_data;
	
	cout<<"The AWS has received shortest path from server A:"<<endl;
    cout<<"------------------------------"<<endl;
    cout<<"Destination 			Min Length"<<endl;
	
    split(resFromA, str, is_any_of("$"));
    int ll=resFromA.size();
	for(int i=0;i<ll;i++){
		string temp=resFromA[i];
		int size1=temp.length();
		string val1,val2;
		int be=0;
		for(int j=be;j<size1;j++){
			if(temp.at(j)==' '){
				val1=temp.substr(be,j);
				be=j+1;
				break;
			}
		}
		val2=temp.substr(be,size1);
		cout<<val1<<" 					"<<val2<<endl;
	}
	cout<<"------------------------------"<<endl;
    

	string str=recv_data;
	vector<string> fields;
    split(fields, str, is_any_of("$"));
	
	close(socketFileDescriptor);
}

int main(){
	int listening= socket(AF_INET, SOCK_STREAM, 0);
	if(listening==-1){
		cerr << "Can't create a socket!";
		return -1;
	}

	sockaddr_in hint;
	hint.sin_family=AF_INET;
	hint.sin_port=htons(54000);
	inet_pton(AF_INET,"0.0.0.0", &hint.sin_addr);

	if(bind(listening, (sockaddr *)&hint, sizeof(hint))==-1){
		cerr << "Can't bind to IP/port";
		return -2;
	}

	if(listen(listening, SOMAXCONN)==-1){
		cerr << "Can't ";
		return -3;
	}

	cout<<"The AWS is up and running."<<endl;

	sockaddr_in client;
	socklen_t clientSize=sizeof(client);
	char host[NI_MAXHOST];
	char svc[NI_MAXSERV];

	int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

	if(clientSocket==-1){
		cerr <<"Problem with client";
		return -4;
	}

	close(listening);

	memset(host, 0, NI_MAXHOST);
	memset(svc, 0, NI_MAXSERV);

	int result= getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);
	if(result){
		cout << host << "connected on " << svc <<endl;
	}else{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << "connected on" << ntohs(client.sin_port)<<endl; 
	}

	char buf[4096];


	

	while(true){
		memset(buf, 0, 4096);
		int bytesRecv=recv(clientSocket, buf, 4096, 0);
		if(bytesRecv==-1){
			cerr << "Threr was a connection issue"<< endl;
		}
		if(bytesRecv==0){
			cout<<"disconneted";
			break;
		}
		string str=string(buf, 0, bytesRecv);


		string mapId;
		int source;
		string size;
		int l=str.length();
	    int begin=0;
	    
	   
	    cout<<endl<<endl;
	    for(int i=begin;i<l;i++){
	        if(str.at(i)=='$'){
	            mapId=str.substr(begin, i);
	            cout<<mapId<<" "<<mapId.length()<<endl;
	            begin=i+1;
	            break;
	        }
	    }

	    for(int i=begin; i<l; i++){
	        if(str.at(i)=='$'){
	            source=stoi(str.substr(begin, i));
	            begin=i+1;
	            break;
	        }
	    }
	    
	    size=str.substr(begin, l);

		cout<<"The AWS has received map ID "<<mapId<<", start vertex "<<source<<" and file size "<<size<<" from the client using TCP over port 54000"<<endl; 

		createUDPSender(str);
		cout<<111<<endl;
		createReceive();

		cout<<"The AWS has sent path length, propagation speed and transmission speed to server B using UDP over port 23816."<<endl;
		createUDPSender2(str);
		createReceive2();
		
		cout<<"The AWS has received delays from server B:"<<endl;
		cout<<"------------------------------------"<<endl;
		cout<<"Desination 		Tt 		Tp 		Delay";

		int ll=resFromA.size();
		for(int i=0;i<ll;i++){
			string temp=resFromA[i];
			int size1=temp.length();
			string val1;
			int val2;
			int be=0;
			for(int j=be;j<size1;j++){
				if(temp.at(j)==' '){
					val1=temp.substr(be,j);
					be=j+1;
					break;
				}
			}
			val2=stoi(temp.substr(be,size1));
			cout<<"0.1  					"<<val2/1000<<endl;
		}
		cout<<"------------------------------------"<<endl;
		send(clientSocket, buf, bytesRecv+1, 0);

		cout<<"The AWS has sent calculated delay to client using TCP over port 54000."<<endl;
	}

	close(clientSocket);


	
	return 0;
}

