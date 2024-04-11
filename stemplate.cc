#include "stemplate.h"
#include <stdio.h>
#include <string.h>
#include <algorithm>

Stemplate::Stemplate(int size):
  _placeholder_size(size),
  _has_more(false)
{
  INIT_LIST_HEAD(&head);
}

Stemplate::Stemplate():
  _placeholder_size(128),
  _has_more(false)
{
  INIT_LIST_HEAD(&head);
}

Stemplate::~Stemplate()
{
  list_head* pos;
  list_head* n;
  part_t* part;
  list_for_each_safe(pos, n, &head)
  {
    part = list_entry(pos, part_t, list); 
    if (part) 
    {
      if (part->type == Tag_section)
      {
        delete (Stemplate*)part->ptr;
      }
      free((void*)part);
    }
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
    fprintf(stderr, "fopen file %s failed.\n", file);
    return -1;
  }
  int ret = 0;
  do {
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char* buf = (char*)malloc(size + 1);
    if (nullptr == buf) {
      ret = -1;
      break;
    }
    memset(buf, 0, size+1);
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

int Stemplate::load_buffer(const char* str, bool section_parsed)
{
  if (nullptr == str) 
    return -1;

  enum state 
  {
    Text = 0,
    Text_End = 1,
    Tag_Begin = 2,
    Tag_End = 3,
    Section_Tag_Begin = 4,
    Section_Tag_End = 5,
    Text_Eof = 6,
  }s;
  Stemplate *p_temp = nullptr;
  const char* p = str;
  const char* q = p;
  std::string tag, value, tag1;
  s = Text;
  ++p;
  int offset = 0;
  while (*q != '\0') 
  {
    ++offset;
    if (*p == '\0') {
      s = Text_Eof;
    }
    switch(s)
    {
      case Text:
        if (*p == '{' && *q == '{') {
          s = Tag_Begin;
        }
        else if (*p == '{') {
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
          part->type = Tag_value;
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
        if (*p == '#') {
          s = Section_Tag_Begin;
        }
        else if (*p == '/' && section_parsed) {
          return offset;
        }
        else if (*p == '/') {
          s = Section_Tag_End;
        }
        else if (*p == '}') {
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
          part->type = Tag_common;
          part->len = 0;
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
      case Section_Tag_Begin:
        if (*q == '}') {
          p_temp = new Stemplate(_placeholder_size);
          if (nullptr == p_temp) 
            return -1;
          int pos = p_temp->load_buffer(p+1, true); 
          p += pos + 1;
          q += pos + 1;
          s = Section_Tag_End;
        } else if (*p != '}') {
          tag.push_back(*p);
        }
      break;
      case Section_Tag_End:
        if (*p == '}') {
          if (tag != tag1) {
            if (p_temp != nullptr) {delete p_temp; p_temp = nullptr;}
            fprintf(stderr, "file content format error.\n");
            return -1;
          }
          else {
            part_t* part = (part_t*)malloc(sizeof(part_t));
            if (nullptr == part) {
              return -1;
            }
            part->type = Tag_section;
            part->ptr = p_temp;
            part->len = 0;
            list_add_tail(&part->list, &head);
            _tag_list[tag] = &part->list;
            tag.clear();
            tag1.clear();
            s = Text;
          }
        } else {
          tag1.push_back(*p);       
        }
      break;
      case Text_Eof:
          part_t* part = (part_t*)malloc(sizeof(part_t) + value.size());
          if (nullptr == part){
            return -1;
          }
          part->type = Tag_value;
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
    if (nullptr != p_part) {
      if (p_part->type == Tag_section)
      {
        Stemplate* ptempl = (Stemplate*)p_part->ptr;
        if (ptempl->_has_more) {
          output.append(ptempl->_output);
          ptempl->_output.clear();
          ptempl->_has_more = false;
        } else {
          ptempl->render(output);
        }
      }
      else 
      {
        output.append(p_part->buffer, p_part->len);
      }
      if (p_part->type == Tag_common) {
        p_part->len = 0;
      }
    }
  }
  return 0;
}

int Stemplate::render_and_drop_crlf(std::string& output)
{
  list_head* pos;
  part_t* p_part;
  list_for_each(pos, &head)
  {
    p_part = list_entry(pos, part_t, list);
    if (nullptr != p_part) {
      if (p_part->type == Tag_section) 
      {
        Stemplate* ptempl = (Stemplate*)p_part->ptr;
        if (ptempl->_has_more) {
          output.append(ptempl->_output);
          ptempl->_output.clear();
          ptempl->_has_more = false;
        } else {
          ptempl->render_and_drop_crlf(output);
        }
      }
      else {
        output.append(p_part->buffer, p_part->len);
      }
      if (p_part->type == Tag_common) {
        p_part->len = 0;
      }
    }
  }
  output.erase(std::unique(output.begin(), output.end(),[](char a, char b) { return a == '\n' && b == '\n';}), output.end());
  return 0;
}

int Stemplate::render(char* buffer, size_t size)
{
  if (nullptr == buffer || size == 0) 
    return -1;
  std::string str;
  render(str);
  size_t n = std::min(size, str.size());
  memcpy(buffer, str.c_str(), n);
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
      if (p_part->type == Tag_section) {
        Stemplate* p_templ = (Stemplate*)p_part->ptr;
        if (p_templ && p_templ->_has_more) {
          size += p_templ->_output.size();
        }
        else if (p_templ) {
          size += p_templ->get_buffer_size();
        }
      } 
      else {
        size += p_part->len;
      }
    }
  }
  return size;
}

Stemplate* Stemplate::mutable_template(const char* tag)
{
  part_t* p_part = get_part(tag);
  if (nullptr == p_part) 
    return nullptr;
   
  if (p_part->type != Tag_section)
    return nullptr;

  return (Stemplate*)p_part->ptr;
}

void Stemplate::add()
{
  std::string str;
  render(str);
  _output.append(str);
  _has_more = true;
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
