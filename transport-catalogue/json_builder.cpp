#include "json_builder.h"

namespace json
{
    
using namespace std::literals;

Builder::Builder()
{
	nodes_stack_.emplace_back(&root_);
}

void Builder::AddCollection(Node collection)
{
    if (nodes_stack_.empty())
    {
        throw LogicError("Build was finished"s);	
    }
    
    if (nodes_stack_.back()->IsArray())
	{
		nodes_stack_.back()->AsArray().emplace_back();
		nodes_stack_.back()->AsArray().back().GetValue() = collection.GetValue();
		nodes_stack_.emplace_back(&nodes_stack_.back()->AsArray().back());
	}
	else if (nodes_stack_.back()->IsDict())
	{
        if (keys_.empty())
        {
            throw LogicError("Value without key"s);
        }
		nodes_stack_.back()->AsDict().at(keys_.back()).GetValue() = collection.GetValue();
		nodes_stack_.emplace_back(&nodes_stack_.back()->AsDict().at(keys_.back()));
		keys_.clear();
	}
	else
	{
		nodes_stack_.back()->GetValue() = collection.GetValue();
	}
}

DictItemContext Builder::StartDict()
{
    AddCollection(json::Dict());

	DictItemContext dict{*this};
	return dict;
}
	
BaseItemContext Builder::EndDict()
{
    if (nodes_stack_.empty())
    {
        throw LogicError("Build was finished"s);	
    }
    
    if (!nodes_stack_.back()->IsDict())
	{
		throw LogicError("End of non-Dict"s);
	}
	
	if (!keys_.empty())
	{
		throw LogicError("Key without value"s);
	}
    
    nodes_stack_.pop_back();
    
    BaseItemContext base{*this};
	return base;
}

ArrayItemContext Builder::StartArray()
{
    AddCollection(json::Array());
    
	ArrayItemContext item_array{*this};
	return item_array;
}

BaseItemContext Builder::EndArray()
{
    if (nodes_stack_.empty())
    {
        throw LogicError("Build was finished"s);	
    }
    
	if (!nodes_stack_.back()->IsArray())
	{
		throw LogicError("End of non-array"s);	
	}
	
	nodes_stack_.pop_back();
    
	BaseItemContext base{*this};
	return base;
}

KeyItemContext Builder::Key(std::string key)
{
    if (nodes_stack_.empty())
    {
        throw LogicError("Build was finished"s);	
    }
    
    if (!nodes_stack_.back()->IsDict())
    {
        throw LogicError("Dict is not started"s);
    }

    if (!keys_.empty())
    {
        throw LogicError("Key without value"s);
    }
    
	keys_.push_back(key);
    
    nodes_stack_.back()->AsDict()[key];
    
   	KeyItemContext item_key{*this};
	return item_key;
}

ValueItemContext Builder::Value(Node::Value value) 
{
    if (nodes_stack_.empty())
    {
        throw LogicError("Build was finished"s);	
    }
    
    if (nodes_stack_.back()->IsNull() && nodes_stack_.size() == 1)
    {
        nodes_stack_.back()->GetValue() = value;
    nodes_stack_.clear();
    }
    else if (nodes_stack_.back()->IsArray())
    {
        nodes_stack_.back()->AsArray().emplace_back();
        nodes_stack_.back()->AsArray().back().GetValue() = value;
    }
    else if (nodes_stack_.back()->IsDict())
    {
        if (keys_.empty())
        {
            throw LogicError("Value without key"s);
        }
        nodes_stack_.back()->AsDict().at(keys_.back()).GetValue() = value;
        keys_.clear();
    }
    
    ValueItemContext base{*this};
    return base;
}

json::Node Builder::Build()
{
	if (!nodes_stack_.empty())
	{
		throw LogicError("Build error (incomplete Node)"s);
	}

	return root_;
}


BaseItemContext::BaseItemContext(Builder& build)
    : build_(build)
{
}

BaseItemContext BaseItemContext::StartDict()
{
    build_.StartDict();
    return {*this};
}

BaseItemContext BaseItemContext::EndDict()
{
    build_.EndDict();
    return {*this};
}

BaseItemContext BaseItemContext::StartArray()
{
    build_.StartArray();
    return {*this};
}

BaseItemContext BaseItemContext::EndArray()
{
    build_.EndArray();
    return {*this};
}
    
BaseItemContext BaseItemContext::Key(std::string key)
{
    build_.Key(key);
    return {*this};
}

BaseItemContext BaseItemContext::Value(Node::Value value)
{
    build_.Value(value);
    return {*this};
}

json::Node BaseItemContext::Build()
{
    return build_.Build();
}

BaseItemContext DictItemContext::EndDict()
{
    build_.EndDict();
    return BaseItemContext{*this};
}

KeyItemContext DictItemContext::Key(std::string key)
{
    build_.Key(key);
    return KeyItemContext{*this};
}

DictItemContext KeyItemContext::StartDict()
{
    build_.StartDict();
    return DictItemContext{*this};
}

ArrayItemContext KeyItemContext::StartArray()
{
    build_.StartArray();
    return ArrayItemContext{*this};
}

DictItemContext KeyItemContext::Value(Node::Value value)
{
    build_.Value(value);
    return DictItemContext{*this};
}

DictItemContext ArrayItemContext::StartDict()
{
    build_.StartDict();
    return DictItemContext{*this};
}
    
ArrayItemContext ArrayItemContext::StartArray()
{
    build_.StartArray();
    return *this;
}

BaseItemContext ArrayItemContext::EndArray()
{
    build_.EndArray();
    return BaseItemContext{*this};
}

ArrayItemContext ArrayItemContext::Value(Node::Value value)
{
    build_.Value(value);
    return *this;
}

} //namespace json
