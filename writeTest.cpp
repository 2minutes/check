#include <sstream> 
#include <vector> 
#include <string> 
#include <fstream> 
#include <iostream> 
#include <map> 
#include <cstring>
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <queue>
#include <typeinfo>

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



#define PORT "21816"
#define PORT2 "23816"
#define SERV_IP "127.0.0.1"

using namespace std; 
using namespace boost;
struct node{
    int vertexNum;
    int dis;
    bool operator < (const node n) const{
        return n.dis < dis;
    }
};

class Comparator {
public:
    bool operator()(const node* a, const node* b)
    {
        return (a->dis > b->dis);
    }
};

void constructGraph();
void findSmallestPath(string mapId, int source);
bool isNum(string str);  
void dijkstra(map<int, vector<vector<int> > > &graph, map<int, node*> &curDis, int source, map<int, int> &distance);
bool cmp(vector<int>&a, vector<int>& b);


void createReceive(string &mapId, int &source);
void createSend();


class Map{
public:
    string id;
    int val1;
    int val2;
    map<int, vector<vector<int> > > graph;

    Map(string id, int val1, int val2){
        this->id=id;
        this->val1=val1;
        this->val2=val2;
    }
};

vector<string> vec;
map<string, Map> maps;
string res;


int main() 
{ 
    ifstream myfile("./map.txt"); 
    string temp; 
    if (!myfile.is_open()) 
    { 
        cout << "Can't open the file" << endl; 
    } 
    
    while(getline(myfile,temp,char(13))) 
    { 
        // if(temp.size() && temp[0]==char(10)){
        //     temp=temp.substr(1);
        // }
        vec.push_back(temp);
        myfile.get();
    } 

    // vector<string> p;
    // ifstream infile("./map.txt");
    // string s;
    // while(!infile.eof()){
    //     infile>>s;
    //     p.push_back(s);
    // }
    // p.pop_back();
    // cout<<p[0]<<"size"<<endl;
    

    constructGraph();

    cout<<"The server A has constructed a list of "<<maps.size()<<" maps:"<<endl;
    cout<<"----------------------------------------------------------"<<endl;
    cout<<"Map ID          Num Vertices          Num Edges"<<endl;
    cout<<"----------------------------------------------------------"<<endl;
    map<string, Map>::iterator out=maps.begin();
    while(out!=maps.end()){
         map<int, vector<vector<int> > >::iterator itt=out->second.graph.begin();
         cout<<out->first<<"                    ";
         cout<<out->second.graph.size()<<"                  ";
         int sum=0;
        while(itt!=out->second.graph.end()){
             int size=itt->second.size();
             sum+=size;
            ++itt;
        }
        cout<<sum/2<<"                  "<<endl;
        out++;
    }
    cout<<"----------------------------------------------------------"<<endl;
   
    string mapId="";
    int source; 
    createReceive(mapId, source);
    cout<<"The server A has received input for finding shortest paths: starting vertex "<<source<<" of map "<<mapId<<"."<<endl;
    
    res="";
    findSmallestPath(mapId, source);
    

    

    createSend();
    cout<<"The server A has sent shortest paths to AWS."<<endl;
    myfile.close(); 
    // outfile.close();
    return 0; 
} 

void findSmallestPath(string mapId, int source){
    map<string, Map>::iterator itor=maps.find(mapId);
    cout<<itor->first<<endl;
    map<int, vector<vector<int> > > tempGraph=itor->second.graph;
    
    map<int, int> distance;
    map<int, node* > curDis;
    dijkstra(tempGraph, curDis, source, distance);
    cout<<"The server A has identified the following shortest paths:"<<endl;
    cout<<"------------------------------------------------"<<endl;
    cout<<"Destination          Min Length"<<endl;
    map<int, int>::iterator itDis=distance.begin();
    while(itDis!=distance.end()){
        res.append(to_string(itDis->first)).append(" ").append(to_string(itDis->second)).append("$");
        cout<<itDis->first<<"                       "<<itDis->second<<endl;
        itDis++;
    }
    cout<<"-------------------------------------------------"<<endl;
}


void dijkstra(map<int, vector<vector<int> > > &graph, map<int, node* > &curDis, int source, map<int, int> &distance){
    map<int, vector<vector<int> > >::iterator it=graph.begin();
    vector<node*> nodes;
    priority_queue<node*, vector<node*>, Comparator> pq;
    while(it!=graph.end()){
        node* curNode=new node();
        curNode->vertexNum=it->first;
        if(it->first==source){
            curNode->dis=0;
        }else{
            curNode->dis=99999999;
        }
        nodes.push_back(curNode);
        curDis[it->first]=curNode;
        pq.push(curNode);
        ++it;
    }

    

    while(pq.empty()==0){
        node* cur=pq.top();
        pq.pop();
        distance[cur->vertexNum]=cur->dis;
        map<int, vector<vector<int> > >::iterator git=graph.find(cur->vertexNum);
        vector<vector<int> > neibors=git->second;
        int size=neibors.size();
        for(int i=0;i<size;i++){
            map<int, int>::iterator in=distance.find(neibors[i][0]);
            if(in==distance.end()){
                int curVal=cur->dis+neibors[i][1];
                map<int, node* >::iterator disIt=curDis.find(neibors[i][0]);
                if(curVal<disIt->second->dis){
                    disIt->second->dis=curVal;
                }
            }
        }
    }
}

