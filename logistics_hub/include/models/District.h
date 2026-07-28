#pragma once
#include <string>

class District {
   public:
    District();
    District(int id, const std::string& name);
    ~District() = default;

    int getId() const;
    const std::string& getName() const;

   private:
    int id;
    std::string name;
};