# libptrie
A Minimum and Efficient library of Patricia Trie in implemented "C".

# Requirements
- C99 Compiler(``cc`` or ``gcc`` or ``clang``)
- The archiver command "ar" which is including Unix utility 
- libbsd(Only for Linux).

# Usage
This library basically stores a pair of key and value to LEAF node.
Type of key is "char" and type of value is voluntary which specified macro named "DATA" in ptrie.h.  
You can define to use ENTRY type which you want to store a pair of KEY and VALUE.  
Additionally, you know what became of result of manipulate such as insert() or delete() to Patricia through ENTRY type.    
ENTRY type consists three-entity such as "key" and "data" and "status".  
The last member of "status" means condition which result of manipulation such as insert() or search() and delete().  

If you try to use for practical purpose, See [test](./test).

# Build
```
make ptrie
cd ./test
cc -o test_insert test_insert.c -L ../ -lptrie
```

# LICENSE
See [License](./LICENSE).


