#include <iostream>
#include <vector>
#include <fstream>
#include <stdlib.h>

int m; // order

using namespace std;

std::ofstream out ("output.txt", std::ofstream::out);

//structure that defines the node
struct node
{
  vector<pair<double,vector<string> > > leaf;
  vector<double> internal_node;
  vector<node*> children;
  bool isRoot=false;
  bool isLeaf=false;
  bool isEmpty=true;
  node *prev, *next, *parent;
};

//class that encapsulates the B+tree fucntions
class tree
{
  public:
  node *root;
  int m;

  node *create_node();
  void initialize(int order);
  void insert(double key, string value);
  node *search(double key, double key2, bool isKeyRange, bool isInsert);
  node *sorted_insert(node *temp,double key, string value);
  void check_and_balance(node *current_node);
  void display(node *roott);
};

//optional display function to do preorder display
void tree::display(node *roott)
{
  if(roott==NULL)
    cout<<"empty tree"<<endl;
  if(!roott->isLeaf)
  {
    if(roott==NULL)
      cout<<"empty"<<endl;
    else if(roott!=NULL)
    {
      for(int i=0; i<roott->internal_node.size(); i++)
      cout<<"non leaf"<<roott->internal_node[i]<<" ";
      cout<<endl;
      for(int i=0; i<roott->children.size(); i++)
        display(roott->children[i]);
    }
  }
  else
  {
    for(int i=0;i<roott->leaf.size();i++)
      cout<<"Leaf "<<roott->leaf[i].first<<" ";
      cout<<"\n";
  }
}

//creates memory for a new node
node *tree::create_node()
{
    node *temp;
    temp = new(struct node);
    if(temp!=NULL)
    {
        temp->prev = NULL;
        temp->next = NULL;
        temp->parent = NULL;
        return temp;
    }
    else
    {
        cout<< " Failed to create Memory "<<endl;
        exit(0);
    }
}

///////////////////////

//initializes the tree with order m and empty root
void tree::initialize( int order )
{
  root = create_node();
  m = order;
}

///////////////////////


