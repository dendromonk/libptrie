# libptrie - A Minimum and Efficient library of Patricia in implemented "C"

# Requirements
This library needs only Standard library for C99 and libbsd for Linux.

# Usage
Basically this structure of patricia stores a pair of KEY-VALUE to LEAF node.
Type of KEY is "char" and type of VALUE is voluntary which specified  macro named "DATA" in ptrie.h.　　
You can define to use ENTRY type which you want to store a pair of KEY and VALUE.　　
Additionally, you know what became of result of manipulate such as insert() or delete() to Patricia through ENTRY type.　　  
ENTRY type consists three-entity such as "key" and "data" and "status".　　
The last member of "status" means condition which result of manipulation such as insert() or search() and delete().　　

If you try to use for practical purpose, See [test](./test).

