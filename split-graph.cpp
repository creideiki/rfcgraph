// Copyright (C) 2005 Karl-Johan Karlsson <creideiki@ferretporn.se>
//  
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//  
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//  
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

// $HeadURL$
// $Id$

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <functional>
#include <iomanip>
#include <fstream>

using namespace std;

class RFC;

vector<RFC *> rfcs;

class RFC
{
public:
   RFC(int _number, string _attributes) :
      live(true), number(_number), attributes(_attributes)
   {}

   void add_child(int _number, string _attributes)
   {
      children.push_back(pair<RFC *, string>(rfcs[_number], _attributes));
   }

   void add_parent(int _number)
   {
      parents.push_back(rfcs[_number]);
   }

   bool live;
   int number;
   string attributes;
   vector< pair<RFC *, string> > children;
   vector<RFC *> parents;

   friend ostream& operator<<(ostream &ost, const RFC &rfc);
};

ostream& operator<<(ostream &ost, const RFC &rfc)
{
   ost << setfill('0') << setw(4) << rfc.number << rfc.attributes;
   return ost;
}

ofstream *file;
ofstream *index_html;

void visit(int number);

void visit_child(int parent, pair<RFC *, string> edge)
{
   *file << setfill('0') << setw(4)
         << parent << " -> "
         << setfill('0') << setw(4)
         << edge.first->number << edge.second << "\n";
   visit(edge.first->number);
}

void visit_parent(RFC *parent)
{
   visit(parent->number);
}

void visit(int number)
{
   RFC *rfc = rfcs[number];

   if(!rfc->live)
      return;

   rfc->live = false;

   *index_html << "<tr><td>"
               << "<a href=\"http://www.ietf.org/rfc/rfc"
               << setfill('0') << setw(4) << rfc->number
               << ".txt\">"
               << setfill('0') << setw(4) << rfc->number
               << "</a></td><td>" << rfc->attributes << "</td></tr>\n";

   *file << *rfc << "\n";

   for_each(rfc->children.begin(), rfc->children.end(), bind1st(ptr_fun(visit_child),  number));
   for_each(rfc->parents.begin(),  rfc->parents.end(),  &visit_parent);
}

int main(int argc, char *argv[])
{
   if(argc != 2)
   {
      cout << "Usage: " << argv[0] << " [max_rfc_num]" << endl;
      exit(2);
   }

   int max_rfc_num;
   istringstream iss(argv[1]);
   iss >> max_rfc_num;

   rfcs.resize(max_rfc_num + 1, NULL);

   while(cin)
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
         if(!cin)
            break;
         cout << "Unknown record type \"" << type << "\"" << endl;
         exit(1);
      }
   }

   index_html = new ofstream("index.html");
   *index_html << "<html><head><title>RFC index</title></head><body>\n";

   for(int i = 0; i <= max_rfc_num; ++i)
   {
      if(!rfcs[i] || !rfcs[i]->live)
         continue;

      ostringstream oss;
      oss << setfill('0') << setw(4) << i;
      file = new ofstream((oss.str() + ".dot").c_str());

      *index_html << "<a href=\"" << (oss.str() + ".png") << "\">Image</a>\n"
                  << "<table>\n";

      *file << "digraph rfc" << i << "\n";
      *file << "{" << "\n";
      *file << "rankdir=LR;" << "\n";
      *file << "node [shape=box,style=filled];\n";

      visit(i);

      *file << "}" << "\n" << endl;

      *index_html << "</table>\n";

      file->close();
      delete file;
   }

   *index_html << "</body></html>" << endl;
   index_html->close();
   delete index_html;
}
