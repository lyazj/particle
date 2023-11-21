#include "particle.h"
#include "nlohmann/json.hpp"
#include <unordered_map>
#include <fstream>
#include <vector>
#include <string>

using namespace std;
using nlohmann::json;

const char *ParticleAttribute::get_type_name(ParticleAttribute::type_t type)
{
  const char *names[] = {
    [PAT_NUL] = "null",
    [PAT_I64] = "int64_t",
    [PAT_D64] = "double",
    [PAT_STR] = "string",
  };
  return names[type];
}

ParticleAttributeTypeError::ParticleAttributeTypeError(ParticleAttribute::type_t type) :
  logic_error("unexpected attribute type: " + string(ParticleAttribute::get_type_name(type))) { }

ParticleAttributeKeyError::ParticleAttributeKeyError(const char *key) :
  logic_error("unexpected attribute name: " + string(key)) { }

class ParticleDatabase::Inner {

private:
  vector<string> keys;
  unordered_map<int64_t, json> values;

public:
  Inner(const char *filename);
  ParticleAttribute query(int64_t pid, const char *key) const;

};

ParticleDatabase::ParticleDatabase(const char *filename) :
  inner(new Inner(filename)) { }

ParticleDatabase::~ParticleDatabase() { delete inner; }

ParticleAttribute ParticleDatabase::query(int64_t pid, const char *key) const
{
  return inner->query(pid, key);
}

inline ParticleDatabase::Inner::Inner(const char *filename)
{
  ifstream datafile(filename);
  json data = json::parse(datafile);
  keys = data[0];
  for(size_t i = 1; i < data.size(); ++i) {
    const json &item = data[i];
    int64_t pid = item[0];
    values.emplace(pid, std::move(item));
  }
}

inline ParticleAttribute ParticleDatabase::Inner::query(int64_t pid, const char *key) const {
  for(size_t i = 0; i < keys.size(); ++i) {
    if(keys[i] == key) {
      const json &attr = values.at(pid)[i];
      if(attr.is_null()) return ParticleAttribute();
      if(attr.is_number_integer()) return ParticleAttribute((int64_t)attr);
      if(attr.is_number_float()) return ParticleAttribute((double)attr);
      const string &value = attr;
      return ParticleAttribute(value);
    }
  }
  throw ParticleAttributeKeyError(key);
}
