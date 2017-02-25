#include <StandardCplusplus.h>
#include <system_configuration.h>
#include <unwind-cxx.h>
#include <utility.h>

#include <vector>
//#include <Math.h>
#include <iostream>
using namespace std;



void setup() {
  // put your setup code here, to run once:
   Serial.begin(57600);
   gridInit();
  FindPath(0,0,10,10);

}

void loop() {
  // put your main code here, to run repeatedly:

//    gridInit();
//  FindPath(0,0,10,10);
 // cout << "[""]" << "\n";

}







struct node 
  {
    int posx;
    int posy;
    int gcost;
    int hcost;
    bool walkable;

    struct node* parent;
  };

struct node grid[20][20];
int gridsizex = 20;

int gridsizey = 20;
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
  grid[0][10].walkable = false;
  grid[0][9].walkable = false;
  grid[1][10].walkable = false;
  grid[2][9].walkable = false;


}

//using structures to represent nodes for open and closed lists
vector<struct node *> open; // nodes that need to be evaluated
vector<struct node *> closed; //already been evaluated

vector<struct node *> neighbours;


int abs(int i) {
  return i >= 0 ? i : -i;
}

struct node * get_node(int x, int y){

  return &grid[x][y];

}

//calculate fcost
int fcost(struct node *a)
{
  return a->gcost + a->hcost;
}

int getDistance(struct node *a, struct node *b){
  int dstX = abs(a->posx - b->posx);
  int dstY = abs(a->posy - b->posy);

  return dstX + dstY;
}

bool inClosed(struct node *current)
{
  for(int i = 0; i< closed.size(); i++){
    if (current == closed[i])
      return true;
  }
  return false;
}

bool inOpen(struct node *current)
{
  for(int i = 0; i< open.size(); i++){
    if (current == open[i])
      return true;
  }
  return false;

}



void printnode(struct node* node){ // print this bitch
 Serial.print( node->posx);
  Serial.print(" ");
  Serial.println(node->posy);
}

void RetracePath(struct node *startNode, struct node *endNode){
 // cout << "[retrace]" << "\n";
  vector<struct node *> path;
  struct node *currentNode = endNode;

  // TODO WTF DO WE IF THERES NO PATH BRAH
  while(currentNode != startNode){
    path.push_back(currentNode);
    currentNode = currentNode->parent;
  Serial.print("[");
   Serial.print(currentNode->posx);
    Serial.print(", ");
     Serial.print(currentNode->posy);
      Serial.println("]");

    if (!currentNode) {
      // THERE'S NO PATH THO.
      break;


    }
  }
}
void GetNeighbours(struct node *current){
  neighbours.clear();

  for(int x = -1; x<=1;x++){ 
    for (int y = -1; y <= 1; y++){
      if(abs(x)==abs(y))
        continue;

      int checkX = current->posx + x;
      int checkY = current->posy + y;

      if(checkX >= 0 && checkX < gridsizex && checkY >= 0 && checkY<gridsizey){
        neighbours.push_back(&grid[checkX][checkY]);
      }
    }
  }
  return; 
}
void FindPath (int start_x, int start_y, int target_x, int target_y){
  struct node *startNode = get_node(start_x,start_y);
  struct node *targetNode = get_node(target_x,target_y);
  //cout << "[1]" << "\n";
  open.push_back(startNode);
  //cout << open[0]->posy << "\n";

  while(open.size() > 0 ){
    //cout << "[2]" << "\n";
    struct node *currentNode = open[0];
    int index = 0;

    for (int i =1; i<open.size(); i++){
      if(fcost(open[i]) < fcost(currentNode) || fcost(open[i]) == fcost(currentNode) && open[i]->hcost < currentNode->hcost){
        currentNode = open[i];
        index = i;
        
      }
      
    }
    open.erase(open.begin() + index);
    closed.push_back(currentNode);
    printnode(currentNode);
    if(currentNode == targetNode){
      RetracePath(startNode, targetNode);
      return;
    }

    GetNeighbours(currentNode);
    for(int i = 0; i< neighbours.size(); i++){  
      if (!neighbours[i]->walkable || inClosed(neighbours[i])){
        continue;
      }
      int newMovementCostToNeighbour = currentNode->gcost + getDistance(currentNode,neighbours[i]);
      if(newMovementCostToNeighbour < neighbours[i]->gcost || !inOpen(neighbours[i])){
        neighbours[i]->gcost = newMovementCostToNeighbour;
        neighbours[i]->hcost = getDistance(neighbours[i], targetNode);
        neighbours[i]->parent = currentNode;

        if(!inOpen(neighbours[i]))
          open.push_back(neighbours[i]);
      }

    }
  }
} 



