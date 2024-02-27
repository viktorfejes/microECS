#pragma once

#include <sstream>
#include <string>
#include <vector>

namespace microECS
{
    class Type
    {
    public:
        Type(const std::vector<std::string>& type)
            : m_Type(type) {}

        std::string Str() const
        {
            std::stringstream ss;
            for (size_t i = 0; i < m_Type.size(); ++i)
            {
                if (i != 0)
                {
                    ss << ", ";
                }
                ss << m_Type[i];
            }
            return ss.str();
        }

    private:
        std::vector<std::string> m_Type;
    };
}