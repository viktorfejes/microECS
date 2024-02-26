#pragma once

#include <string>
#include <vector>

namespace ECS
{
    class Type
    {
    public:
        Type(const std::vector<std::string>& type);
        std::string Str() const;

    private:
        std::vector<std::string> m_Type;
    };
}