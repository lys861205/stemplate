# stemplate
模版文件替换

## 格式
* 基本占位符符号定义

  {{}}表示一个替换的占位符信息
* 块占位符定义

  {{#}} 表示一个块占位符的开始

  {{/}} 表示一个块占位符的结束
  
 ## 使用demo
 <html>
 <h1>
 {{TITLE}}
 </h1>
 {{#section}}
 <body>
 {{TEXT}}
 </body>
 {{/section}}
  
 ```
Stemplate template;
template.load("demo.tpl");
template.set_value("TITLE", "Hello world!");
Stemplate* ptr = template.mutable_template("section");
ptr->set_value("TEXT", "Hello section!");
 ```
