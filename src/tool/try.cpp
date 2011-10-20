/* try.cpp  -  for trying out some language features....
 *             scons will create the binary bin/try
 *
 */ 

// 8/07  - how to control NOBUG??
//         execute with   NOBUG_LOG='ttt:TRACE' bin/try
// 1/08  - working out a static initialisation problem for Visitor (Tag creation)
// 1/08  - check 64bit longs
// 4/08  - comparison operators on shared_ptr<Asset>
// 4/08  - conversions on the value_type used for boost::any
// 5/08  - how to guard a downcasting access, so it is compiled in only if the involved types are convertible
// 7/08  - combining partial specialisation and subclasses 
// 10/8  - abusing the STL containers to hold noncopyable values
// 6/09  - investigating how to build a mixin template providing an operator bool()
// 12/9  - tracking down a strange "warning: type qualifiers ignored on function return type"
// 1/10  - can we determine at compile time the presence of a certain function (for duck-typing)?
// 4/10  - pretty printing STL containers with python enabled GDB?
// 1/11  - exploring numeric limits
// 1/11  - integer floor and wrap operation(s)
// 1/11  - how to fetch the path of the own executable -- at least under Linux?
// 10/11 - simple demo using a pointer and a struct


#include <iostream>

using std::cout;



/**
 * custom datastructure
 * holding a constant char array with "hey"
 */
struct MyStruct
  {
    char data_[3];
    const int length_;
    
    MyStruct()
      : length_(3)
      { 
        const char *tmp = "hey";
        for (int i=0; i<length_; ++i)
          data_[i] = *(tmp+i);
      }
  };


// define a global variable holding a MyStruct
MyStruct theStruct;



void
printMyStruct(MyStruct* myPointer)
  {
    for (int i=0; i < myPointer->length_; ++i)
      cout << myPointer->data_[i];
    
    cout << "\n";
  }



int 
main (int, char**) //(int argc, char* argv[])
  {
    printMyStruct (&theStruct);
    
    cout <<  "\n.gulp.\n";
    
    return 0;
  }
