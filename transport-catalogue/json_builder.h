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
    
    void AddCollection(Node collection);
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

// В данном классе не запрещен метод Biuld()
// Он просто не переопределен относительно метода Build() для BaseItemContext
// Объект данного класса возвращается когда метод Value был вызван сразу после создания Builder'a
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
    
	BaseItemContext EndDict();
    
	DictItemContext StartArray() = delete;
    
	DictItemContext EndArray() = delete;
		
	KeyItemContext Key(std::string key);

	DictItemContext Value(Node::Value value) = delete;
    
    json::Node Build() = delete;
};

class KeyItemContext : public BaseItemContext {
public:
    DictItemContext StartDict();
    
	KeyItemContext EndDict() = delete;
    
	ArrayItemContext StartArray();
    
	KeyItemContext EndArray() = delete;
		
	KeyItemContext Key(std::string key) = delete;
	
	DictItemContext Value(Node::Value value);
    
    KeyItemContext Build() = delete;
};

class ArrayItemContext : public BaseItemContext {
public:
    DictItemContext StartDict();
    
	ArrayItemContext EndDict() = delete;
    
	ArrayItemContext StartArray();
    
	BaseItemContext EndArray();
		
	ArrayItemContext Key(std::string key) = delete;
	
	ArrayItemContext Value(Node::Value value);
    
    ArrayItemContext Build() = delete;
};

}
