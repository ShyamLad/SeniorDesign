//this code was written by Shyam Lad and with a little help by his bff David, because shyam doesn't know shit about computer architecture and pointers
//this shit has the MIT license thing on it so you can use it however you want, just make sure to cite us in your comments for dat credit
//and follow me on insta @shyamlad

#include <vector>
//#include <Math.h>
#include <iostream>
using namespace std;




// SO, the general idea of A* algo is to calculate the most optimal path to a location based on values that are given to each grid location.
//The location with the lowest value is considered to be the most optimal. This is done repedtedly until you reach the target location
//We call each grid location a 'node'
//so we start with a node, say [0,0], and we begin calculating the distance of the adjacent nodes from the start node (we call that the gcost)
// and to distance to the target node (we call this the heuristic cost, or 'h cost')
//the node with the lowest fcost (gcost + hcost) is chosen, and the whole process starts again until the chosen node is the target node. 


//**********************************************************************************************************************************************

//Most A* algos have a seperate file for the node object, but im like fuck it lets use structs
//A Struct, in simple terms, is a container for a bunch of variables, and you can basically treat them like objects

struct node 
	{
		int posx; //x location of node in grid
		int posy; //y loction of node in grid
		int gcost; // distance from start node
		int hcost; // distance to end node
		bool walkable; // is this grid location occupied by an obstical or not

		struct node* parent; // parent node pointer, we will use this later. Dont worry, ill explain it.....given that i dont get lazy and stop commenting lol 
	};

struct node grid[20][20]; // construct a grid of nodes, i put a 20 by 20 grid, but you can change it
// also notice how i used 'struct node' as if it were an object. This makes it define a grid of the previously defined struct node.
int gridsizex = 20; //self explanitory

int gridsizey = 20; //^^


// this portion initialized the grid, and gives values to some of the struct node variables
void gridInit(){
	for (int i = 0; i < gridsizex; i++)
	{
		for (int j = 0; j < gridsizey; j++)
		{
			grid[i][j].posx = i;
			grid[i][j].posy = j;
			grid[i][j].walkable = true;
		}
	}
	
	//** the below portion is only done to simulate obsticals in the environment. Remove when actually implementing the code
	grid[0][10].walkable = false;
	grid[0][9].walkable = false;
	grid[1][10].walkable = false;
	grid[2][9].walkable = false;


}

//so you have probably wondered, 'hey shyam, how the gosh darn heck are we gonna know which node has been checked, and which nodes we should be checking?
//well the answer is open and closed lists:
std::vector<struct node *> open; // nodes that need to be evaluated

std::vector<struct node *> closed; //nodes that have already been evaluated

std::vector<struct node *> neighbours; //list of neighboring nodes for the node being analyzed, we will use this l8ter g8tor

//totally useless with the math library
int abs(int i) {
	return i >= 0 ? i : -i;
}

//returns node at a given x and y coordinate
struct node * get_node(int x, int y){

	return &grid[x][y]; //the & and * are all pointer shits, google it 

}

//calculate fcost, whcih is just hcost + gcost
int fcost(struct node *a)
{
	return a->gcost + a->hcost; //when attempting to retreive a variable from a struct node pointer, you gotta do this thing '->' rather than a.gcost. google it
}
// calculates distance to from node a to node b
int getDistance(struct node *a, struct node *b){
	int dstX = abs(a->posx - b->posx);
	int dstY = abs(a->posy - b->posy);

	return dstX + dstY; //since the robot will not do diagonal motions, we simply define the distance as x+y.....obviously you will need a diffrent equation to take diagonals into account
}


// checks if  a given node is in the closed list
bool inClosed(struct node *current)
{
	for(int i = 0; i< closed.size(); i++){
		if (current == closed[i])
			return true;
	}
	return false;
}
// checks if a given node is in the open and closed list
bool inOpen(struct node *current)
{
	for(int i = 0; i< open.size(); i++){
		if (current == open[i])
			return true;
	}
	return false;

}



void printnode(struct node* node){ // print this bitch
	cout << node->posx << " ";
	cout << node->posy << "\n";
}

