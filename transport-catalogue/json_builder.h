#pragma once

#include <iostream>
#include <memory>
#include <variant>
#include <string>

#include "json.h"

namespace json 
{

class LogicError : public std::logic_error {
public:
    using logic_error::logic_error;
};

class BaseItemContext;
class ValueItemContext;
class KeyItemContext;
class DictItemContext;
class DictValueItemContext;
class ArrayItemContext;

	
class Builder {
public:
	
	Builder();
	
	DictItemContext StartDict();
	
	BaseItemContext EndDict();
	
	ArrayItemContext StartArray();
	
	BaseItemContext EndArray();
		
	KeyItemContext Key(std::string key);
	
	ValueItemContext Value(Node::Value value);
	
	json::Node Build();
	
private:
	Node root_;
	std::vector<Node*> nodes_stack_;
    std::vector<std::string> keys_;
};
	



class BaseItemContext {
public:
    BaseItemContext(Builder& build);
    
    BaseItemContext StartDict();
    
	BaseItemContext EndDict();
    
	BaseItemContext StartArray();
    
	BaseItemContext EndArray();
		
	BaseItemContext Key(std::string key);
	
	BaseItemContext Value(Node::Value value);
    
    json::Node Build();

    Builder& build_;
};

//праввило 3
class ValueItemContext : public BaseItemContext {
public:
    ValueItemContext StartDict() = delete;
    
	ValueItemContext EndDict() = delete;
    
	ValueItemContext StartArray() = delete;
    
	ValueItemContext EndArray() = delete;
		
	ValueItemContext Key(std::string key) = delete;

	ValueItemContext Value(Node::Value value) = delete;
};

class DictItemContext : public BaseItemContext {
public:
    DictItemContext StartDict() = delete;
    
	BaseItemContext EndDict()
    {
        build_.EndDict();
        return BaseItemContext{*this};
    }
    
	DictItemContext StartArray() = delete;
    
	DictItemContext EndArray() = delete;
		
	KeyItemContext Key(std::string key)
    {
        build_.Key(key);
        return KeyItemContext{*this};
    }

	DictItemContext Value(Node::Value value) = delete;
    
    json::Node Build() = delete;
    
};

//правило 1
class KeyItemContext : public BaseItemContext {
public:
    DictItemContext StartDict()
    {
        build_.StartDict();
        return DictItemContext{*this};
    }
    
	KeyItemContext EndDict() = delete;
    
	ArrayItemContext StartArray();
    {
        build_.StartArray();
        return ArrayItemContext{*this};
    }
    
	KeyItemContext EndArray() = delete;
		
	KeyItemContext Key(std::string key) = delete;
	
	DictItemContext Value(Node::Value value)
    {
        build_.Value(value);
        return DictItemContext{*this};
    }
    
    KeyItemContext Build() = delete;
    
};

class ArrayItemContext : public BaseItemContext {
public:
    DictItemContext StartDict()
    {
        build_.StartDict();
        return DictItemContext{*this};
    }
    
	ArrayItemContext EndDict() = delete;
    
	ArrayItemContext StartArray()
    {
        build_.StartArray();
        return *this;
    }
    
	BaseItemContext EndArray()
    {
        build_.EndArray();
        return BaseItemContext{*this};
    }
		
	ArrayItemContext Key(std::string key) = delete;
	
	ArrayItemContext Value(Node::Value value)
	{
		build_.Value(value);
        return *this;
	}
    
    ArrayItemContext Build() = delete;
    
};

}
