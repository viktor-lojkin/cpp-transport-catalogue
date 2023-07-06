#pragma once

#include <string>
#include <vector>
#include <map>
#include <variant>

#include <iostream>


namespace json {

class Node;

using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;

class JsonParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class Node {
private:
    using Value = std::variant<std::nullptr_t, std::string, int, double, bool, Array, Dict>;
    Value value_;

public:
    Node() = default;
    Node(std::nullptr_t);
    Node(std::string value);
    Node(int value);
    Node(double value);
    Node(bool value);
    Node(Array array);
    Node(Dict map);

    bool IsInt() const;
    bool IsDoubleOrInt() const;
    bool IsBool() const;
    bool IsString() const;
    bool IsNull() const;
    bool IsArray() const;
    bool IsMap() const;

    int AsInt() const;
    bool AsBool() const;
    double AsDouble() const;
    const std::string& AsString() const;
    const Array& AsArray() const;
    const Dict& AsMap() const;

    const Value& GetValue() const;

    bool operator==(const Node& rhs) const;
    bool operator!=(const Node& rhs) const;
};

class Document {
public:
    explicit Document(Node root);

    const Node& GetRoot() const;

    bool operator==(const Document& rhs) const;
    bool operator!=(const Document& rhs) const;

private:
    Node root_;
};

Document Load(std::istream& input);

struct PrintContext {
    std::ostream& out;
    int indent_step = 4;
    int indent = 0;

    void PrintIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    PrintContext Indented() const {
        return { out, indent_step, indent_step + indent };
    }
};

template <typename Value>
void PrintValue(const Value& value, const PrintContext& context) {
    context.out << value;
}

void PrintValue(std::nullptr_t, const PrintContext& context);
void PrintValue(std::string value, const PrintContext& ccontexttx);
void PrintValue(bool value, const PrintContext& context);
void PrintValue(Array array, const PrintContext& context);
void PrintValue(Dict dict, const PrintContext& context);

void PrintNode(const Node& node, const PrintContext& context);

void Print(const Document& doc, std::ostream& output);

}  // namespace json