// 'member that parent struct node variable? well here is where we use it. Each node that is checked has a parent node that it refreences.
//for example, start with node a, check nodes around. Choose node with lowest hcost, which is node b. 
//We set node b's parent to be node a.....and then node c's parent will be node b, and etc........ do you feel it now, mr.krabs?
// this parental chain will give us the actual list of nodes that make up the path, in order ( actually its backwards, but still in order)
void RetracePath(struct node *startNode, struct node *endNode){
	cout << "[retrace]" << "\n";
	std::vector<struct node *> path;
	struct node *currentNode = endNode;

	// TODO WTF DO WE IF THERES NO PATH BRAH
	while(currentNode != startNode){
		path.push_back(currentNode);
		currentNode = currentNode->parent;
		cout << "["<< currentNode->posx << ", "<< currentNode->posy << "]" << "\n";

		if (!currentNode) {
			// THERE'S NO PATH THO.
			break;


		}
	}
}

//we check a given node for its neighbors
//since we dont give a shit about diaginal movement, we can ignor diagonal nodes
void GetNeighbours(struct node *current){
	neighbours.clear(); //clear neighbor vector of everything

	//loop through various coordinates around the current node coordinate
	for(int x = -1; x<=1;x++){ 
		for (int y = -1; y <= 1; y++){
			if(abs(x)==abs(y)) //this makes it ignore diagonals 
				continue;

			int checkX = current->posx + x;
			int checkY = current->posy + y;
			//add to neighbors vector if the node is within the bounds of the grid
			if(checkX >= 0 && checkX < gridsizex && checkY >= 0 && checkY<gridsizey){ 
				neighbours.push_back(&grid[checkX][checkY]); //adds to back
			}
		}
	}
	return; 
}

//heres the money maker

void FindPath (int start_x, int start_y, int target_x, int target_y){
	struct node *startNode = get_node(start_x,start_y); //gets node at start location
	struct node *targetNode = get_node(target_x,target_y); //gets node at target location
	//cout << "[1]" << "\n";
	open.push_back(startNode); //add start node to open list
	//cout << open[0]->posy << "\n";

	while(open.size() > 0 ){
		//cout << "[2]" << "\n";
		struct node *currentNode = open[0]; //start analyzing the first node in the open list
		int index = 0; 
		//compare fcosts in all the nodes in open list, choose node with smallest fcost. IF two have equal fcosts, then compare hcosts. Smaller hcost wins
		for (int i =1; i<open.size(); i++){
			if(fcost(open[i]) < fcost(currentNode) || fcost(open[i]) == fcost(currentNode) && open[i]->hcost < currentNode->hcost){
				currentNode = open[i]; //set current node
				index = i;
				
			}
			
		}
		open.erase(open.begin() + index);//remove the current node from open list
		closed.push_back(currentNode); //add current node to closed list
		printnode(currentNode); // print current node, just a reference for you, nothing important
		
		//stops if current node is equal to target node. this means we have completed the path
		if(currentNode == targetNode){
			RetracePath(startNode, targetNode); //start retracing dat path
			return;
		}

		GetNeighbours(currentNode); //find new neighbors 
		
		//go through neighbors, try to find which neighbors to choose to set to current node
		for(int i = 0; i< neighbours.size(); i++){	
			if (!neighbours[i]->walkable || inClosed(neighbours[i])){
				continue;
			}
			//calculate movement cost to neighbor. basically comparing gcosts of all the neigbors and choosing the lowest one
			int newMovementCostToNeighbour = currentNode->gcost + getDistance(currentNode,neighbours[i]);
			if(newMovementCostToNeighbour < neighbours[i]->gcost || !inOpen(neighbours[i])){
				neighbours[i]->gcost = newMovementCostToNeighbour;
				neighbours[i]->hcost = getDistance(neighbours[i], targetNode);
				neighbours[i]->parent = currentNode;
				//Add to open list
				if(!inOpen(neighbours[i]))
				 	open.push_back(neighbours[i]);
			}

		}
	}
} 

//main method to test shit out
int main(){
	gridInit();
	FindPath(0,0,10,10);
	cout << "[""]" << "\n";
	
}
