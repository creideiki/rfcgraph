#include <string>
#include <vector>
#include <sstream>

using namespace std;

class RFC
{
public:
   RFC(string number, string attributes) :
      live(true), _number(number), _attributes(attributes),
   {}

   bool live;
   string _number, _attributes;
   vector<RFC *> children;
   vector<RFC *> parents;
};

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

   vector<RFC *> rfcs(max_rfc_num, NULL);

   while(true)
   {
      string type;
      cin >> type;

      if(type == "node")
      {
         string number;
         cin >> number;

         int num;
         istringstream iss(number);
         number >> num;

         //Läs resten, typ
         cin.getline();
         string attributes;

         RFC *rfc = new RFC(number, attributes);
         rfcs[num] = rfc;
      }
      else if(type == "edge")
      {
      }
      else
      {
         cout << "Unknown record type \"" << type "\"" << endl;
         exit(1);
      }
   }
}
