#ifndef RBX_LLVM_TYPES_HPP
#define RBX_LLVM_TYPES_HPP

namespace llvm {
  class Instruction;
  class Value;
}

namespace rubinius {
  class Context;
  class GlobalCacheEntry;

  namespace type {
    enum Kind {
      eUnknown = 0,
      eTrue,
      eFalse,
      eNil,
      eFixnum,
      eStaticFixnum,
      eInstance,
      eSingletonInstance,
      eSymbol,
      eType,
      eClassObject,
      eGlobalCacheEntry
    };

    enum Source {
      eUnknownSource = 0,
      eLocal = 1
    };

    class KnownType {
      Kind kind_;
      uintptr_t value_;

      Source source_;
      int source_id_;

    public:
      KnownType()
        : kind_(eUnknown)
        , value_(0)
        , source_(eUnknownSource)
        , source_id_(0)
      {}

      KnownType(Kind kind, uintptr_t value=0, Source s=eUnknownSource, int sid=0)
        : kind_(kind)
        , value_(value)
        , source_(s)
        , source_id_(sid)
      {}

      static KnownType unknown() {
        return KnownType(eUnknown);
      }

      static KnownType instance(int class_id) {
        return KnownType(eInstance, class_id);
      }

      static KnownType singleton_instance(int class_id) {
        return KnownType(eSingletonInstance, class_id);
      }

      static KnownType nil() {
        return KnownType(eNil);
      }

      static KnownType true_() {
        return KnownType(eTrue);
      }

      static KnownType false_() {
        return KnownType(eFalse);
      }

      static KnownType fixnum(int val) {
        return KnownType(eStaticFixnum, val);
      }

      static KnownType fixnum() {
        return KnownType(eFixnum);
      }

      static KnownType symbol() {
        return KnownType(eSymbol);
      }

      static KnownType type() {
        return KnownType(eType);
      }

      static KnownType class_object(int class_id) {
        return KnownType(eClassObject, class_id);
      }

      static KnownType global_cache_entry(GlobalCacheEntry* entry) {
        return KnownType(eGlobalCacheEntry, reinterpret_cast<uintptr_t>(entry));
      }

      bool known_p() {
        return kind_ != eUnknown;
      }

      bool unknown_p() {
        return kind_ == eUnknown;
      }

      Kind kind() {
        return kind_;
      }

      int value() {
        return value_;
      }

      bool instance_p() {
        return kind_ == eInstance || kind_ == eSingletonInstance;
      }

      bool singleton_instance_p() {
        return kind_ == eSingletonInstance;
      }

      bool class_p() {
        return kind_ == eClassObject;
      }

      bool global_cache_entry_p() {
        return kind_ == eGlobalCacheEntry;
      }

      int class_id() {
        switch(kind_) {
        case eInstance:
        case eSingletonInstance:
        case eClassObject:
          return value_;
        default:
          return -1;
        }
      }

      GlobalCacheEntry* global_cache_entry() {
        if(global_cache_entry_p()) {
          return reinterpret_cast<GlobalCacheEntry*>(value_);
        }
        return NULL;
      }

      bool fixnum_p() {
        return kind_ == eStaticFixnum || kind_ == eFixnum;
      }

      bool static_fixnum_p() {
        return kind_ == eStaticFixnum;
      }

      bool symbol_p() {
        return kind_ == eSymbol;
      }

      bool type_p() {
        return kind_ == eType;
      }

      void set_local_source(int id) {
        source_ = eLocal;
        source_id_ = id;
      }

      Source source() {
        return source_;
      }

      bool local_source_p() {
        return source_ == eLocal;
      }

      int local_id() {
        if(source_ != eLocal) return -1;
        return source_id_;
      }

      int source_id() {
        return source_id_;
      }

      const char* describe();

      void associate(Context* ctx, llvm::Instruction* I);
      void associate(Context* ctx, llvm::Value* V);

      static KnownType extract(Context* ctx, llvm::Instruction* I);
      static KnownType extract(Context* ctx, llvm::Value* I);

      static bool has_hint(Context* ctx, llvm::Value* V);

      void inherit_source(Context* ctx, llvm::Value* V);
      void inherit_source(type::KnownType kt);
    };
  }
}

#endif
