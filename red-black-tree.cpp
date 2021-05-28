/*
13 May 2021 - Mark Zhang - Red Black Tree Part 1: Insertion - creates a red black tree from user input
user can choose to manually enter numbers or create the tree using numbers in a file
REFERENCES:
  Videos on Canvas
  https://courses.cs.duke.edu/spring05/cps130/lectures/skiena.lectures/lecture10.pdf 
  http://pages.cs.wisc.edu/~cs400/readings/Red-Black-Trees/ 
  https://www.cs.auckland.ac.nz/software/AlgAnim/red_black.html
*/
#include <iostream>
#include <cstring>
#include <fstream>

using namespace std;

//each node in a binary tree can be connected to up to 2 child nodes and a parent node, represented in this node struct

struct node{
    int data;
    bool isRed = true;
    node* left = NULL;
    node* right = NULL;
    node* parent = NULL;
};

//function declarations

void printTree(node* root, int spacing);
void addNode(node* root, node* newNode);
void leftRotate(node*&root, node* n);
void rightRotate(node*&root, node* n);
void insert(node*&root, node*n);
node* searchTree(node* root, int a);

int main(){
    node* root = new node;
    bool rootInitialized = false;
    while(1){ //takes commands until user quits
        cout << "Enter a command: [add] a series of numbers, [read] from a file, [print] tree, [search] for a node, or [quit] the program" << endl;
        char* input = new char[10];
        int* array = new int[100];
        int index = 0;
        bool addCommand = false;
        cin.clear();
        cin.getline(input, 10);
        if(strcmp(input, "add") == 0){
            char* temp = new char[500];
            cout << "enter integers separated with spaces" << endl;
            cin.getline(temp, 500);
            char* token = strtok(temp, " ");
            while(token != NULL){
                array[index] = atoi(token);
                token = strtok(NULL, " ");
                index++;
            }
            addCommand = true;
        }
        else if(strcmp(input, "read") == 0){
            cout << "enter file name (ex: numbers.txt) containing space-separated numbers" << endl;
            string fileName;
            cin.clear();
            cin >> fileName;
            fstream file;
            int temp = 0;
            file.open(fileName, ios::in);
            while(file >> temp){
                array[index] = temp;
                index++;
            }
            addCommand = true;
        }
        else if(strcmp(input, "print") == 0){
            printTree(root, 0);
        }
        else if(strcmp(input, "search") == 0){
            int temp = 0;
            cout << "enter number to search" << endl;
            cin >> temp;
            node* n = searchTree(root, temp);
            if(n == NULL){
                cout << "node could not be found" << endl;
            }
            else{
                cout << "node found: " << n->data << endl;
            }
        }
        else if(strcmp(input, "quit") == 0){
            cout << "quitting..." << endl;
            exit(0);
        }
        else{
            cout << "please enter valid command" << endl;
        }
        if(addCommand == true){
            int i = 0;
            if(rootInitialized == false){ 
                //CASE 1: tree is empty, root is set as a black node
                root->data = array[0];
                root->isRed = false;
                i++;
                rootInitialized = true;
            }
            while(i < index){
                node* n = new node;
                n->data = array[i];
                //insert(root, n);
                insert(root, n);
                i++;
            }
            delete [] array;
        }
    }
    return 0;
}

void printTree(node* root, int spacing){
    //recursively prints the red black tree, r denotes a red node, b denotes a black node
    if(root == NULL){
        return;
    }
    spacing += 7;
    printTree(root->right, spacing);
    cout << endl;
    for(int i = 7; i < spacing; i++){
        cout << " ";
    }
    if(root->isRed){
        cout << "r-";
    }
    else{
        cout << "b-";
    }
    cout << root->data << '\n';
    printTree(root->left, spacing);
}

