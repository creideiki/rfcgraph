#include <string>
#include <vector>
#include <sstream>
#include <iostream>

using namespace std;

class RFC;

vector<RFC *> rfcs;

class RFC
{
public:
   RFC(int number, string attributes) :
      live(true), _number(number), _attributes(attributes)
   {}

   void add_child(int number, string attributes)
   {
      children.push_back(pair<RFC *, string>(rfcs[number], attributes));
   }

   void add_parent(int number)
   {
      parents.push_back(rfcs[number]);
   }

   bool live;
   int _number;
   string _attributes;
   vector< pair<RFC *, string> > children;
   vector<RFC *> parents;
};

ostream& operator<<(ostream &ost, const RFC &rfc)
{
   return ost;
}

int main(int argc, char *argv[])
{
   if(argc != 2)
   {
      cout << "Usage: " << argv[0] << " [max_rfc_num]" << endl;
      exit(2);
   }

   // Read max RFC number from command line argument
   int max_rfc_num;
   istringstream iss(argv[1]);
   iss >> max_rfc_num;

   rfcs.resize(max_rfc_num + 1, NULL);

   while(!cin.eof())
   {
      string type;
      cin >> type;

      if(type == "node")
      {
         string number;
         cin >> number;

         int num;
         istringstream iss(number);
         iss >> num;

         string attributes;
         getline(cin, attributes);

         RFC *rfc = new RFC(num, attributes);
         rfcs[num] = rfc;
      }
      else if(type == "edge")
      {
         string from, arrow, to;
         cin >> from >> arrow >> to;

         int parent, child;
         istringstream iss(from);
         iss >> parent;
         istringstream iss2(to);
         iss2 >> child;

         if(!rfcs[parent])
         {
            cout << "Edge from " << from << " to " << to << " has a non-existant parent." << endl;
            exit(1);
         }

         if(!rfcs[child])
         {
            cout << "Edge from " << from << " to " << to << " has a non-existant child." << endl;
            exit(1);
         }

         string attributes;
         getline(cin, attributes);

         rfcs[parent]->add_child(child, attributes);
         rfcs[child]->add_parent(parent);
      }
      else
      {
         if(cin.eof())
            break;
         cout << "Unknown record type \"" << type << "\"" << endl;
         exit(1);
      }
   }

   for(int i = 0; i <= max_rfc_num; ++i)
   {
      if(!rfcs[i]->live)
         continue;

      
   }
}
