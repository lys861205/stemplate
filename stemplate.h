#ifndef __S_TEMPLATE_H__
#define __S_TEMPLATE_H__

#include "list.h"
#include <map>
#include <string>
#include <stdint.h>

class Stemplate 
{
  enum TagType 
  {
    Tag_common = 0,
    Tag_section = 1,
    Tag_repeated = 2 //not implement
  };
  #define REPLACE_STR_SIZE 512
  typedef struct {
    list_head list;
    TagType type;
    int len;
    union {
      void* ptr;
      char buffer[0];
    };
  } part_t;
public:
  Stemplate(int size);
  Stemplate();
  ~Stemplate();

  int load(const char* file);
  int load_buffer(const char* str, bool section_parsed = false);
  Stemplate* mutable_template(const char* tag);
  int get_buffer_size();
  int set_value(const char* tag, const char* value, size_t size);
  int set_value(const char* tag, const std::string& value);
  int set_value(const char* tag, int32_t value);
  int set_value(const char* tag, int64_t value);
  int set_value(const char* tag, uint32_t value);
  int set_value(const char* tag, uint64_t value);
  int set_value(const char* tag, float value);
  int set_value(const char* tag, double value);
  int render_and_drop_crlf(std::string& output);
  int render(std::string& output);
  int render(char* buffer, size_t* size);
private:
  part_t* get_part(const char* tag);
private:
  std::map<std::string, list_head*> _tag_list;
  list_head head;
  int _placeholder_size;
};

#endif // __S_TEMPLATE_H__

