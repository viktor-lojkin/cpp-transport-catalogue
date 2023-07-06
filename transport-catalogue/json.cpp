#include "json.h"

using namespace std;

namespace json {

namespace {

using Number = std::variant<int, double>;

Node LoadNode(istream& input);

Node LoadNull(istream& input) {
    const string s = "null";

    for (size_t i = 0; i < s.size(); i++) {
        if (s.at(i) == input.get()) {
            continue;
        } else {
            throw JsonParsingError("Null parsing error"s);
        }
    }

    return {};
}

std::string LoadString(std::istream& input) {
    auto iter = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    std::string s;

    while (true) {
        if (iter == end) {
            throw JsonParsingError("String parsing error"s);
        }

        const char ch = *iter;
        if (ch == '"') {
            ++iter;
            break;
        } else if (ch == '\\') {
            ++iter;

            if (iter == end) {
                throw JsonParsingError("String parsing error"s);
            }

            const char escaped_char = *(iter);
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
                    throw JsonParsingError("Unknown escape-sequence: \\"s + escaped_char);
            }
        } else if (ch == '\n' || ch == '\r') {
            throw JsonParsingError("Wrong escape-sequence"s);
        } else {
            s.push_back(ch);
        }

        ++iter;
    }

    return s;
}

Node LoadNumber(std::istream& input) {
    std::string parsed_num;

    auto read_char = [&parsed_num, &input] {
        parsed_num += static_cast<char>(input.get());
        if (!input) {
            throw JsonParsingError("Wrong stream"s);
        }
    };

    auto read_digit = [&input, read_char] {
        if (!std::isdigit(input.peek())) {
            throw JsonParsingError("A digit is expected"s);
        }
        while (std::isdigit(input.peek())) {
            read_char();
        }
    };

    if (input.peek() == '-') {
        read_char();
    }

    if (input.peek() == '0') {
        read_char();
    } else {
        read_digit();
    }

    bool is_int = true;
    if (input.peek() == '.') {
        read_char();
        read_digit();
        is_int = false;
    }

    if (int ch = input.peek(); ch == 'e' || ch == 'E') {
        read_char();
        if (ch = input.peek(); ch == '+' || ch == '-') {
            read_char();
        }
        read_digit();
        is_int = false;
    }

    try {
        if (is_int) {
            try {
                return std::stoi(parsed_num);
            } catch (...) {}
        }
        return std::stod(parsed_num);
    } catch (...) {
        throw JsonParsingError("Failed converting "s + parsed_num + " to number"s);
    }
}

Node LoadBool(istream& input) {
    const string nameFalse = "false"s;
    const string nameTrue = "true"s;
    char c = input.get();
    bool value = (c == 't');
    std::string const* name = value ? &nameTrue : &nameFalse;
    for (size_t i = 1; i < name->size(); i++) {
        if (name->at(i) == input.get()) continue;
        else throw JsonParsingError("Bool parsing error"s);
    }
    return Node(value);
}

Node LoadArray(istream& input) {
    Array result;
    if (input.peek() == -1) throw JsonParsingError("Array parsing error"s);

    for (char c; input >> c && c != ']';) {
        if (c != ',') {
            input.putback(c);
        }
        result.push_back(LoadNode(input));
    }

    return Node(std::move(result));
}

Node LoadDict(istream& input) {
    Dict result;
    if (input.peek() == -1) throw JsonParsingError("Dict parsing error"s);

    for (char c; input >> c && c != '}';) {
        if (c == ',') {
            input >> c;
        }

        string key = LoadString(input);
        input >> c;
        result.insert({ std::move(key), LoadNode(input) });
    }

    return Node(std::move(result));
}

Node LoadNode(istream& input) {
    char c;
    input >> c;

    if (c == 'n') {
        input.putback(c);
        return LoadNull(input);
    }
    else if (c == '"') {
        return LoadString(input);
    }
    else if (c == 't' || c == 'f') {
        input.putback(c);
        return LoadBool(input);
    }
    else if (c == '[') {
        return LoadArray(input);
    }
    else if (c == '{') {
        return LoadDict(input);
    }
    else {
        input.putback(c);
        return LoadNumber(input);
    }
}

}  // namespace


Node::Node(std::nullptr_t)
    : value_(nullptr) {
}

Node::Node(std::string value)
    : value_(std::move(value)) {
}

Node::Node(int value)
    : value_(value) {
}

Node::Node(double value)
    : value_(value) {
}

Node::Node(bool value)
    : value_(value) {
}

