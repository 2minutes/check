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

using namespace std;

int main(int argc, char const *argv[]){
	int sock=socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1){
		return 1;
	}
	int port=54000;
	string ipAddress="127.0.0.1";

	sockaddr_in hint;
	hint.sin_family=AF_INET;
	hint.sin_port=htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	int connectRes = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if(connectRes == -1){
		return 1;
	}

	char buf[4096];
	string mapID=argv[1];
	string sourceVertex=argv[2];
	string fileSize=argv[3];
	string userInput;
	userInput=mapID+"$"+sourceVertex+"$"+fileSize;
	cout<<"The client is up and running."<<endl;
	cout<<userInput<<endl;
	do {
		cout << "> ";
		// getline(cin, userInput);


		int sendRes=send(sock, userInput.c_str(), userInput.size()+1,0);
		cout<<"The client has sent query to AWS using TCP over port 54000: start vertex "<<sourceVertex<<"; map "<<mapID<<"; file size "<<fileSize<<endl;
		if(sendRes==-1){
			cout<< "Could not send to server! Whoops!\r\n";
			continue;
		}

		memset(buf, 0, 4096);
		int butesReceived =recv(sock, buf, 4096,0);
		cout<<"The client has sent received results from AWS:"<<endl;
		cout<<"---------------------------------------"<<endl;
		cout<<"Destination 		Min Length 		Tt 		Tp 		Delay"<<endl;
		cout<<"---------------------------------------"<<endl;
		// cout << "SERVER>" << string(buf, butesReceived) << "\r\n";
		getline(cin, userInput);
	} while(true);

	close(sock);

	return 0;
}