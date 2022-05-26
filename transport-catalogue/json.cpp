#include "json.h"

using namespace std;

namespace json {

Node::Node(void)
    : value_(nullptr) {
}

Node::Node(nullptr_t null)
    : value_(null) {
}

Node::Node(Array array)
    : value_(move(array)) {
}

Node::Node(Dict map)
    : value_(move(map)) {
}

Node::Node(bool b)
    : value_(move(b)) {
}

Node::Node(int value)
    : value_(value) {
}

Node::Node(double value)
    : value_(move(value)) {
}

Node::Node(string value)
    : value_(move(value)) {
}
	
bool Node::operator== (const Node& other) const
{
    return (this->value_ == other.value_);
}
 
bool Node::operator!= (const Node& other) const
{
    return !((*this) == other);
}
	
bool Node::IsInt() const {
	if (holds_alternative<int>(value_)) {
		return true;
	}
	return false;
}

bool Node::IsDouble() const {
	if (holds_alternative<double>(value_) || holds_alternative<int>(value_)) {
		return true;
	}
	return false;
}

///Возвращает true, если в Node хранится int либо double.
bool Node::IsPureDouble() const {
	if (holds_alternative<double>(value_)) {
		return true;
	}
	return false;
} 

///Возвращает true, если в Node хранится double.
bool Node::IsBool() const {
	if (holds_alternative<bool>(value_)) {
		return true;
	}
	return false;
}

bool Node::IsString() const {
	if (holds_alternative<std::string>(value_)) {
		return true;
	}
	return false;
}

bool Node::IsNull() const {
	if (holds_alternative<std::nullptr_t>(value_)) {
		return true;
	}
	return false;
}

bool Node::IsArray() const {
	if (holds_alternative<Array>(value_)) {
		return true;
	}
	return false;
}

bool Node::IsMap() const {
	if (holds_alternative<Dict>(value_)) {
		return true;
	}
	return false;
}

int Node::AsInt() const {
	if (IsInt()) {
		return std::get<int>(value_);
	}
	else {
		throw std::logic_error("other type");
	}
}

bool Node::AsBool() const {
	if (IsBool()) {
		return std::get<bool>(value_);
	}
	else {
		throw std::logic_error("other type");
	}
}

double Node::AsDouble() const {
	if (IsDouble()) {
		if (IsInt()) {
			return std::get<int>(value_);
		}
		return std::get<double>(value_);
	}
	else {
		throw std::logic_error("other type");	
	}
}

const std::string& Node::AsString() const {
	if (IsString()) {
		return std::get<std::string>(value_);
	}
	else {
		throw std::logic_error("other type");	}
}

const Array& Node::AsArray() const {
    if (IsArray()) {
		return std::get<Array>(value_);
	}
	else {
		throw std::logic_error("other type");
	}
}

const Dict& Node::AsMap() const {
    if (IsMap()) {
		return std::get<Dict>(value_);
	}
	else {
		throw std::logic_error("other type");
	}
}

	
Node LoadNode(istream& input);

using Number = std::variant<int, double>;

Number LoadNumber(std::istream& input) {
    using namespace std::literals;

    std::string parsed_num;

    // Считывает в parsed_num очередной символ из input
    auto read_char = [&parsed_num, &input] {
        parsed_num += static_cast<char>(input.get());
        if (!input) {
            throw ParsingError("Failed to read number from stream"s);
        }
    };

    // Считывает одну или более цифр в parsed_num из input
    auto read_digits = [&input, read_char] {
        if (!std::isdigit(input.peek())) {
            throw ParsingError("A digit is expected"s);
        }
        while (std::isdigit(input.peek())) {
            read_char();
        }
    };

    if (input.peek() == '-') {
        read_char();
    }
    // Парсим целую часть числа
    if (input.peek() == '0') {
        read_char();
        // После 0 в JSON не могут идти другие цифры
    } else {
        read_digits();
    }

    bool is_int = true;
    // Парсим дробную часть числа
    if (input.peek() == '.') {
        read_char();
        read_digits();
        is_int = false;
    }

    // Парсим экспоненциальную часть числа
    if (int ch = input.peek(); ch == 'e' || ch == 'E') {
        read_char();
        if (ch = input.peek(); ch == '+' || ch == '-') {
            read_char();
        }
        read_digits();
        is_int = false;
    }

    try {
        if (is_int) {
            // Сначала пробуем преобразовать строку в int
            try {
                return std::stoi(parsed_num);
            } catch (...) {
                // В случае неудачи, например, при переполнении,
                // код ниже попробует преобразовать строку в double
            }
        }
        return std::stod(parsed_num);
    } catch (...) {
        throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
    }
}

Node LoadArray(istream& input) {
    Array result;
    auto end = std::istreambuf_iterator<char>();

	char ch;
    while (true) {
		if (!input) {
			throw ParsingError("Array parsing error");
		}
		input >> ch;
		if (ch == ']') {
			break;
		}
        if (ch != ',') {
            input.putback(ch);
        }
        result.push_back(LoadNode(input));
    }

    return result;
}

// Считывает содержимое строкового литерала JSON-документа
// Функцию следует использовать после считывания открывающего символа ":
std::string LoadString(std::istream& input) {
    using namespace std::literals;
    
    auto it = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    std::string s;
    while (true) {
        if (it == end) {
            // Поток закончился до того, как встретили закрывающую кавычку?
            throw ParsingError("String parsing error");
        }
        const char ch = *it;
        if (ch == '"') {
            // Встретили закрывающую кавычку
            ++it;
            break;
        } else if (ch == '\\') {
            // Встретили начало escape-последовательности
            ++it;
            if (it == end) {
                // Поток завершился сразу после символа обратной косой черты
                throw ParsingError("String parsing error");
            }
            const char escaped_char = *(it);
            // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
            switch (escaped_char) {
                case 'n':
                    s.push_back('\n');
                    break;
                case 't':
                    s.push_back('\t');
                    break;
                case 'r':
                    s.push_back('\r');
                    break;
                case '"':
                    s.push_back('"');
                    break;
                case '\\':
                    s.push_back('\\');
                    break;
                default:
                    // Встретили неизвестную escape-последовательность
                    throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
            }
        } else if (ch == '\n' || ch == '\r') {
            // Строковый литерал внутри- JSON не может прерываться символами \r или \n
            throw ParsingError("Unexpected end of line"s);
        } else {
            // Просто считываем очередной символ и помещаем его в результирующую строку
            s.push_back(ch);
        }
        ++it;
    }
    return s;
}

Dict LoadDict(istream& input) {
    Dict result;

	char ch;
    while (true) {
		if (!input) {
			throw ParsingError("Array parsing error");
		}
		input >> ch;
		if (ch == '}') {
			break;
		}
        if (ch == ',') {
            input >> ch;
        }

        string key = LoadString(input);
        input >> ch;
        result.insert({move(key), LoadNode(input)});
    }

    return result;
}

bool LoadBool(istream& input) {
	bool ans;
	char ch;
	int count = 4;
	if (input.peek() == 'f') {
		count = 5;
	}
	int i = 0;
	std::string s;
	while (i<count)
	{
		if (!input) {
			throw ParsingError("Bool parsing error");
		}
		input >> ch;
		s.push_back(ch);
		++i;
	}
	
	if (i == 4 && s == "true") {
		ans =true;
	} else if (i == 5 && s == "false") {
		ans = false;
	} else {
		throw ParsingError("Bool parsing error");
	}
	
	return ans;
}

nullptr_t LoadNull(istream& input) {
	nullptr_t ans;
	int i = 0;
	char ch;
	std::string s;
	while (i<4)
	{
		if (!input) {
			throw ParsingError("Null parsing error");
		}
		ch = ' ';
		input.get(ch);
		if (ch == ' ') {
			throw ParsingError("Null parsing error");
		}
		s.push_back(ch);
		++i;
	}
	
	if (i == 4 && s == "null") {
		ans = nullptr;
	} else {
		throw ParsingError("Null parsing error"); 
	}
	return ans;
}

Node LoadNode(istream& input) {
    char ch;
    input >> ch;

	switch (ch){
		case EOF:
			throw ParsingError("String parsing error");
		case '[':
			return Node(LoadArray(input));
		case '{':
			return Node(LoadDict(input));
		case '"':
			return Node(LoadString(input));
		case 't':
			return Node(LoadBool(input.unget()));
		case 'f':
			return Node(LoadBool(input.unget()));
		case 'n':
			return Node(LoadNull(input.unget()));
		default:
			auto num = LoadNumber(input.unget());
			if (holds_alternative<double>(num)) {
				return std::get<double>(num);
			} else {
				return std::get<int>(num);
			}
	}
}


Document::Document(Node root)
    : root_(move(root)) {
}

const Node& Document::GetRoot() const {
    return root_;
}

bool Document::operator== (const Document& other) {
	return this->GetRoot() == other.GetRoot();
}

bool Document::operator!= (const Document& other) {
	return (this->GetRoot() != other.GetRoot());
}

Document Load(istream& input) {
    return Document{LoadNode(input)};
}

// int, double
template <typename Value>
void PrintValue(const Value& value, const RenderContext& context) {
	auto& out = context.out;
	out << value;
}

// Перегрузка функции PrintValue для вывода значений null
void PrintValue(std::nullptr_t, const RenderContext& context) {
	auto& out = context.out;
	out << "null"sv;
}

// string
void PrintValue(const std::string& st, const RenderContext& context) {
	auto& out = context.out;
	auto it = st.begin();
	auto end = st.end();
	out << '"';
	while (true) {
		if (it == end) {
			break;
		}
		const char ch = *it;
		switch (ch) {
			case '\\':
				out << '\\' << '\\';
				break;
			case '"':
				out << '\\' << '"';
				break;
			case '\n':
				out << '\\' << 'n';
				break;
			case '\r':
				out << '\\' << 'r';
				break;
			case '\t':
				out << '\t';
				break;
			default:
				out << ch;
		}
		++it;
	}
	out << '"';
}

// bool
void PrintValue(const bool b, const RenderContext& context) {
	auto& out = context.out;

	if (b) {
		out << "true";
	} 
	else {
		out << "false";
	}
}

// Array
void PrintValue(const Array& array, const RenderContext& context) {
	auto& out = context.out;
	out << '[' << '\n';
	context.RenderIndent();
	bool first = true;
	for (auto a : array) {
		if (!first) {
			out << ',' << '\n';
			context.RenderIndent();
		}
		first = false;
		PrintNode(a, out);
	}
	context.RenderIndent();
	out << '\n' << ']';
}

// Array
void PrintValue(const Dict& dict, const RenderContext& context) {
	auto& out = context.out;
	out << "{" << '\n';
	context.RenderIndent();
	bool first = true;
	for (const auto& [key, value] : dict) {
		if (!first) {
			out << ", " << '\n';
			context.RenderIndent();
		}
		first = false;
        out << '"' << key << "\": "; /*<< value << "; ";*/
		PrintNode(value, out);
    }
	out << '\n';
	context.RenderIndent();
    out << "}";
}

void PrintNode(const Node& node, std::ostream& out) {
	RenderContext context(out, 2, 2);
    std::visit(
        [&context](const auto& value){ PrintValue(value, context); },
        node.GetValue());
} 


void Print(const Document& doc, std::ostream& out) {
	PrintNode(doc.GetRoot(), out);
}

}  // namespace json
