#include "LSystem.hpp"

using namespace std;

int main(int argc, char **argv) {


  LRule rules = {{"F", "F+F--F+F"}};

  LSystem sys("F", rules);

  return 0;
}
