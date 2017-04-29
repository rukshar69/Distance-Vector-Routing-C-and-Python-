#include <cstdio>
#include <cstring>
#include <cstdlib>
#include<bits/stdc++.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#define NULL_VALUE 99999
using namespace std;
string unreachable = "unreachable";
string ownIP="";
//string fileName="";
int numberOfNeighbors = 0;
//structure to store nextHop and associated cost//
struct node{
    string nextHop;
    int cost;

};
//structure to store neighbor's information//
struct neighborNode{

	int currentCost;
	int oldCost ;
	int clkTick ; //to detect link failure
	bool up; //boolean variable to check link validity
};
//routerTable of node
map<string, node> routerTable;
//map to track neighbor's info 
map<string, neighborNode> neighbors;
//print neighbor map
void neighborMap()
{
    cout<<"neighbor table:\n";
    typedef map<string, neighborNode>:: iterator it_type;
    for(it_type iterator = neighbors.begin(); iterator!= neighbors.end();iterator++)
    {
        string s = iterator->first;
        neighborNode n = iterator->second;
        cout<<s<<" "<<n.currentCost<<" "<<n.oldCost<<" "<<n.clkTick<<" "<<n.up <<endl;
//        string str= to_string(n.cost);

    }
    cout<<"number of neighbors: "<<numberOfNeighbors<<endl;
}
//initialize routerTable
void populateRouterTable(string input,string fileName)
{
    string source, dest;
    int c;
    ifstream infile(fileName.c_str());
    while (infile >> source >> dest>>c)
    {
	//case when one is neighbor
        if(source == input )
        {
            node n ;
            n.nextHop = dest;
            n.cost = c;
//neighborNode
	    neighborNode neigh;
	    neigh.currentCost = c; neigh.oldCost = 0; neigh.up = true; neigh.clkTick = 0;
            routerTable.insert ( std::pair<string,node>(dest,n) );
	    neighbors.insert ( std::pair<string,neighborNode>(dest,neigh));  
		numberOfNeighbors++;
        }
        else if( dest == input)
        {
            node n ;
            n.nextHop = source;
            n.cost = c;

	    neighborNode neigh;
	    neigh.currentCost = c; neigh.oldCost = 0; neigh.up = true; neigh.clkTick = 0;

            routerTable.insert ( std::pair<string,node>(source,n) );
	    neighbors.insert ( std::pair<string,neighborNode>(source,neigh)); 
		numberOfNeighbors++;
        }


    }
    // case when none is neighbor
    ifstream anfile(fileName.c_str());
    while (anfile >> source >> dest>>c)
    {


        if(source != input&& dest != input)
        {
            map<string,node>::iterator it;


              it = routerTable.find(source);
              if (it != routerTable.end())
                {
                    //cout<<"already in map\n";
                }
                else
                {
                    node n ;
                    n.nextHop = unreachable;
                    n.cost = NULL_VALUE;
                    routerTable.insert ( std::pair<string,node>(source,n) );

                }



                it = routerTable.find(dest);
              if (it != routerTable.end())
                {
                    //cout<<"already in map\n";
                }
                else
                {
                    node n ;
                    n.nextHop = unreachable;
                    n.cost = NULL_VALUE;
                    routerTable.insert ( std::pair<string,node>(dest,n) );

                }


        }

    }

}

void printRouterTable()
{

	cout<<"checking out map\n";

	    typedef map<string, node>:: iterator it_type;
	    for(it_type iterator = routerTable.begin(); iterator!= routerTable.end();iterator++)
	    {
		string s = iterator->first;
		node n = iterator->second;
		cout<<s<<" "<<n.nextHop<<" "<<n.cost<<endl;

	    }
}


string getNextHop(string input)
{

	

	    typedef map<string, node>:: iterator it_type;
	    for(it_type iterator = routerTable.begin(); iterator!= routerTable.end();iterator++)
	    {
		string s = iterator->first;
		node n = iterator->second;
		if(input == s) return n.nextHop;

	    }
}

