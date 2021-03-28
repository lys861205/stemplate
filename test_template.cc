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
    pTempl->set_value("TEXT", "Hello Stemplate");
  temp.set_value("XXXX", 40);
  temp.set_value("YYYY", 1.5);

  std::string output;
  temp.render(output);
  printf("%s\n", output.c_str());
  return 0;
}