void constructGraph(){
    int size=vec.size();
    for(int i=0;i<size-2;i++){
        if(isNum(vec[i+1])&&isNum(vec[i+2]))  
        {   
            string newId=vec[i];
            int temp1=stoi(vec[i+1]);
            int temp2=stoi(vec[i+2]);
            Map curMap(newId, temp1, temp2);
            i+=3;
            map<int, vector<vector<int> > > newGraph;
            while(i<size-2&&!(isNum(vec[i+1])&&isNum(vec[i+2]))){
                vector<string> fields;
                split(fields, vec[i], is_any_of(" "));
                int nodes[3]={stoi(fields[0]),stoi(fields[1]),stoi(fields[2])};
                vector<int> neibor1;
                neibor1.push_back(nodes[1]);
                neibor1.push_back(nodes[2]);
                vector<int> neibor2;
                neibor2.push_back(nodes[0]);
                neibor2.push_back(nodes[2]);
                map<int, vector<vector<int> > >::iterator iter=newGraph.find(nodes[0]);
                if(iter!=newGraph.end()){
                    iter->second.push_back(neibor1);
                }else{
                    vector<vector<int> > newOne;
                    newOne.push_back(neibor1);
                    newGraph.insert(make_pair(nodes[0], newOne));
                }

                iter=newGraph.find(nodes[1]);
                if(iter!=newGraph.end()){
                    iter->second.push_back(neibor2);
                }else{
                    vector<vector<int> > newOne;
                    newOne.push_back(neibor2);
                    newGraph.insert(make_pair(nodes[1], newOne));
                }

                i++;
            }
            if(i==size-2){
                for(i=size-2;i<size;i++){
                    vector<string> fields;
                    split(fields, vec[i], is_any_of(" "));
                    int nodes[3]={stoi(fields[0]),stoi(fields[1]),stoi(fields[2])};
                    vector<int> neibor1;
                    neibor1.push_back(nodes[1]);
                    neibor1.push_back(nodes[2]);
                    vector<int> neibor2;
                    neibor2.push_back(nodes[0]);
                    neibor2.push_back(nodes[2]);
                    map<int, vector<vector<int> > >::iterator iter=newGraph.find(nodes[0]);
                    if(iter!=newGraph.end()){
                        iter->second.push_back(neibor1);
                    }else{
                        vector<vector<int> > newOne;
                        newOne.push_back(neibor1);
                        newGraph.insert(make_pair(nodes[0], newOne));
                    }

                    iter=newGraph.find(nodes[1]);
                    if(iter!=newGraph.end()){
                        iter->second.push_back(neibor2);
                    }else{
                        vector<vector<int> > newOne;
                        newOne.push_back(neibor2);
                        newGraph.insert(make_pair(nodes[1], newOne));
                    }

                }
            }
            curMap.graph=newGraph;
            maps.insert(make_pair(newId, curMap));
            i--;
        }  
    }
}



bool isNum(string str)  
{  
    stringstream sin(str);  
    double d;  
    char c;  
    if(!(sin >> d))  
        return false;  
    if (sin >> c)  
        return false;  
    return true;  
}  


void createReceive(string &mapId, int &source){
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

    if((addr_status=getaddrinfo(SERV_IP, PORT, &hints, &addrInfo))!=0){
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

    cout<<"The Server A is up and running using UDP on port 21816"<<endl;


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

    string str=recv_data;
    // cout<<str<<"!!"<<endl;

    int l=str.length();
    int begin=0;
    if((int)str.at(0)==12){
        cout<<111<<endl;
        begin=1;
    }
    for(int i=begin;i<l;i++){
        if(str.at(i)=='$'){
            mapId=str.substr(begin, i-1);
            cout<<mapId<<" "<<mapId.length()<<endl;
            begin=i+1;
            break;
        }
    }
    for(int i=begin; i<l; i++){
        if(str.at(i)=='$'){
            source=stoi(str.substr(begin, i-1));
            break;
        }
    }
    // cout<<mapId.length()<<endl;
    // mapId="A";
    // source=3;


    close(socketFileDescriptor);
    return;
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

    if((addr_status=getaddrinfo(SERV_IP, PORT2, &hints, &addrInfo))!=0){
        cout<< "Client: Error in getting addr into from "<< SERV_IP<<endl;
        cout<< gai_strerror(addr_status)<< endl;
        exit(1);
    }

    if((socketFileDescriptor=socket(addrInfo -> ai_family, addrInfo -> ai_socktype, addrInfo ->ai_protocol))==-1){
        cout<< "Client: Error in socket creation" << endl;
        exit(1);
    }

    // cout<<"Client: input message to send"<<endl;

    // cout<<res<<endl;
    // cin>>send_data;
    
    int len=res.length();
    res.copy(send_data, len, 0);
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