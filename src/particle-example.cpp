#include "particle.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>

using namespace std;

int main()
{
  ParticleDatabase db(PDG_PATH);
  for(int64_t i = 1; i < 100; ++i) {
    try {
      string name = db.query(i, "pdg_name");
      string latex = db.query(i, "latex_name");
      cout << left << setw(8) << i << left << setw(16) << name << left << setw(16) << latex << endl;
    } catch(const out_of_range &) { }
  }
  return 0;
}
