#include <cstdio>
#include <cstring>
#include <cstdlib>
#include<bits/stdc++.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "functions.cpp"
using namespace std;

void changeInRouterTableAfterFailure(string neighbor)
{

	typedef map<string, node>:: iterator it_type;
	    for(it_type iterator = routerTable.begin(); iterator!= routerTable.end();iterator++)
	    {
		string s = iterator->first;
		node n = iterator->second;
		if(n.nextHop == neighbor)
		{routerTable[s].nextHop = unreachable; routerTable[s].cost = NULL_VALUE;} 

	    }
}

int getNewCost(string nextHop)
{

	
	return neighbors[nextHop].currentCost-neighbors[nextHop].oldCost;
}

void changeNextHopValue(string nextHop )
{
	

	
	int newCost = getNewCost( nextHop);
	typedef map<string, node>:: iterator it_type;
	    for(it_type iterator = routerTable.begin(); iterator!= routerTable.end();iterator++)
	    {
		string s = iterator->first;
		node n = iterator->second;
		if(n.nextHop == nextHop)
		{ routerTable[s].cost = routerTable[s].cost +newCost;}

	    }
	if(routerTable[nextHop].cost>neighbors[nextHop].currentCost) routerTable[nextHop].cost = neighbors[nextHop].currentCost;
}

void linkFailureDetection()
{


		

	typedef map<string, neighborNode>:: iterator it_type;
			    for(it_type iterator = neighbors.begin(); iterator!= neighbors.end();iterator++)
			    {
				string s = iterator->first;
				neighborNode n = iterator->second;
				if(n.clkTick >= 3 && neighbors[s].up == true) //if neighbor info fails to come 3 times the link is down 
				{
					neighbors[s].up = false;
					cout<<"link to "<<s<<" is not available\n";
					
					changeInRouterTableAfterFailure(s);
					//printRouterTable();
					
				}


			    }

}

void updateCostInNeighborTable(string neighbor, int newCost)
{

	string s = neighbor;
	
	int old = neighbors[s].currentCost;
					neighbors[s].oldCost = old;
					neighbors[s].currentCost= newCost;

					neighborMap();

}


