#include "Type.h"

#include <sstream>
#include <vector>

namespace ECS
{
    Type::Type(const std::vector<std::string>& type)
        : m_Type(type)
    {
    }

    std::string Type::Str() const
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
}