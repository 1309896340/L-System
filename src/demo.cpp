#include "LSystem.hpp"

using namespace std;

int main(int argc, char** argv) {
    // LSystem::D0L_System lsys("A(4,5)F(1,2,3)B(4)", "F(x,y,z)->A(x+z,y-x)F(x-2,z-3,y-1)B(y/2)\nA(x,y)->B(x/2)A(y/2,x/2)B(y+1)");

    vector<string> prods = {
        "F(x,y,z)->A(x+z,y-x)\\+F(x-2,z-3,y-1)/B(y/2)-",
        "A(x,y)->+-B(x/2)/A(y/2,x/2)B(y+1)"
    };
    LSystem::D0L_System lsys("A(4,5)F(1,2,3)B(4)", prods);

    for (int i = 0; i < 3; i++) 
        cout << lsys.next() << endl;
    
    return 0;
}
