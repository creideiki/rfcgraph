#!/usr/bin/gawk -f
#
# Turns the RFC index into a Graphviz .dot graph.
#
# Distributed under the terms of the GPL. See the file COPYING for
# details.
#
# $HeadURL$
# $Id$

BEGIN {
   node_color["experimental"] = "crimson";
   node_color["proposed standard"] = "orange";
   node_color["draft standard"] = "yellow";
   node_color["internet standard"] = "green";
   node_color["standard"] = "green";
   node_color["informational"] = "deepskyblue";
   node_color["obsolete"] = "red";
   node_color["unknown"] = "gray";

   edge_attrs["update"] = " [label=\"Update\",color=blue]";
   edge_attrs["obsolete"] = " [label=\"Obsolete\",color=red]";

   in_rfc_list = 0;
}

# Beginning of RFC
/^[[:digit:]][[:digit:]][[:digit:]][[:digit:]]/ {
   in_rfc_list = 1;
   description = $0;
   next;
}

# Description lines: concatenate the line to the description,
# collapsing runs of white space.
in_rfc_list {
   description = description gensub(/  +/, " ", "g", $0);
}

# Empty line, end of RFC
/^$/ && in_rfc_list {
   end_of_rfc(description);
   description = "";
}

END {
   end_of_rfc(description);
   description = "";
}

function get_parenthesized_rfc_numbers(description, relation, array,   left, right, rfcs, i)
{
   if(left = index(description, "(" relation ))
   {
      right = index(substr(description, left), ")");
      rfcs = substr(description,
                    left + length(relation) + 1,
                    right - length(relation) - 2);
      gsub(/ |(RFC)/, "", rfcs);
   }
   split(rfcs, array, ",");
   for(i in array)
   {
      if(!match(array[i], /^[[:digit:]][[:digit:]][[:digit:]][[:digit:]]$/))
         array[i] = 0;
   }
}

function get_status(description,   left, right)
{
   if(left = index(description, "(Status: "))
   {
      right = index(substr(description, left), ")");
      status = substr(description,
                      left + 9,
                      right - 10);
      return status;
   }
   else
   {
      return "UNKNOWN";
   }
}

function end_of_rfc(description,   number, color, label)
{
   number = substr(description, 1, 4);

   color = node_color[tolower(get_status(description))];
   if(color == "")
   {
      color = node_color["unknown"];
   }

   label = number "\\n" substr(description, 6);
   gsub(/ \((Format:|Obsoletes|Obsoleted by|Updates|Updated by|Also|Status:|DOI:) [^)]*\)/, "", label);
   gsub(/ $/, "", label);
   gsub(/\"/, "\\\"", label);

   printf("node %s [color=%s,label=\"%s\"]\n", number, color, label);

   get_parenthesized_rfc_numbers(description, "Updates", array);
   for(updated in array)
   {
      if(array[updated] != 0)
         print("edge " array[updated] " -> " number edge_attrs["update"]);
   }

   get_parenthesized_rfc_numbers(description, "Obsoletes", array);
   for(obsoleted in array)
   {
      if(array[obsoleted] != 0)
         print("edge " array[obsoleted] " -> " number edge_attrs["obsolete"]);
   }
}
