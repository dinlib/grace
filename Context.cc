//
// Created by Guilherme Souza on 11/22/18.
//

#include <Context.hh>

Type *Context::getLLVMType(std::string &TypeRepresentation) {

    if (TypeRepresentation == "type_int")
        return Type::getInt64Ty(getContext());

    if (TypeRepresentation == "type_bool")
        return Type::getInt1Ty(getContext());

    // TODO: Check for type_string

    return nullptr;
}

void Context::enterScope() {
    std::map<std::string, Value *> NewScope;
    NamedValues.push_front(NewScope);
}

void Context::leaveScope() {
    NamedValues.pop_front();
}

Value *Context::getNamedValueInScope(std::string &Name) {
    for (auto &Scope : NamedValues) {
        auto It = Scope.find(Name);
        if (It != Scope.end())
            return It->second;
    }

    return nullptr;
}

void Context::insertNamedValueIntoScope(const std::string &Name, Value *V) {
    NamedValues.front()[Name] = V;
}


