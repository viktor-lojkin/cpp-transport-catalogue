#pragma once

#include <vector>
#include <string>
#include <map>
#include <variant>

#include <iostream>

namespace json {

class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};
    
class Node;
using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;

class Node {
public:
    using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;
    
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
    bool IsBool() const;
    bool IsNull() const;
    bool IsString() const;
    bool IsArray() const;
    bool IsDict() const;
    
    int AsInt() const;
    double AsDouble() const;
    bool AsBool() const;
    const std::string& AsString() const;
    const Array& AsArray() const;
    const Dict& AsDict() const;

    bool operator==(const Node& rhs) const;
    
    const Value& GetValue() const;
    Value& GetValue();
        
private:
    Value value_;
    
    bool IsPureDouble() const;
};

inline bool operator!=(const Node& lhs, const Node& rhs) {
    return !(lhs == rhs);
}

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

void Print(const Document& doc, std::ostream& output);

} // namespace json