Node::Node(Array array)
    : value_(std::move(array)) {
}

Node::Node(Dict map)
    : value_(std::move(map)) {
}

bool Node::IsInt() const {
    return holds_alternative<int>(value_);
}

bool Node::IsDoubleOrInt() const {
    return holds_alternative<double>(value_) || holds_alternative<int>(value_);
}

bool Node::IsBool() const {
    return holds_alternative<bool>(value_);
}

bool Node::IsString() const {
    return holds_alternative<std::string>(value_);
}

bool Node::IsNull() const {
    return holds_alternative<std::nullptr_t>(value_);
}

bool Node::IsArray() const {
    return holds_alternative<Array>(value_);
}

bool Node::IsMap() const {
    return holds_alternative<Dict>(value_);
}

int Node::AsInt() const {
    if (!IsInt()) {
        throw JsonParsingError("Int is expected"s);
    }
    return std::get<int>(value_);
}

bool Node::AsBool() const {
    if (!IsBool()) {
        throw JsonParsingError("Bool is expected"s);
    }
    return std::get<bool>(value_);
}

double Node::AsDouble() const {
    if (!IsDoubleOrInt()) {
        throw JsonParsingError("Double is expected"s);
    }
    if (IsInt()) {
        return static_cast<double>(std::get<int>(value_));
    }
    return std::get<double>(value_);
}

const std::string& Node::AsString() const {
    if (!IsString()) {
        throw JsonParsingError("String is expected"s);
    }
    return std::get<std::string>(value_);
}

const Array& Node::AsArray() const {
    if (!IsArray()) {
        throw JsonParsingError("Array is expected"s);
    }
    return std::get<Array>(value_);
}

const Dict& Node::AsMap() const {
    if (!IsMap()) {
        throw JsonParsingError("Map is expected"s);
    }
    return std::get<Dict>(value_);
}

const Node::Value& Node::GetValue() const {
    return value_;
}

bool Node::operator==(const Node& rhs) const {
    return value_ == rhs.value_;
}

bool Node::operator!=(const Node& rhs) const {
    return !(value_ == rhs.value_);
}

Document::Document(Node root)
    : root_(std::move(root)) {
}

const Node& Document::GetRoot() const {
    return root_;
}

bool Document::operator==(const Document& rhs) const {
    return root_ == rhs.root_;
}

bool Document::operator!=(const Document& rhs) const {
    return !(root_ == rhs.root_);
}

Document Load(istream& input) {
    return Document{ LoadNode(input) };
}

void PrintValue(std::nullptr_t, const PrintContext& context) {
    context.out << "null"sv;
}

void PrintValue(std::string value, const PrintContext& context) {
    context.out << "\""sv;

    for (const char& c : value) {

        if (c == '\n') {
            context.out << "\\n"sv;
            continue;
        }

        if (c == '\r') {
            context.out << "\\r"sv;
            continue;
        }

        if (c == '\"') {
            context.out << "\\"sv;
        }

        if (c == '\t') {
            context.out << "\t"sv;
            continue;
        }

        if (c == '\\') {
            context.out << "\\"sv;
        }

        context.out << c;
    }
    context.out << "\""sv;
}

void PrintValue(bool value, const PrintContext& context) {
    context.out << std::boolalpha << value;
}

void PrintValue(Array array, const PrintContext& context) {
    context.out << "[\n"sv;
    auto inner_context = context.Indented();
    bool first = true;
    for (const auto& elem : array) {
        if (first) {
            first = false;
        } else {
            context.out << ",\n"s;
        }
        inner_context.PrintIndent();
        PrintNode(elem, inner_context);
    }
    context.out << "\n"s;
    context.PrintIndent();
    context.out << "]"sv;
}

void PrintValue(Dict dict, const PrintContext& context) {
    context.out << "{\n"sv;
    auto inner_context = context.Indented();

    bool first = true;
    for (const auto& [key, node] : dict) {
        if (first) {
            first = false;
        } else {
            context.out << ",\n"s;
        }
        inner_context.PrintIndent();
        PrintValue(key, context);
        context.out << ": ";
        PrintNode(node, inner_context);
    }
    context.out << "\n"s;
    context.PrintIndent();
    context.out << "}"sv;
}

void PrintNode(const Node& node, const PrintContext& context) {
    std::visit(
        [&context](const auto& value) {
            PrintValue(value, context);
        }, node.GetValue()
    );
}

void Print(const Document& doc, std::ostream& output) {
    PrintContext context{ output };
    PrintNode(doc.GetRoot(), context);
}

}  // namespace json