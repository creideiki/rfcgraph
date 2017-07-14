// Copyright (C) 2005-2017 Karl-Johan Karlsson <creideiki@ferretporn.se>
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

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <functional>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <ctime>

using namespace std;

class RFC;

vector<RFC *> rfcs;

string left_delimiter = "\\n";
string right_delimiter = "\"]";

class RFC
{
public:
   RFC(int _number, string _attributes) :
      live(true), number(_number), attributes(_attributes)
   {
      if(title().find("Not Issued") != string::npos)
         live = false;
   }

   void add_child(int _number, string _attributes)
   {
      children.push_back(pair<RFC *, string>(rfcs[_number], _attributes));
   }

   void add_parent(int _number)
   {
      parents.push_back(rfcs[_number]);
   }

   string title() const
   {
      int full_title_start = attributes.find(left_delimiter) + left_delimiter.length();
      int full_title_end   = attributes.rfind(right_delimiter);
      string full_title = attributes.substr(full_title_start, full_title_end - full_title_start);

      int backslash_position = 0;
      while((backslash_position = full_title.find('\\', backslash_position)) != string::npos)
         full_title.erase(backslash_position, 1);

      return full_title;
   }

   string filled_attributes() const
   {
      string filled = attributes;
      int title_start = filled.find(left_delimiter) + left_delimiter.length();
      int position = title_start;
      int start_of_line = position;

      while((position = filled.find(" ", position + 1)) != string::npos)
      {
         int next_word_break = filled.find(" ", position + 1);
         if(next_word_break == string::npos)
            next_word_break = filled.length();

         if(next_word_break - start_of_line > 25)
         {
            filled.replace(position, 1, "\\n");
            start_of_line = position;
         }
      }

      return filled;
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
   ost << setfill('0') << setw(4) << rfc.number << rfc.filled_attributes();
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
               << "<a href=\"https://tools.ietf.org/html/rfc"
               << rfc->number << "\">"
               << setfill('0') << setw(4) << rfc->number
               << "</a></td><td>" << rfc->title() << "</td></tr>\n";

   *file << *rfc << "\n";

   for_each(rfc->children.begin(), rfc->children.end(), bind1st(ptr_fun(visit_child),  number));
   for_each(rfc->parents.begin(),  rfc->parents.end(),  &visit_parent);
}

int main(int argc, char *argv[])
{
   if(argc != 2)
   {
      cerr << "Usage: " << argv[0] << " <max_rfc_num>" << endl;
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
            cerr << "Edge from " << from << " to " << to << " has a non-existant parent." << endl;
            exit(1);
         }

         if(!rfcs[child])
         {
            cerr << "Edge from " << from << " to " << to << " has a non-existant child." << endl;
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
         cerr << "Unknown record type \"" << type << "\"" << endl;
         exit(1);
      }
   }

   index_html = new ofstream("index.html");
   if(!index_html || !*index_html)
   {
      cerr << "Unable to write \"index.html\"." << endl;
      exit(1);
   }
   *index_html << "<html><head><title>RFC index</title>\n"
               << "<style type=\"text/css\">\n"
               << "   table\n"
               << "   {\n"
               << "      border: 1px solid black;\n"
               << "      margin-top: 1em;\n"
               << "   }\n"
               << "\n"
               << "   th\n"
               << "   {\n"
               << "      text-align: left;\n"
               << "   }\n"
               << "</style>\n"
               << "</head><body>\n"
               << "<p>Last updated: ";
   time_t t = time(NULL);
   char time_string[15];
   if(strftime(time_string, 15, "%F", localtime(&t)) == 0)
   {
      cerr << "Unable to write update time." << endl;
      exit(1);
   }
   *index_html << time_string
               << "</p>\n"
               << "<p><a href=\"legend.png\">Legend</a>\n"
               << "   <a href=\"https://www.ferretporn.se/subversion/rfcgraph/\">Source code</a></p>\n";

   for(int i = 0; i <= max_rfc_num; ++i)
   {
      if(!rfcs[i] || !rfcs[i]->live)
         continue;

      ostringstream oss;
      oss << setfill('0') << setw(4) << i;
      file = new ofstream((oss.str() + ".dot").c_str());

      *index_html << "<table>\n"
                  << "<tr><th colspan=\"2\"><a href=\"" << (oss.str() + ".png") << "\">Relationship graph</a></th></tr>\n";

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