void addNode(node* root, node* n){
    //function that adds a new node in a way that keeps the in-order traversal property of a binary search tree
    //but does not update the node's color to maintain a red-black tree
    node* traverse = root;
    while(1){
        //traverses the tree until it reaches the end of the appropriate branch and adds the new node
        if(n->data >= traverse->data){
            if(traverse->right == NULL){
                traverse->right = n;
                n->parent = traverse;
                return;
            }
            else{
                traverse = traverse->right;
            }
        }
        else if(n->data < traverse->data){
            if(traverse->left == NULL){
                traverse->left = n;
                n->parent = traverse;
                return;
            }
            else{
                traverse = traverse->left;
            }
        }
    }
}

void leftRotate(node*& root, node* n){
    //function for left rotation around node n
    cout << "left rotation started" << " on node: " << n->data << endl;
    node* right = n->right;
    node* temp = right->left;
    node* parent = n->parent;
    if(n == root){
        cout << "root changed" << endl;
        root = right;    
    }
    else if(n == n->parent->left){
        n->parent->left = right;
    }
    else if(n == n->parent->right){
        n->parent->right = right;
    }
    n->parent = right;
    n->right = temp;
    right->left = n;
    right->parent = parent;
    cout << "left rotation completed" << endl;
}

void rightRotate(node*&root, node* n){
    //function for right rotation around node n
    cout << "right rotation started" << " on node: " << n->data << endl;
    node* left = n->left;
    node* temp = left->right;
    node* parent = n->parent;
    if(n->parent == NULL){
        cout << "root changed" << endl;
        root = left;
        root->parent = NULL;
    }
    else if(n == n->parent->left){
        n->parent->left = left;
    }
    else if(n == n->parent->right){
        n->parent->right = left;
    }
    n->parent = left;
    n->left = temp;
    left->parent = parent;
    left->right = n;
    cout << "right rotation completed" << endl;
}

void insert(node*& root, node* n){
    //function that adds the node and then maintains the properties of the red-black tree
    addNode(root, n);
    //CASE 2: parent is black, node is red, nothing happens
    if(n->parent->isRed){
        //red black tree property is violated, checks for cases 3, 4, and 5
        node* uncle = NULL;
        bool uncleIsRight = false;
        while((n != root) && (n->parent->isRed)){ //loop goes up the tree until it reaches the root and if the node it is examining has red parent
            //finds the uncle node and its location from the grandparent
            if(n->parent != root && n != root){
                if(n->parent == n->parent->parent->left){
                    uncle = n->parent->parent->right;
                    uncleIsRight = true;
                }
                else{
                    uncle = n->parent->parent->left;
                    uncleIsRight = false;
                }
            }
            else if(n == root->left){
                uncleIsRight = true;
            }
            else if(n == root->right){
                uncleIsRight = false;
            }

            //CASE 3: uncle is red
            if((uncle != NULL) && (uncle->isRed)){
                n->parent->isRed = false;
                uncle->isRed = false;
                n->parent->parent->isRed = true;
                n = n->parent->parent;
            }
            else if((uncle == NULL) || (uncle->isRed == false)){
                if(((n == n->parent->right) && (n->parent == n->parent->parent->left)) || ((n == n->parent->left) && (n->parent == n->parent->parent->right))){
                    //CASE 4: uncle is black, node and its parent are not in-line
                    if(n == n->parent->left){
                        n = n->parent;
                        rightRotate(root, n);
                    }
                    else{
                        n = n->parent;
                        leftRotate(root, n);
                    }
                }
                n->parent->parent->isRed = true;
                n->parent->isRed = false;
                    //CASE 5: uncle is black and node and its parent are aligned
                if(uncleIsRight){
                    rightRotate(root, n->parent->parent);
                }
                else{
                    leftRotate(root, n->parent->parent);
                }
            }
        }
    }
    root->isRed = false;
}

node* searchTree(node* root, int a){
    //function can be called to check the existence of a node in the tree based on its integer value
    //if the node exists, it is returned, if not, the function returns NULL
    node* traverse = new node;
    traverse = root;
    while(1){
        if(a > traverse->data){
            if(traverse->right == NULL){
                return NULL;
            }
            traverse = traverse->right;
        }
        else if(a < traverse->data){
            if(traverse->left == NULL){
                return NULL;
            }
            traverse = traverse->left;
        }
        else if(traverse->data == a){
            return traverse;
        }
    }
}