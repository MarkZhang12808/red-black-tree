/*
13 May 2021 - Mark Zhang - Red Black Tree Part 1: Insertion - creates a red black tree from user input
user can choose to manually enter numbers or create the tree using numbers in a file
REFERENCES: 
  https://courses.cs.duke.edu/spring05/cps130/lectures/skiena.lectures/lecture10.pdf 
  http://pages.cs.wisc.edu/~cs400/readings/Red-Black-Trees/ 
  https://www.cs.auckland.ac.nz/software/AlgAnim/red_black.html
*/
#include <iostream>
#include <cstring>
#include <fstream>

using namespace std;

//each node in a binary tree can be connected to up to 2 other nodes, represented in this node struct

struct node{
    int data;
    bool isRed = true;
    node* left = NULL;
    node* right = NULL;
    node* parent = NULL;
};

void printTree(node* root, int spacing);
void addNode(node* root, node* newNode);
void leftRotate(node*&root, node* n);
void rightRotate(node*&root, node* n);
void recolor(node*&root, node* n);
void insert(node*&root, node*n);
void recolorNode(node* n);
node* searchTree(node* root, int a);

int main(){
    node* root = new node;
    bool rootInitialized = false;
    int blackDepth = 0;
    while(1){
        cout << "Enter a command: [add] an individual number, [read] from a file, [print] tree, or [quit] the program" << endl;
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
        else if(strcmp(input, "lrotate") == 0){
            int temp = 0;
            cout << "enter number of node to rotate" << endl;
            cin >> temp;
            node* n = searchTree(root, temp);
            if(n != NULL){
                leftRotate(root, n);
            }
            else{
                cout << "node could not be found" << endl;
            }
        }
        else if(strcmp(input, "rrotate") == 0){
            int temp = 0;
            cout << "enter number of node to rotate" << endl;
            cin >> temp;
            node* n = searchTree(root, temp);
            if(n != NULL){
                rightRotate(root, n);
            }
            else{
                cout << "node could not be found" << endl;
            }
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

void rightRotate1(node*&root, node* n){
    node* parent = n->parent;
    node* right = n->right;
    node* left = n->left;
    if((n == root)){
        n->left = left->right;
        n->parent = left;
        left->parent = NULL;
        left->right = n;
        root = left;
    }
    /*
    else if(n == root->left){
        n->right = parent;
        parent->left = right;
        n->parent = NULL;
        parent->parent = n;
        root = n;
    }
    */
    else if(n == parent->left){
        parent->left = right;
        n->right = parent;
        n->parent = parent->parent;
        parent->parent = n;
        n->parent->left = n;
    }
    else if(n == parent->right){
        parent->right = left;
        n->parent = left;
        left->parent = parent;
        n->left = left->right;
        left->right = n;
    }
}

void insert(node*& root, node* n){
    addNode(root, n);
    if(n->parent->isRed){
        //red black tree property is violated
        node* uncle = NULL;
        bool uncleIsRight = false;
        while((n != root) && (n->parent->isRed)){
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
            if((uncle != NULL) && (uncle->isRed)){
                recolorNode(n->parent);
                recolorNode(uncle);
                recolorNode(n->parent->parent);
                n = n->parent;
            }
            else if((uncle == NULL) || (uncle->isRed == false)){
                if(uncleIsRight){
                    recolorNode(n->parent);
                    recolorNode(n->parent->parent);
                    rightRotate(root, n->parent->parent);
                }
                else{
                    recolorNode(n->parent);
                    recolorNode(n->parent->parent);
                    leftRotate(root, n->parent->parent);
                }
            }
            node* temp = n->parent;
            n = temp;
        }
    }
    root->isRed = false;
}

void recolorNode(node* n){
    if(n->isRed){
        n->isRed = false;
    }
    else if(n->isRed == false){
        n->isRed = true;
    }
}

node* searchTree(node* root, int a){
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