string serializeToString()
{
    string serial = "";
    typedef map<string, node>:: iterator it_type;
    for(it_type iterator = routerTable.begin(); iterator!= routerTable.end();iterator++)
    {
        string s = iterator->first;
        node n = iterator->second;
      //  cout<<s<<" "<<n.nextHop<<" "<<n.cost<<endl;
//        string str= to_string(n.cost);
        int Number = n.cost;
        string str = static_cast<ostringstream*>( &(ostringstream() << Number) )->str();

        serial = serial + s + " "+n.nextHop +" "+str+" ";

    }
    return serial;
}
////de serialize from incoming string////////////////////////////
map<string, node> routerTable2;
int countSpaces(string input)
{

    std::string _str( input);
	int iSpaces( 0 );

	for( unsigned int iLoop( 0 ); iLoop < _str.length( ); iLoop++ )
		if( _str.at( iLoop ) == ' ' )
			iSpaces++;

	//std::cout << "Spaces found: " << iSpaces << std::endl;
    return iSpaces;
}



void deSerialize(string input)
{
    //std::string s("Somewhere down the road");
    std::istringstream iss(input);
    string destination , cost , nextHop;
    int spaces = countSpaces(input);
    int count = 0;

    do
    {
        //for(int i = 0;i<3;i++){
            std::string sub;
            if(count == spaces) break;
            iss >> sub;
            destination = sub;
            //std::cout << "Substring: " << sub << std::endl;
            count++;

            iss >> sub;
            nextHop = sub;
            //std::cout << "Substring: " << sub << std::endl;
            count++;

            iss >> sub;
            cost = sub;
            //std::cout << "Substring: " << sub << std::endl;
            count++;

            node n ;
            n.nextHop = nextHop;
            n.cost = atoi(cost.c_str());;
            routerTable2.insert ( std::pair<string,node>(destination,n) );
        //}
    } while (iss);

   
}


int isInNeighbor(string nextHop)
{
    typedef map<string, neighborNode>:: iterator it_type;
    for(it_type iterator = neighbors.begin(); iterator!= neighbors.end();iterator++)
    {
        string s = iterator->first;
        if(s == nextHop)
        {
            return true;
        }

    }
    return false;
}


int findNeighborIndex(string ip2)
{

	int j = 0;
	
		
		    typedef map<string, neighborNode>:: iterator it_type;
		    for(it_type iterator = neighbors.begin(); iterator!= neighbors.end();iterator++)
		    {
			string s = iterator->first;
			//int n = iterator->second;
			
			if(s == ip2) return j;
			j++;
		    }
	return -1;
}


void updateRoundTable(string recvID) //recvID ipAddressY
{

    
	int distance_X_TO_Y = neighbors[recvID].currentCost;
	
	string hop_X_TO_Y = routerTable[recvID].nextHop;
	

	    typedef map<string, node>:: iterator it_type;
	for(it_type iterator = routerTable.begin(); iterator!= routerTable.end();iterator++)
	    {

		string destination = iterator->first;
		node n1 = iterator->second;

		int distance_X_TO_Z = n1.cost;
		int distance_Y_TO_Z ;
		string splitHorizontalTest;
		if(destination == recvID){
			distance_Y_TO_Z = 0;
			splitHorizontalTest= recvID;
		}
		else {
			distance_Y_TO_Z =  routerTable2[destination].cost;
			splitHorizontalTest = routerTable2[destination].nextHop;
		}	
		
		if( (distance_X_TO_Y+distance_Y_TO_Z<=distance_X_TO_Z) || ( n1.nextHop == recvID && splitHorizontalTest!= ownIP)  ){//n1.nextHop == recvID && splitHorizontalTest!= ownIP 
																   //is forced update and split horizon rule respectively
			
			node temp; temp.cost = (distance_X_TO_Y+distance_Y_TO_Z); temp.nextHop = recvID; 
		std::map<string, node>::iterator it = routerTable.find(destination);
                        if (it != routerTable.end())
                            it->second = temp;
	    }
		
		
	    }

      routerTable2.clear();


}
