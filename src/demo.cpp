#include "LSystem.hpp"

using namespace std;

int main(int argc, char **argv) {

  // LSystem::test_for_using();

  vector<string> vars = {"F"};
  LRule rules = {{"F", "F+F--F+F"}};

  LSystem sys(vars, "F", rules);

  return 0;
}
