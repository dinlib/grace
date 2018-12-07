//
// Created by Guilherme Souza on 11/22/18.
//

#include <Context.hh>

Type *Context::getLLVMType(std::string &TypeRepresentation) {

  if (TypeRepresentation == "type_int")
    return Type::getInt32Ty(getContext());

  if (TypeRepresentation == "type_bool")
    return Type::getInt1Ty(getContext());

  // TODO: Check for type_string

  return nullptr;
}

void Context::enterScope() {
  std::map<std::string, AllocaInst *> NewScope;
  NamedValues.push_front(NewScope);
}

void Context::leaveScope() { NamedValues.pop_front(); }

AllocaInst *Context::getNamedValueInScope(std::string &Name) {
  for (auto &Scope : NamedValues) {
    auto It = Scope.find(Name);
    if (It != Scope.end())
      return It->second;
  }

  return nullptr;
}

void Context::insertNamedValueIntoScope(const std::string &Name,
                                        AllocaInst *V) {
  NamedValues.front()[Name] = V;
}

std::string Context::getType(Type *T) {
  if (T->isIntegerTy(INT_SIZE))
    return "int";

  if (T->isIntegerTy(BOOL_SIZE))
    return "bool";

  return "";
}

bool Context::typeCheck(Type *A, Type *B) {
  if (A->getTypeID() != B->getTypeID())
    return false;

  switch (A->getTypeID()) {
  case Type::TypeID::IntegerTyID:
    return A->getIntegerBitWidth() == B->getIntegerBitWidth();
  case Type::TypeID::PointerTyID:
    return typeCheck(A->getPointerElementType(), B->getPointerElementType());
  case Type::VoidTyID:
    break;
  case Type::HalfTyID:
    break;
  case Type::FloatTyID:
    break;
  case Type::DoubleTyID:
    break;
  case Type::X86_FP80TyID:
    break;
  case Type::FP128TyID:
    break;
  case Type::PPC_FP128TyID:
    break;
  case Type::LabelTyID:
    break;
  case Type::MetadataTyID:
    break;
  case Type::X86_MMXTyID:
    break;
  case Type::TokenTyID:
    break;
  case Type::FunctionTyID:
    break;
  case Type::StructTyID:
    break;
  case Type::ArrayTyID:
    break;
  case Type::VectorTyID:
    break;
  }

  return false;
}