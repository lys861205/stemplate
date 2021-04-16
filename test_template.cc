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
    pTempl->set_value("Age", 0);
    pTempl->set_value("Name", "Hello C++");
    pTempl->add_drop_crlf();
    pTempl->set_value("Age", 1);
    pTempl->set_value("Name", "Hello Lua");
    pTempl->add_drop_crlf();
    pTempl->set_value("Age", 2);
    pTempl->set_value("Name", "Hello Python");
    pTempl->add_drop_crlf();
    pTempl->set_value("Age", 3);
    pTempl->set_value("Name", "Hello Golang");
    pTempl->add_drop_crlf();
    pTempl->set_value("Age", 4);
    pTempl->set_value("Name", "Hello Java");
    pTempl->add_drop_crlf();
    pTempl->set_value("Age", 5);
    pTempl->set_value("Name", "Hello Ruby");
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
