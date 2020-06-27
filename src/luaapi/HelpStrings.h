//
// Created by rutio on 2020-06-23.
//

#ifndef LUCIMALIR_HELPSTRINGS_H
#define LUCIMALIR_HELPSTRINGS_H

#include <map>
#include <string>

class HelpStrings {
public:
  class Node {
  public:
    operator const std::string&() const;
    const Node& get(const std::string& key) const;

  private:
    std::map<std::string, Node> children;
    std::string content;
  };

  static HelpStrings& getinstance();

private:
  HelpStrings();
};


#endif //LUCIMALIR_HELPSTRINGS_H
