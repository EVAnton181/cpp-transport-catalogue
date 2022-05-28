/*!
 * @file json.h
 * @author Elistratov Anton
 * @date Май 2022
 * @version 1.0
 * 
 * @brief Заголовочный файл с классами и функциями для работы с 
 * json
 * 
*/
#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>

namespace json {

using namespace std::literals;

struct RenderContext {
    RenderContext(std::ostream& out)
        : out(out) {
    }

    RenderContext(std::ostream& out, int indent_step, int indent = 0)
        : out(out)
        , indent_step(indent_step)
        , indent(indent) {
    }

    RenderContext Indented() const {
        return {out, indent_step, indent + indent_step};
    }

    void RenderIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    std::ostream& out;
    int indent_step = 0;
    int indent = 0;
};

class Node;
// Сохраните объявления Dict и Array без изменения
using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;

// Эта ошибка должна выбрасываться при ошибках парсинга JSON
class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

using NodeVal = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;

class Node final : public NodeVal {
public:

	bool operator== (const Node& rhs) const;
	bool operator!= (const Node& rhs) const;

    bool IsInt() const;
	bool IsDouble() const; //Возвращает true, если в Node хранится int либо double.
	bool IsPureDouble() const; //Возвращает true, если в Node хранится double.
	bool IsBool() const;
	bool IsString() const;
	bool IsNull() const;
	bool IsArray() const;
	bool IsMap() const;
	
	const NodeVal& GetValue() const;
	
	int AsInt() const;
	bool AsBool() const;
	double AsDouble() const; // Возвращает значение типа double, если внутри хранится double либо int. В последнем случае возвращается приведённое в double значение.
	const std::string& AsString() const;
	const Array& AsArray() const;
	const Dict& AsMap() const;
	
private:
	using variant::variant;
};

class Document {
public:
    explicit Document(Node root);

    const Node& GetRoot() const;

	bool operator== (const Document& other);
	bool operator!= (const Document& other);
private:
    Node root_;
};

Document Load(std::istream& input);

struct NodePrintValue {
    RenderContext& context;

    void operator()(std::nullptr_t) const;
    void operator()(const Array& value) const;
    void operator()(const Dict& value) const;
    void operator()(const int& value) const;
    void operator()(const bool& value) const;
    void operator()(const double& value) const;
    void operator()(const std::string& value) const;
};

void PrintNode(const Node& node, std::ostream& out);

void Print(const Document& doc, std::ostream& out);
}  // namespace json
