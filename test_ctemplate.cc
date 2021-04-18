#include <ctemplate/template.h>
#include <sys/time.h>

int main(int argc, char* argv[])
{  uint64_t loop = 1000000;
  if (argc == 2) {
    loop = atoi(argv[1]);
  }
  struct timeval tv;
  gettimeofday(&tv, NULL);
  uint64_t start = tv.tv_sec * 1000000ULL + tv.tv_usec;
  ctemplate::TemplateDictionary dict("test");
  for (int i = 0; i < loop; ++i) 
  {
    ctemplate::TemplateDictionary *tb_dict = dict.AddSectionDictionary("section");
    tb_dict->SetIntValue("Age", i);
    tb_dict->SetValue("Name", std::to_string(i) + " name");
  }
  std::string content;
  ctemplate::Template* tp;
  tp = ctemplate::Template::GetTemplate("text.tpl", ctemplate::DO_NOT_STRIP);
  tp->Expand(&content, &dict);
  //printf("%s\n", content.c_str());
  gettimeofday(&tv, NULL);
  uint64_t end = tv.tv_sec * 1000000ULL + tv.tv_usec;
  printf("ctemplate cost time: %lu\n", end - start);
  return 0;
}
