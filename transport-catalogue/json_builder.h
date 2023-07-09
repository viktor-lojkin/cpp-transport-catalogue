#pragma once

#include "json.h"

#include <optional>

namespace json {

class Builder {
private:
    Node root_{ nullptr };
    std::vector<Node*> nodes_stack_;
    std::optional<std::string> key_{ std::nullopt };

public:
    class DictItemContext;
    class DictKeyContext;
    class ArrayItemContext;

    Builder();
    DictKeyContext Key(std::string key);
    Builder& Value(Node::Value value);
    DictItemContext StartDict();
    Builder& EndDict();
    ArrayItemContext StartArray();
    Builder& EndArray();
    Node Build();
    Node GetNode(Node::Value value);
};

class Builder::DictItemContext {
private:
    Builder& builder_;

public:
    DictItemContext(Builder& builder);

    DictKeyContext Key(std::string key);
    Builder& EndDict();
};

class Builder::ArrayItemContext {
private:
    Builder& builder_;

public:
    ArrayItemContext(Builder& builder);

    ArrayItemContext Value(Node::Value value);
    DictItemContext StartDict();
    Builder& EndArray();
    ArrayItemContext StartArray();
};

class Builder::DictKeyContext {
private:
    Builder& builder_;

public:
    DictKeyContext(Builder& builder);

    DictItemContext Value(Node::Value value);
    ArrayItemContext StartArray();
    DictItemContext StartDict();
};

} // namespace json