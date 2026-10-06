#include "pch.h"
#include "../Serialization/Json.h"
#include "../Core/File.h"

#include <rapidjson/istreamwrapper.h>
#include <rapidjson/error/en.h>
#include <iostream>

namespace nu::json {
    bool Load(const std::string& filename, document_t& document)
    {
        // read the file into a string
        std::string buffer;
        if (!ReadTextFile(filename, buffer))
        {
            std::cerr << "Could not read file:" << filename << std::endl;
            return false;
        }

        // convert the string into a json stream
        std::stringstream stream(buffer);
        rapidjson::IStreamWrapper istream(stream);

        // set the json document from the stream
        document.ParseStream(istream);

        // check if the parse was successful
        if (document.HasParseError())
        {
            std::cerr << "Could not parse JSON: " << filename << std::endl;
            std::cerr << "Error: " << rapidjson::GetParseError_En(document.GetParseError()) << std::endl;

            return false;
        }

        // check that the root value is an object, not an array/string/number/etc.
        if (!document.IsObject())
        {
            std::cerr << "JSON root is not an object: " << filename << std::endl;
            return false;
        }

        return true;
    }

    bool Read(const value_t& value, const std::string& name, int& data, bool required)
    {
        if (!value.HasMember(name.c_str()))
        {
            if (required)
            {
                std::cerr << "Could not read JSON value (int): " << name << std::endl;
                return false;
            }

            return true;
        }

        if (!value[name.c_str()].IsInt())
        {
            std::cerr << "Could not read JSON value (int): " << name << std::endl;
            return false;
        }

        data = value[name.c_str()].GetInt();
        return true;
    }  
      
    bool Read(const value_t& value, const std::string& name, unsigned int& data, bool required)
    {
        if (!value.HasMember(name.c_str()))
        {
            if (required)
            {
                std::cerr << "Could not read JSON value (unsigned int): " << name << std::endl;
                return false;
            }

            return true;
        }

        if (!value[name.c_str()].IsUint())
        {
            std::cerr << "Could not read JSON value (unsigned int): " << name << std::endl;
            return false;
        }

        data = value[name.c_str()].GetUint();
        return true;
    }

    bool Read(const value_t& value, const std::string& name, bool& data, bool required)
    {
        if (!value.HasMember(name.c_str()))
        {
            if (required)
            {
                std::cerr << "Could not read JSON value (bool): "
                    << name << std::endl;
                return false;
            }

            return true;
        }

        if (!value[name.c_str()].IsBool())
        {
            std::cerr << "Could not read JSON value (bool): "
                << name << std::endl;
            return false;
        }

        data = value[name.c_str()].GetBool();

        return true;
    }

    bool Read(const value_t& value, const std::string& name, float& data, bool required)
    {
        if (!value.HasMember(name.c_str()))
        {
            if (required)
            {
                std::cerr << "Could not read JSON value (float): "
                    << name << std::endl;
                return false;
            }

            return true;
        }

        if (!value[name.c_str()].IsNumber())
        {
            std::cerr << "Could not read JSON value (float): "
                << name << std::endl;
            return false;
        }

        data = value[name.c_str()].GetFloat();

        return true;
    }

    bool Read(const value_t& value, const std::string& name, std::string& data, bool required)
    {
        if (!value.HasMember(name.c_str()))
        {
            if (required)
            {
                std::cerr << "Could not read JSON value (string): "
                    << name << std::endl;
                return false;
            }

            return true;
        }

        if (!value[name.c_str()].IsString())
        {
            std::cerr << "Could not read JSON value (string): "
                << name << std::endl;
            return false;
        }

        data = value[name.c_str()].GetString();

        return true;
    }

    bool Read(const value_t& value, const std::string& name, Vector2& data, bool required)
    {
        if (!value.HasMember(name.c_str()))
        {
            if (required)
            {
                std::cerr << "Could not read JSON value (Vector2): "
                    << name << std::endl;
                return false;
            }

            return true;
        }

        if (!value[name.c_str()].IsArray() ||
            value[name.c_str()].Size() != 2)
        {
            std::cerr << "Could not read JSON value (Vector2): "
                << name << std::endl;
            return false;
        }

        auto& array = value[name.c_str()];

        for (rapidjson::SizeType i = 0; i < array.Size(); i++)
        {
            if (!array[i].IsNumber())
            {
                std::cerr << "Could not read JSON value (Vector2): "
                    << name << std::endl;
                return false;
            }

            data[i] = array[i].GetFloat();
        }

        return true;
    }

    bool Read(const value_t& value, const std::string& name, Vector3& data, bool required)
    {
        if (!value.HasMember(name.c_str()))
        {
            if (required)
            {
                std::cerr << "Could not read JSON value (Vector3): "
                    << name << std::endl;
                return false;
            }

            return true;
        }

        if (!value[name.c_str()].IsArray() ||
            value[name.c_str()].Size() != 3)
        {
            std::cerr << "Could not read JSON value (Vector3): "
                << name << std::endl;
            return false;
        }

        auto& array = value[name.c_str()];

        for (rapidjson::SizeType i = 0; i < array.Size(); i++)
        {
            if (!array[i].IsNumber())
            {
                std::cerr << "Could not read JSON value (Vector3): "
                    << name << std::endl;
                return false;
            }

            data[i] = array[i].GetFloat();
        }

        return true;
    }


    bool Read(const value_t& value, const std::string& name, vector<int>& data, bool required)
    {
        if (!value.HasMember(name.c_str()))
        {
            if (required)
            {
                std::cerr << "Could not read JSON value (vector<int>): "
                    << name << std::endl;
                return false;
            }

            return true;
        }

        auto& array = value[name.c_str()];

        for (rapidjson::SizeType i = 0; i < array.Size(); i++)
        {
            if (!array[i].IsNumber())
            {
                std::cerr << "Could not read JSON value (vector<int>): "
                    << name << std::endl;
                return false;
            }

            data.push_back(array[i].GetInt());
        }

        return true;
    }
}