//checks for imbalanced nodes and balances the tree accordingly
void tree::check_and_balance(node *current_node)
{
  bool imbalance =false;
  node *check_node;
  //find imbalance first
  if(current_node->isLeaf)
  {
    if(current_node->leaf.size()>m-1)
        imbalance = true;
  }
  else
  {
    if(current_node->internal_node.size()>m-1)
        imbalance = true;
  }
  if(imbalance)
  {
    //if imbalance is in the root node
    if(root == current_node)
    {
    if(current_node->parent==NULL)
    {
      if(current_node->isLeaf)
      {
        int mid = current_node->leaf.size()/2;
        node *parent_node = create_node(), *split_node = create_node();
        split_node->isLeaf=true;
        split_node->isEmpty=false;
        split_node->next=current_node->next;
        split_node->prev=current_node;
        if(current_node->next!=NULL)
        current_node->next->prev=split_node;
        current_node->next=split_node;
        split_node->parent=parent_node;
        split_node->leaf.insert(split_node->leaf.begin(), current_node->leaf.begin()+mid,current_node->leaf.end());
        parent_node->internal_node.insert(parent_node->internal_node.begin(),current_node->leaf[mid].first);
        parent_node->isRoot=true;
        current_node->isRoot=false;
        parent_node->isEmpty=false;
        parent_node->children.push_back(current_node);
        parent_node->children.push_back(split_node);
        current_node->parent=parent_node;
        current_node->leaf.erase(current_node->leaf.begin()+mid, current_node->leaf.end());
        root = parent_node;
    }
    else
    {
      int mid = current_node->internal_node.size()/2;
      node *parent_node = create_node(), *split_node = create_node();
      split_node->isEmpty=false;
      split_node->parent=parent_node;
      split_node->internal_node.insert(split_node->internal_node.begin(), current_node->internal_node.begin()+mid+1,current_node->internal_node.end());
      parent_node->internal_node.insert(parent_node->internal_node.begin(),current_node->internal_node[mid]);
      parent_node->isRoot=true;
      current_node->isRoot=false;
      parent_node->isEmpty=false;
      parent_node->children.push_back(current_node);
      parent_node->children.push_back(split_node);
      current_node->parent=parent_node;
      current_node->internal_node.erase(current_node->internal_node.begin()+mid, current_node->internal_node.end());
      root = parent_node;
      split_node->children.insert(split_node->children.begin(), current_node->children.begin()+mid+1,current_node->children.end());
      current_node->children.erase(current_node->children.begin()+mid+1, current_node->children.end());
      for(int i=0; i<split_node->children.size(); i++)
      {
        split_node->children[i]->parent = split_node;
      }
    }
    }
  }
  //if imbalance is in non root node
  else
  {
      if(current_node->isLeaf)
      {
        int mid = current_node->leaf.size()/2;
        node *split_node = create_node();
        split_node->isEmpty=false;
        split_node->isLeaf=true;
        split_node->next=current_node->next;
        split_node->prev=current_node;
        if(current_node->next!=NULL)
        current_node->next->prev=split_node;
        current_node->next=split_node;
        split_node->parent=current_node->parent;
        split_node->leaf.insert(split_node->leaf.begin(), current_node->leaf.begin()+mid,current_node->leaf.end());
        current_node->leaf.erase(current_node->leaf.begin()+mid, current_node->leaf.end());
        //current_node->parent->internal_node.push_back(current_node->leaf[mid].first);

        bool flag = false;
        int i;
        for(i=0; i<current_node->parent->internal_node.size(); i++)
        {
          if(current_node->parent->internal_node[i]>current_node->leaf[mid].first)
            {
              current_node->parent->internal_node.insert(current_node->parent->internal_node.begin()+i,current_node->leaf[mid].first);
              current_node->parent->children.insert(current_node->parent->children.begin()+i+1,split_node);
              flag = true;
              break;
            }
        }
        if(!flag)
        {
          current_node->parent->internal_node.push_back(current_node->leaf[mid].first);
          current_node->parent->children.push_back(split_node);
          flag = true;
        }

        //current_node->parent,current_node->leaf[mid].first, "");

        check_node=current_node->parent;
      }
      else
      {
        int mid = current_node->internal_node.size()/2;
        node *split_node = create_node();
        split_node->isEmpty=false;
        split_node->parent=current_node->parent;
        split_node->internal_node.insert(split_node->internal_node.begin(), current_node->internal_node.begin()+mid+1,current_node->internal_node.end());
        current_node->internal_node.erase(current_node->internal_node.begin()+mid+1, current_node->internal_node.end());
        //current_node->parent->internal_node.push_back(current_node->internal_node[mid]);
        //current_node->parent->children.push_back(split_node);
        bool flag = false;
        int i;
        for(i=0; i<current_node->parent->internal_node.size(); i++)
        {
          if(current_node->parent->internal_node[i]>current_node->internal_node[mid])
            {
              current_node->parent->internal_node.insert(current_node->parent->internal_node.begin()+i,current_node->internal_node[mid]);
              current_node->parent->children.insert(current_node->parent->children.begin()+i+1,split_node);
              flag = true;
              break;
            }
        }
        if(!flag)
        {
          current_node->parent->internal_node.push_back(current_node->internal_node[mid]);
          current_node->parent->children.push_back(split_node);
          flag = true;
        }

        split_node->children.insert(split_node->children.begin(),current_node->children.begin()+mid+1,current_node->children.end());
        current_node->children.erase(current_node->children.begin()+mid+1,current_node->children.end());
        check_node=current_node->parent;
        for(int i=0; i<split_node->children.size(); i++)
        {
          split_node->children[i]->parent = split_node;
        }
      }
      check_and_balance(check_node);// recursive call to see if the parent node is again imbalanced
  }

}
}

///////////////////////

