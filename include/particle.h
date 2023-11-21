#pragma once
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <stdexcept>

class ParticleAttribute {

public:
  ParticleAttribute();
  ParticleAttribute(int64_t);
  ParticleAttribute(double);
  ParticleAttribute(const std::string &);
  ~ParticleAttribute();

  operator int64_t() const;
  operator double() const;
  operator std::string() const;

  typedef enum { PAT_NUL, PAT_I64, PAT_D64, PAT_STR } type_t;
  type_t get_type() const { return type; }
  const char *get_type_name() const { return get_type_name(type); }
  static const char *get_type_name(type_t);

private:
  type_t type;
  union { int64_t i64; double d64; char *str; } data;

};

struct ParticleAttributeTypeError : public std::logic_error {

  ParticleAttributeTypeError(ParticleAttribute::type_t type);

};

struct ParticleAttributeKeyError : public std::logic_error {

  ParticleAttributeKeyError(const char *key);

};

class ParticleDatabase {

public:
  ParticleDatabase(const char *filename);
  ~ParticleDatabase();
  ParticleAttribute query(int64_t pid, const char *key) const;

private:
  class Inner;
  Inner *inner;

};

inline ParticleAttribute::ParticleAttribute()
  : type(PAT_NUL), data{ .i64 = 0 } { }

inline ParticleAttribute::ParticleAttribute(int64_t i64)
  : type(PAT_I64), data{ .i64 = i64 } { }

inline ParticleAttribute::ParticleAttribute(double d64)
  : type(PAT_D64), data{ .d64 = d64 } { }

inline ParticleAttribute::ParticleAttribute(const std::string &str)
  : type(PAT_STR), data{ .str = strdup(str.c_str()) } { }

inline ParticleAttribute::~ParticleAttribute()
{
  if(type == PAT_STR) free(data.str);
}

inline ParticleAttribute::operator int64_t() const
{
  if(type == PAT_I64) return data.i64;
  throw ParticleAttributeTypeError(type);
}

inline ParticleAttribute::operator double() const
{
  if(type == PAT_D64) return data.d64;
  throw ParticleAttributeTypeError(type);
}

inline ParticleAttribute::operator std::string() const
{
  if(type == PAT_STR) return data.str;
  throw ParticleAttributeTypeError(type);
}
