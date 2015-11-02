#/!bin/sh
#
# crash_gdb -- demostrate a Segfault when invoking the debugger
#              observed 8/2015 on Debian/Jessie on X86_64
#              <deb@ichthyostega.de>

CODE=`mktemp`

cat << __END__ | g++ -x c++ - -o$CODE -std=gnu++11  && echo "compiled successfully to $CODE"

#include <functional>
#include <iostream>
#include <string>

using std::string;
using std::cout;
using std::endl;


template<class ELM>
inline string
activate (ELM& something)
  {
    std::function<string(ELM const&)> lambda = [] (ELM const& val)
                                                  {
                                                    return string(val);
                                                  };
    return lambda(something);
  }


int
main (int, char**)
  {
    cout << activate ("Data") << endl;
    return 0;
  }

__END__


$CODE && echo "executed successfully without debugger"

echo -e "now try to crash the debugger...\n\n"

gdb $CODE -ex run -ex quit

if (($?))
	then echo -e "\n\n\n\n *** Debugger crashed"
	else echo -e "\n\n\n\n +++ Debugger worked, no crash"
fi