node *tree::search(double key, double key2=0.0, bool isKeyRange = false, bool isInsert = false)
{
  node *current_node = root, *outp = create_node();
  if(current_node->isEmpty)
  {
    return outp;
  }
  while(!current_node->isLeaf)
  {
      bool flag = false;
      for(int i=0; i<current_node->internal_node.size(); i++)
      {
        if(key<current_node->internal_node[i])
        {
          current_node=current_node->children[i];
          flag = true;
          break;
        }
      }
      if(!flag)
      {
        current_node=current_node->children[current_node->children.size()-1];
      }
  }
  if(isInsert) // for search within an insert call
  {
    bool flag = false;
    for( int i=0; i<current_node->leaf.size(); i++)
    {
      if(current_node->leaf[i].first==key)
        {
          // key found
          return current_node;
        }
    }
    return outp;
  }
  if(!isKeyRange) // for single key search
  {
  bool flag = false;
  for( int i=0; i<current_node->leaf.size(); i++)
  {
    if(current_node->leaf[i].first==key)
      {
        // key found
        for(int j=0; j<current_node->leaf[i].second.size()-1; j++)
          out << current_node->leaf[i].second[j]<<", ";
        out << current_node->leaf[i].second[current_node->leaf[i].second.size()-1];
        //out<<"\n";
        flag = true;
        break;
      }
  }
    if(!flag)
    {
      out<<"NULL";
    }
    return outp;
  }
  if(isKeyRange) // for serach in a key range
  {
    bool flag2 = false;
    while(current_node!=NULL)
    {
      bool flag = false;
      for(int i=0; i<current_node->leaf.size(); i++)
      {
        flag = true;
        if(current_node->leaf[i].first>=key && current_node->leaf[i].first<=key2)
        {
        for(int j=0; j<current_node->leaf[i].second.size()-1; j++)
        {
          out<<"("<<current_node->leaf[i].first<<","<<current_node->leaf[i].second[j]<<")"<<", "; //except the last value
        }
        out<<"("<<current_node->leaf[i].first<<","<<current_node->leaf[i].second[current_node->leaf[i].second.size()-1]<<")"; // last value is outputted seperately to avoid the extra comma and space at the end
        }
        else if(current_node->leaf[i].first>key2)
        {
          flag2=true;
          break;
        }
      }
      if(flag2)
        break;
      current_node=current_node->next;
    }
    return outp;
  }

}

///////////////////////

//insert the key value inside the node in a sorted position
node *tree::sorted_insert(node *temp, double key, string value)
{
  if(temp->isLeaf)
  {
  bool flag = false;
  for(int i=0; i<temp->leaf.size(); i++)
  {
    if(temp->leaf[i].first>key)
      {
        vector<string> tempp;
        tempp.push_back(value);
        temp->leaf.insert(temp->leaf.begin()+i, make_pair(key,tempp));
        flag = true;
        break;
      }
  }
  if(!flag)
  {
    vector<string> tempp;
    tempp.push_back(value);
    temp->leaf.push_back(make_pair(key,tempp));
    flag = true;
  }
return temp;
}
}

// main insert function to insert the given key value pair. Performs sorted and insert
// and checks for imbalance in the tree and corrects them
void tree::insert(double key, string value)
{
  node *temp = search(key,key,false,true);
  if(!temp->isEmpty)
  {
    for(int i=0; i<m; i++)
    {
      if(temp->leaf[i].first==key)
      {
        temp->leaf[i].second.push_back(value);
        break;
      }
    }
  }
  else
  {
      node *current_node = root;
      if(current_node->isEmpty)
        {
          vector<string> dummy;
          dummy.push_back(value);
          current_node->leaf.push_back(make_pair(key,dummy));
          current_node->isRoot=true;
          current_node->isLeaf=true;
          current_node->isEmpty=false;
          root = current_node;
        }
        else
        {
      while(!current_node->isLeaf)
      {
          bool flag = false;
          for(int i=0; i<current_node->internal_node.size(); i++)
          {
            if(key<current_node->internal_node[i])
            {
              current_node=current_node->children[i];
              flag = true;
              break;
            }
          }
          if(!flag)
          {
            current_node=current_node->children[current_node->children.size()-1];
          }
      }
      current_node = sorted_insert(current_node, key, value);
      check_and_balance(current_node);
    }
  }
}

int main( int argc, char* argv[] )
{
  tree bplus; //object for the Bplus tree
  string input_file = string(argv[1]);
  std::ifstream input;
  input.open(input_file.c_str(),ifstream::in);
  std::string line;
  getline(input,line);
  m = atoi(line.c_str()); // Read the order
  bplus.initialize(m);
  double key, key2;
  string value;
  while( getline( input, line ) ) // Read every line
  {
    if(line[0]=='I') // if insert
      {
        key = atof((line.substr(line.find("(")+1,line.find(",")-line.find("(")-1)).c_str());
        value = line.substr(line.find(",")+1,line.find(")")-line.find(",")-1);
        bplus.insert(key,value);
      }
    else if(line[0]=='S' && line.find(",")!=string::npos) // if search in a range
    {
      key = atof((line.substr(line.find("(")+1,line.find(",")-line.find("(")-1)).c_str());
      key2 = atof((line.substr(line.find(",")+1,line.find(")")-line.find(",")-1)).c_str());
      bplus.search(key,key2,true,false);
      out<<endl;
    }
    else // if search with single key
    {
      key = atof((line.substr(line.find("(")+1,line.find(")")-line.find("(")-1)).c_str());
      bplus.search(key,false,false);
      out<<endl;
    }
    //
  }
  //bplus.display(bplus.root);             //display the tree in preorder format
  input.close();
  out.close();
}
