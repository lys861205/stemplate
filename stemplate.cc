#include "stemplate.h"
#include <stdio.h>
#include <string.h>
#include <algorithm>

Stemplate::Stemplate(int size):
  _placeholder_size(size)
{
  INIT_LIST_HEAD(&head);
}

Stemplate::Stemplate():
  _placeholder_size(128)
{
  INIT_LIST_HEAD(&head);
}

Stemplate::~Stemplate()
{
  list_head* pos;
  part_t* part;
  list_for_each(pos, &head)
  {
    part = list_entry(pos, part_t, list); 
    part = nullptr;
  }
}

int Stemplate::load(const char* file)
{
  if (nullptr == file) {
    return -1;
  }
  FILE* fp = fopen(file, "r");
  if (nullptr == fp) {
    return -1;
  }
  int ret = 0;
  do {
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char* buf = (char*)malloc(size);
    if (nullptr == buf) {
      ret = -1;
      break;
    }
    size_t n = fread(buf, size, 1, fp);
    if (n != size) {
      ret = -1;
    }
    ret = load_buffer(buf);
    free(buf);
  }while (0);

  fclose(fp);

  return ret;
}

int Stemplate::load_buffer(const char* str)
{
  if (nullptr == str) 
    return -1;

  enum state 
  {
    Text = 0,
    Text_End = 1,
    Tag_Begin = 2,
    Tag_End = 3,
    Text_Eof = 4,
  }s;
  const char* p = str;
  const char* q = p;
  std::string tag, value;
  s = Text;
  ++p;
  while (*q != '\0') 
  {
    if (*p == '\0') {
      s = Text_Eof;
    }
    switch(s)
    {
      case Text:
        if (*p == '{') {
          value.push_back(*q);
          s = Text_End;
        }
        else if (*q != '}'){
          value.push_back(*q);
        }
      break;
      case Text_End:
        if (*q == '{' && *p == '{') {
          part_t* part = (part_t*)malloc(sizeof(part_t) + value.size());
          if (nullptr == part){
            return -1;
          }
          part->len = value.size();
          memset(part->buffer, 0x00, part->len);
          memcpy(part->buffer, value.c_str(), part->len);
          list_add_tail(&part->list, &head);
          value.clear();
          s = Tag_Begin;
        }
        else {
          fprintf(stderr, "file content format error.\n");
          return -1;
        }
      break;
      case Tag_Begin:
        if (*p == '}') {
          s = Tag_End;
        }
        else if (*p != '{') {
          tag.push_back(*p); 
        }
      break;
      case Tag_End:
        if (*q == '}' && *p == '}') {
          part_t* part = (part_t*)malloc(sizeof(part_t) + _placeholder_size);
          if (nullptr == part) {
            return -1;
          }
          list_add_tail(&part->list, &head);
          _tag_list[tag] = &part->list;
          tag.clear();
          s = Text;
        }
        else {
          fprintf(stderr, "file content format error.\n");
          return -1;
        }
      break;
      case Text_Eof:
          part_t* part = (part_t*)malloc(sizeof(part_t) + value.size());
          if (nullptr == part){
            return -1;
          }
          part->len = value.size();
          memset(part->buffer, 0x00, part->len);
          memcpy(part->buffer, value.c_str(), part->len);
          list_add_tail(&part->list, &head);
      break;
    }
    ++q;
    ++p;
  }
  return s==Text_Eof ? 0 : -1;
}

int Stemplate::set_value(const char* tag, const char* value, size_t size)
{
  part_t* p_part = get_part(tag);
  if (nullptr == p_part) 
    return -1;

  size_t len = std::min(size, (size_t)_placeholder_size);
  p_part->len = len;
  memcpy(p_part->buffer, value, p_part->len);
  return 0;
}

int Stemplate::set_value(const char* tag, const std::string& value)
{
  return set_value(tag, value.c_str(), value.size());
}

int Stemplate::set_value(const char* tag, int32_t value)
{
  part_t* p_part = get_part(tag);
  if (nullptr == p_part) 
    return -1;

  char buf[32] ={0};
  int n = snprintf(buf, sizeof buf, "%d", value);
  p_part->len = n;
  memcpy(p_part->buffer, buf, n);
  return 0;
}

int Stemplate::set_value(const char* tag, int64_t value)
{
  part_t* p_part = get_part(tag);
  if (nullptr == p_part) 
    return -1;
  char buf[32] ={0};
  int n = snprintf(buf, sizeof buf, "%lld", value);
  p_part->len = n;
  memcpy(p_part->buffer, buf, n);
  return 0;
}

int Stemplate::set_value(const char* tag, uint32_t value)
{
  part_t* p_part = get_part(tag);
  if (nullptr == p_part) 
    return -1;
  char buf[32] ={0};
  int n = snprintf(buf, sizeof buf, "%u", value);
  p_part->len = n;
  memcpy(p_part->buffer, buf, n);
  return 0;
}

int Stemplate::set_value(const char* tag, uint64_t value)
{
  part_t* p_part = get_part(tag);
  if (nullptr == p_part) 
    return -1;
  char buf[32] ={0};
  int n = snprintf(buf, sizeof buf, "%llu", value);
  p_part->len = n;
  memcpy(p_part->buffer, buf, n);
  return 0;
}

int Stemplate::set_value(const char* tag, float value)
{
  part_t* p_part = get_part(tag);
  if (nullptr == p_part) 
    return -1;
  char buf[32] ={0};
  int n = snprintf(buf, sizeof buf, "%f", value);
  p_part->len = n;
  memcpy(p_part->buffer, buf, n);
  return 0;
}

int Stemplate::set_value(const char* tag, double value)
{
  part_t* p_part = get_part(tag);
  if (nullptr == p_part) 
    return -1;
  char buf[32] ={0};
  int n = snprintf(buf, sizeof buf, "%lf", value);
  p_part->len = n;
  memcpy(p_part->buffer, buf, n);
  return 0;
}
int Stemplate::render(std::string& output)
{
  list_head* pos;
  part_t* p_part;
  list_for_each(pos, &head)
  {
    p_part = list_entry(pos, part_t, list);
    if (nullptr != p_part)
      output.append(p_part->buffer, p_part->len);
  }
  return 0;
}

int Stemplate::render(char* buffer, size_t* size)
{
  if (nullptr == buffer || size == 0) 
    return -1;
  return 0;
}

int Stemplate::get_buffer_size()
{
  int size = 0;
  list_head* pos;
  part_t* p_part;
  list_for_each(pos, &head)
  {
    p_part = list_entry(pos, part_t, list); 
    if (nullptr != p_part) {
      size += p_part->len;
    }
  }
  return size;
}

Stemplate::part_t* Stemplate::get_part(const char* tag)
{
  std::string tagstr(tag); 
  std::map<std::string, list_head*>::iterator it;
  it = _tag_list.find(tagstr);
  if (it == _tag_list.end()) 
    return nullptr;
  list_head* l = it->second;
  part_t* part = list_entry(l, part_t, list);
  if (nullptr == part) 
    return nullptr;
  memset(part->buffer, 0x00, part->len);
  return part;
}
