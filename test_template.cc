#include "stemplate.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
  Stemplate temp;
  if (0 != temp.load("text.tpl"))
  {
    fprintf(stderr, "load template file faild.\n");
    exit(1);
  }

  Stemplate* pTempl = temp.mutable_template("section");
  if (pTempl)
  {
    pTempl->set_value("TEXT", "Hello C++");
    pTempl->add_drop_crlf();
    pTempl->set_value("TEXT", "Hello Lua");
    pTempl->add_drop_crlf();
    pTempl->set_value("TEXT", "Hello Python");
    pTempl->add_drop_crlf();
    pTempl->set_value("TEXT", "Hello Golang");
    pTempl->add_drop_crlf();
    pTempl->set_value("TEXT", "Hello Java");
    pTempl->add_drop_crlf();
    pTempl->set_value("TEXT", "Hello Ruby");
    pTempl->add_drop_crlf();
  }

  temp.set_value("YYYY", 1.5);
  temp.set_value("TEXT", "Hello string");

  //printf("===============full content\n");
  std::string output;
  temp.render(output);
  printf("%s\n", output.c_str());
  output.clear();
  //temp.render_and_drop_crlf(output);
  //printf("%s\n", output.c_str());
  return 0;
}