int main(int argc, char *argv[]){

	int sockfd;
	int bytes_received;
	int bind_flag;
	int sent_bytes;
	socklen_t addrlen;
	char buffer[1024];
	
	struct sockaddr_in server_address;
	//struct sockaddr_in neighbor_address[];
	struct sockaddr_in client_address;
	struct sockaddr_in recv_address;

	if(argc != 3){
		printf("%s <ip address>\n", argv[0]);
		exit(1);
	}

	string fileName(argv[2]);

	int i;


	client_address.sin_family = AF_INET;
	client_address.sin_port = htons(4747);
	//client_address.sin_addr.s_addr = inet_addr(argv[1]);
	inet_pton(AF_INET,argv[1],&client_address.sin_addr);

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	bind_flag = bind(sockfd, (struct sockaddr*) &client_address, sizeof(sockaddr_in));
	if(bind_flag==0)printf("successful bind");

	string input;
	string str(argv[1]);
	ownIP = str;

	populateRouterTable(str,fileName);
	printRouterTable();

	//initialize router sockets
	struct sockaddr_in neighbor_address[numberOfNeighbors]; //creating sockets for neighbors

	int j = 0;
	
		cout<<"initializing neighbor sockets:\n";
		    typedef map<string, neighborNode>:: iterator it_type;
		    for(it_type iterator = neighbors.begin(); iterator!= neighbors.end();iterator++)
		    {
			string s = iterator->first;

			
			neighbor_address[j].sin_family = AF_INET;
			neighbor_address[j].sin_port = htons(4747);
			//server_address.sin_addr.s_addr = inet_addr("192.168.10.100");
			inet_pton(AF_INET,s.c_str(),&neighbor_address[j].sin_addr);
			cout<<"initialized "<<s<<endl;
			j++;
		    }
	

	while(true){
		for(int i=0;i<1024-1;i++){
		buffer[i]='\0';
		}
		linkFailureDetection();
		
		bytes_received = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*) &recv_address, &addrlen);
		
		printf("[%s:%hu]: \n", inet_ntoa(recv_address.sin_addr), ntohs(recv_address.sin_port) );
		string recvID(inet_ntoa(recv_address.sin_addr));
		
		printf("%c \n",buffer[0]);
		
		if(buffer[0] == 's' && buffer[1] == 'h') //show 
		{
			printRouterTable();
			
			neighborMap();

		}
		else if(buffer[0] == 'c' && buffer[1] == 'l')//clk
		{
			//printRouterTable();
			string serial = serializeToString();
			const char *ch = serial.c_str();
			int length = serial.length()+1;
			int ind;
			for(int ind = 0;ind<numberOfNeighbors;ind++)
			{
				sent_bytes=sendto(sockfd, ch, length, 0, (struct sockaddr*) &neighbor_address[ind], sizeof(sockaddr_in));

			}
//increment clkTick
			typedef map<string, neighborNode>:: iterator it_type;
			    for(it_type iterator = neighbors.begin(); iterator!= neighbors.end();iterator++)
			    {
				string s = iterator->first;
				//neighborNode n = iterator->second;
				//n.clkTick++;
				neighbors[s].clkTick++;
				//cout<<"clktick for "<<s<<" "<<neighbors[s].clkTick<<endl;


			    }
			


		}
		else if(buffer[0] == '1' && recvID!= ownIP ) //neighbor info received
		{
			string ser = serializeToString();
			
			string bufToStr(buffer);
			if(ser != bufToStr)
			{

				deSerialize(bufToStr);
				updateRoundTable(recvID);

			}
			
			neighbors[recvID].clkTick=0;
			neighbors[recvID].up = true;
		}else if(buffer[0] == 'c' && buffer[1] == 'o' ) //cost update
		{
			
			char ip[20];char ip2[20]; char cost[20];
			
			//cout<<buf<<endl;
			inet_ntop(AF_INET, buffer + 4, ip, sizeof(ip)); string ipStr(ip); cout<<ipStr<<endl;
			inet_ntop(AF_INET, buffer + 8, ip2, sizeof(ip2)); string ipStr2(ip2); cout<<ipStr2<<endl;
			
			inet_ntop(AF_INET, buffer + 12, cost, sizeof(cost)); string ipStr3(cost); cout<<ipStr3<<endl;
			string delimeter = ".";
			string token = ipStr3.substr(0,ipStr3.find(delimeter));
			cout<<token<<endl;
			int costInt = atoi(token.c_str());
			if(ownIP == ipStr)
			{
				updateCostInNeighborTable(ipStr2, costInt); changeNextHopValue(ipStr2 );
			}
			else
			{updateCostInNeighborTable(ipStr, costInt);changeNextHopValue(ipStr );}

			
		}
		else if(buffer[0] == 's' && buffer[1] == 'e' ) //send 
		{
			
			char ip[20];char ip2[20]; char msgLen[20]; int length;
			
			//cout<<buf<<endl;
			inet_ntop(AF_INET, buffer + 4, ip, sizeof(ip)); string ipStr(ip); 
			inet_ntop(AF_INET, buffer + 8, ip2, sizeof(ip2)); string ipStr2(ip2); 
			inet_ntop(AF_INET, buffer + 12, msgLen, sizeof(msgLen)); string ipStr3(msgLen); 
			string delimeter = ".";
			string token = ipStr3.substr(0,ipStr3.find(delimeter));
			
			length = atoi(token.c_str());
			char msg[length+1];
			memcpy(msg, &buffer[14], length);
			msg[length] = '\0';
			
			string message(msg);
			string destination = getNextHop(ipStr2);
			if(destination != unreachable){
				int indexOfNextHop = findNeighborIndex(destination);
			
				string msgToSend ="";
				msgToSend = "frwd "+ipStr2+" "+token+" "+message;
				cout<<message<<" packet forwarded to "<<destination<<endl;
				sent_bytes=sendto(sockfd, msgToSend.c_str(), msgToSend.length()+1, 0, (struct sockaddr*) &neighbor_address[indexOfNextHop], sizeof(sockaddr_in));
			}
			else
			{ cout<<"can't reach destination\n";}
			
			
			
		}
		else if(buffer[0] == 'f' && buffer[1] == 'r' ) //frwd
		{
			
			
			
			

				string input(buffer);
			    std::istringstream iss(input);
			    string f , ip , msgLen, msg;
			    

    
        
			    std::string sub;
			    
			    iss >> sub;
			    f = sub;
			    cout<<f<<endl;

			    iss >> sub;
			    ip = sub;
			    cout<<ip <<endl;

			    iss >> sub;
			    msgLen = sub;
			    cout<<msgLen<<endl;

			    
				msg = input.substr(20);

			    if(ip == ownIP)
			    {
				cout<<msg<<" packet reached destination"<<endl;		
				}
				else
				{
					printf("send to someone else\n");
					string destination = getNextHop(ip);
					if(destination != unreachable){
						
			
						int indexOfNextHop = findNeighborIndex(destination);
			
						string msgToSend ="";
						msgToSend = "frwd "+ip+" "+msgLen+" "+msg;
						cout<<msg<<" packet forwarded to "<<destination<<endl;
						sent_bytes=sendto(sockfd, msgToSend.c_str(), msgToSend.length()+1, 0, (struct sockaddr*) &neighbor_address[indexOfNextHop], sizeof(sockaddr_in));
					}
					else
					{ cout<<"can't reach destination\n";}
				}
           
		}

		

		
		
		
	}

	close(sockfd);

	return 0;

}
