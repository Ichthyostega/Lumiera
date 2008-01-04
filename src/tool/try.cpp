/* try.cpp  -  for trying out some language features....
 *             scons will create the binary bin/try
 *
 */ 

// 8/07  - how to control NOBUG??
//         execute with   NOBUG_LOG='ttt:TRACE' bin/try
// 1/08  - working out a static initialisation problem for Visitor (Tag creation)


#include <nobug.h>
#include <iostream>

using std::string;
using std::cout;

NOBUG_CPP_DEFINE_FLAG(test);

    template<class TOOL> class Tag;
    
    
    template<class TOOL, class TOOLImpl>
    class TagTypeRef 
      {
      public:
        static Tag<TOOL> tag;
      };
      
      
    template<class TOOL>
    class Tag
    {
      size_t tagID;
      static size_t lastRegisteredID;
      
    public:
      Tag(size_t tt=0) : tagID(tt) {}
      operator size_t()  const { return tagID; }

      template<class TOOLImpl>
      static Tag<TOOL>&
      get (TOOLImpl* const concreteTool=0)
        {
          INFO (test,"getTag");
          Tag<TOOL>& t = TagTypeRef<TOOL,TOOLImpl>::tag;
          if (!t)
            t.tagID = ++Tag<TOOL>::lastRegisteredID;
          return t;
        }
    
    };
    

    
    
    /** storage for the Tag registry for each concrete tool */
    template<class TOOL, class TOOLImpl>
    Tag<TOOL> TagTypeRef<TOOL,TOOLImpl> ::tag (0); 

    template<class TOOL>
    size_t Tag<TOOL>::lastRegisteredID (0);



    
    
    class TT
    {
      
    };
    
    class TI : public TT
    {
      
    };
    class TII : public TT
    {
      
    };

int main (int argc, char* argv[])
  {
    
    NOBUG_INIT;
    
    size_t xxx = Tag<TT>::get<TII>(); 

    cout << "created Tag=" << xxx <<"\n";
    cout << "created Tag=" << Tag<TT>::get<TI> () <<"\n";
    
    return 0;
  }
