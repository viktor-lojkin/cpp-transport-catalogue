#pragma once

#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace json {

class Node;
using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;

class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class Node {
public:
    using Value = std::variant<std::nullptr_t, std::string, int, double, bool, Array, Dict>;
    
    Node() = default;
    Node(std::nullptr_t);
    Node(std::string value);
    Node(int value);
    Node(double value);
    Node(bool value);
    Node(Array array);
    Node(Dict map);   

    bool IsInt() const;
    bool IsDouble() const;
    bool IsDoubleOrInt() const;
    bool IsBool() const;
    bool IsNull() const;
    bool IsArray() const;
    bool IsString() const;
    bool IsDict() const;
    
    int AsInt() const;
    double AsDouble() const;
    bool AsBool() const;
    const Array& AsArray() const;
    const std::string& AsString() const;
    const Dict& AsDict() const;
    
    bool operator==(const Node& rhs) const;
    bool operator!=(const Node& rhs) const;

    const Value& GetValue() const;
    Value& GetValue();
    
private:
    Value value_;
    
};

class Document {
public:
    explicit Document(Node root)
        : root_(std::move(root)) {
    }

    const Node& GetRoot() const {
        return root_;
    }

private:
    Node root_;
};

inline bool operator==(const Document& lhs, const Document& rhs) {
    return lhs.GetRoot() == rhs.GetRoot();
}

inline bool operator!=(const Document& lhs, const Document& rhs) {
    return !(lhs == rhs);
}

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
void PrintValue(const Value& value, const PrintContext& ctx) {
    ctx.out << value;
}
    
void PrintValue(const std::string& value, const PrintContext& ctx);
void PrintValue(const std::nullptr_t&, const PrintContext& ctx);
void PrintValue(const bool& value, const PrintContext& ctx);
void PrintValue(const Array& nodes, const PrintContext& ctx);
void PrintValue(const Dict& nodes, const PrintContext& ctx);
    
void PrintNode(const Node& value, const PrintContext& ctx);    
void PrintString(const std::string& value, std::ostream& out);
    
void Print(const Document& doc, std::ostream& output);

}  // namespace json