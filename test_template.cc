#include "stemplate.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

int main(int argc, char* argv[])
{
  uint64_t loop = 1000000;
  if (argc == 2) {
    loop = atoi(argv[1]);
  }
  struct timeval tv;
  gettimeofday(&tv, NULL);
  uint64_t start = tv.tv_sec * 1000000ULL + tv.tv_usec;
  Stemplate temp;
  if (0 != temp.load("text.tpl"))
  {
    fprintf(stderr, "load template file faild.\n");
    exit(1);
  }

  Stemplate* pTempl = temp.mutable_template("section");
  if (pTempl)
  {
    for (int i=0; i < loop; ++i) 
    {
      pTempl->set_value("Age", i);
      pTempl->set_value("Name", std::to_string(i) + " name");
      pTempl->add();
    }
  }
  std::string output;
  temp.render(output);
  //printf("%s\n", output.c_str());
  gettimeofday(&tv, NULL);
  uint64_t end = tv.tv_sec * 1000000ULL + tv.tv_usec;
  printf("stemplate cost time: %lu \n", end - start);
  return 0;
}
