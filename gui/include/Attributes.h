#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <exception>

namespace wgui
{
    enum class eAttributeType
    {
        Int,
        Real,
        String
    };

    struct CaseInsensitiveStrCompare
    {
        bool operator()(const std::string& a, const std::string& b) const
        {
            return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end(),
                [](char a, char b)
                {
                    return std::tolower(a) < std::tolower(b);
                });
        }
    };

    class CtrlAttribute
    {
    public:
        CtrlAttribute(eAttributeType type)
            : mType(type) { }

    private:
        eAttributeType mType;
    };

    class AttrInt : public CtrlAttribute
    {
    public:
        AttrInt()
            : mValue(0), CtrlAttribute(eAttributeType::Int) {}

        int64_t Get() const { return mValue; }
        operator int64_t() const { return mValue; }

        AttrInt Set(int64_t value)
        {
            mValue = value;
            return *this;
        }

        AttrInt operator = (int64_t value)
        {
            mValue = value;
            return *this;
        }

        int64_t& GetRef()
        {
            return mValue;
        }

    private:
        int64_t mValue;
    };

    class AttrReal : public CtrlAttribute
    {
    public:
        AttrReal()
            : mValue(0.0), CtrlAttribute(eAttributeType::Real) {}

        double Get() const { return mValue; }
        operator double() const { return mValue; }

        AttrReal Set(double value)
        {
            mValue = value;
            return *this;
        }

        AttrReal operator = (double value)
        {
            mValue = value;
            return *this;
        }

        double& GetRef()
        {
            return mValue;
        }

    private:
        double mValue;
    };

    class AttrString : public CtrlAttribute
    {
    public:
        AttrString()
            : CtrlAttribute(eAttributeType::String) {}

        std::string Get() const { return mValue; }
        operator std::string() const { return mValue; }

        AttrString Set(const std::string& value)
        {
            mValue = value;
            return *this;
        }

        AttrString operator = (const std::string& value)
        {
            mValue = value;
            return *this;
        }

        std::string& GetRef()
        {
            return mValue;
        }

    private:
        std::string mValue;
    };

    class Attribute
    {
    public:
        Attribute(eAttributeType type)
        {
            SetType(type);
        }

        eAttributeType GetType() { return mType; }

        template<class T>
        T* As()
        {
            if (!mValue)
            {
                throw std::runtime_error("Attribute not intialized to a value");
            }

            return reinterpret_cast<T*>(mValue.get());
        }

        template<class T>
        T* const As() const
        {
            if (!mValue)
            {
                throw std::runtime_error("Attribute not intialized to a value");
            }

            return reinterpret_cast<T* const>(mValue.get());
        }

        void SetType(eAttributeType type)
        {
            mType = type;

            switch (type)
            {
            case eAttributeType::Int:
                mValue = std::make_unique<AttrInt>();
                break;
            case eAttributeType::Real:
                mValue = std::make_unique<AttrReal>();
                break;
            case eAttributeType::String:
                mValue = std::make_unique<AttrString>();
                break;
            }
        }

    private:
        std::unique_ptr<CtrlAttribute> mValue;
        eAttributeType mType;
    };

    class AttributeSet
    {
    public:
        AttributeSet() : mAttributes() { }

        Attribute* operator[](const std::string& name) const
        {
            auto attr = mAttributes.find(name);

            if (attr == mAttributes.end())
            {
                throw std::out_of_range("Attribute with the given name was not found.");
            }

            return attr->second.get();
        }

        Attribute* Get(const std::string& name) const
        {
            return (*this)[name];
        }

        Attribute* Add(const std::string& name, eAttributeType type)
        {
            auto attr = mAttributes.find(name);

            if (attr != mAttributes.end())
            {
                throw std::out_of_range("Attribute with given name already exists.");
            }

            mAttributes.emplace(name, std::make_unique<Attribute>(type));
            return mAttributes[name].get();
        }

        bool AttributeExists(const std::string& name)
        {
            return mAttributes.find(name) != mAttributes.end();
        }

    private:
        std::map<std::string,
            std::unique_ptr<Attribute>,
            CaseInsensitiveStrCompare> mAttributes;
    };
}
