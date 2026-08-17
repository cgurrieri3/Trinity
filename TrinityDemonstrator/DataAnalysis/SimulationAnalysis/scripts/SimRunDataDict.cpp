// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME SimRunDataDict
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "ROOT/RConfig.hxx"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Header files passed as explicit arguments
#include "SimRunData.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void *new_SimRunData(void *p = nullptr);
   static void *newArray_SimRunData(Long_t size, void *p);
   static void delete_SimRunData(void *p);
   static void deleteArray_SimRunData(void *p);
   static void destruct_SimRunData(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::SimRunData*)
   {
      ::SimRunData *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::SimRunData >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("SimRunData", ::SimRunData::Class_Version(), "SimRunData.h", 11,
                  typeid(::SimRunData), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::SimRunData::Dictionary, isa_proxy, 4,
                  sizeof(::SimRunData) );
      instance.SetNew(&new_SimRunData);
      instance.SetNewArray(&newArray_SimRunData);
      instance.SetDelete(&delete_SimRunData);
      instance.SetDeleteArray(&deleteArray_SimRunData);
      instance.SetDestructor(&destruct_SimRunData);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::SimRunData*)
   {
      return GenerateInitInstanceLocal(static_cast<::SimRunData*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::SimRunData*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr SimRunData::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *SimRunData::Class_Name()
{
   return "SimRunData";
}

//______________________________________________________________________________
const char *SimRunData::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::SimRunData*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int SimRunData::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::SimRunData*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *SimRunData::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::SimRunData*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *SimRunData::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::SimRunData*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void SimRunData::Streamer(TBuffer &R__b)
{
   // Stream an object of class SimRunData.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(SimRunData::Class(),this);
   } else {
      R__b.WriteClassBuffer(SimRunData::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_SimRunData(void *p) {
      return  p ? new(p) ::SimRunData : new ::SimRunData;
   }
   static void *newArray_SimRunData(Long_t nElements, void *p) {
      return p ? new(p) ::SimRunData[nElements] : new ::SimRunData[nElements];
   }
   // Wrapper around operator delete
   static void delete_SimRunData(void *p) {
      delete (static_cast<::SimRunData*>(p));
   }
   static void deleteArray_SimRunData(void *p) {
      delete [] (static_cast<::SimRunData*>(p));
   }
   static void destruct_SimRunData(void *p) {
      typedef ::SimRunData current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::SimRunData

namespace ROOT {
   static TClass *vectorlEfloatgR_Dictionary();
   static void vectorlEfloatgR_TClassManip(TClass*);
   static void *new_vectorlEfloatgR(void *p = nullptr);
   static void *newArray_vectorlEfloatgR(Long_t size, void *p);
   static void delete_vectorlEfloatgR(void *p);
   static void deleteArray_vectorlEfloatgR(void *p);
   static void destruct_vectorlEfloatgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<float>*)
   {
      vector<float> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<float>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<float>", -2, "vector", 389,
                  typeid(vector<float>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEfloatgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<float>) );
      instance.SetNew(&new_vectorlEfloatgR);
      instance.SetNewArray(&newArray_vectorlEfloatgR);
      instance.SetDelete(&delete_vectorlEfloatgR);
      instance.SetDeleteArray(&deleteArray_vectorlEfloatgR);
      instance.SetDestructor(&destruct_vectorlEfloatgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<float> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<float>","std::vector<float, std::allocator<float> >"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<float>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEfloatgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<float>*>(nullptr))->GetClass();
      vectorlEfloatgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEfloatgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEfloatgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<float> : new vector<float>;
   }
   static void *newArray_vectorlEfloatgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<float>[nElements] : new vector<float>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEfloatgR(void *p) {
      delete (static_cast<vector<float>*>(p));
   }
   static void deleteArray_vectorlEfloatgR(void *p) {
      delete [] (static_cast<vector<float>*>(p));
   }
   static void destruct_vectorlEfloatgR(void *p) {
      typedef vector<float> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<float>

namespace {
  void TriggerDictionaryInitialization_SimRunDataDict_Impl() {
    static const char* headers[] = {
"SimRunData.h",
nullptr
    };
    static const char* includePaths[] = {
"/opt/root/include/",
"/home/sstepanoff/TrinityDemonstrator/DataAnalysis/SimulationAnalysis/scripts/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "SimRunDataDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class __attribute__((annotate("$clingAutoload$SimRunData.h")))  SimRunData;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "SimRunDataDict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "SimRunData.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"SimRunData", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("SimRunDataDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_SimRunDataDict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_SimRunDataDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_SimRunDataDict() {
  TriggerDictionaryInitialization_SimRunDataDict_Impl();
}
