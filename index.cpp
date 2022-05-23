#include "esptk/src/espfile.h"
#include <vector>
#include <napi.h>
#include <string>
#include "string_cast.h"
#include <filesystem>
#include "esptk/src/espexceptions.h"

namespace fs = std::filesystem;

const std::wstring exstring = L".tmp";

void rethrow(const Napi::Env &env, const std::string &filePath, const std::exception &e) {
  napi_value err;
  napi_status s = napi_create_error(
    env,
    Napi::String::New(env, "UNKNOWN"),
    Napi::String::New(env, e.what()),
    &err);

  if (s != napi_ok) {
    throw Napi::Error::New(env, e.what());
  }

  napi_set_property(env, err, Napi::String::New(env, "path"), Napi::String::New(env, filePath.c_str()));

  try {
    throw;
  }
  catch (const ESP::FileMissingException& e) {
    napi_set_property(env, err, Napi::String::New(env, "name"), Napi::String::New(env, "FileMissingError"));
    napi_set_property(env, err, Napi::String::New(env, "code"), Napi::String::New(env, "ENOENT"));
  }
  catch (const ESP::InvalidRecordException& e) {
    napi_set_property(env, err, Napi::String::New(env, "name"), Napi::String::New(env, "InvalidRecordError"));
    napi_set_property(env, err, Napi::String::New(env, "code"), Napi::String::New(env, "EINVAL"));
  }
  catch (const ESP::InvalidFileException& e) {
    napi_set_property(env, err, Napi::String::New(env, "name"), Napi::String::New(env, "InvalidFileError"));
    napi_set_property(env, err, Napi::String::New(env, "code"), Napi::String::New(env, "EINVAL"));
  }
  catch (const std::exception& e) {
    // nop
  }
  napi_throw(env, err);
}

class ESPFile : public Napi::ObjectWrap<ESPFile> {
private:
#if defined(_WIN32)
  std::wstring m_FileName;
#else
  std::string m_FileName;
#endif
  bool m_IsMaster;
  bool m_IsLight;
  bool m_IsDummy;
  uint32_t m_Revision;
  std::string m_Author;
  std::string m_Description;
  std::vector<std::string> m_Masters;

public:

  static Napi::Object Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "ESPFile", {
      InstanceMethod("setLightFlag", &ESPFile::setLightFlag),
      InstanceAccessor("isMaster", &ESPFile::isMaster, nullptr, napi_enumerable),
      InstanceAccessor("isLight", &ESPFile::isLight, nullptr, napi_enumerable),
      InstanceAccessor("isDummy", &ESPFile::isDummy, nullptr, napi_enumerable),
      InstanceAccessor("author", &ESPFile::author, nullptr, napi_enumerable),
      InstanceAccessor("description", &ESPFile::description, nullptr, napi_enumerable),
      InstanceAccessor("masters", &ESPFile::masters, nullptr, napi_enumerable),
      InstanceAccessor("masterList", &ESPFile::masterList, nullptr, napi_enumerable),
      InstanceAccessor("revision", &ESPFile::revision, nullptr, napi_enumerable),
      });
    Napi::FunctionReference* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    exports.Set("ESPFile", func);

    env.SetInstanceData<Napi::FunctionReference>(constructor);
    return exports;
  }

  static Napi::Object CreateNewItem(Napi::Env env) {
    Napi::FunctionReference* constructor = env.GetInstanceData<Napi::FunctionReference>();
    return constructor->New({ });
  }

  ESPFile(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<ESPFile>(info)
  {
    setFileName(info.Env(), info[0].ToString().Utf8Value());
  }

  Napi::Value setLightFlag(const Napi::CallbackInfo &info) {
    try {
      bool enable = info[0].ToBoolean();
      {
        ESP::File file(m_FileName);
        file.setLight(enable);
        file.write(m_FileName + exstring);
      }
      fs::rename(m_FileName + exstring, m_FileName);
    }
    catch (const std::exception& e) {
      rethrow(info.Env(), toMB(m_FileName.c_str(), CodePage::UTF8, m_FileName.length()), e);
    }

    return info.Env().Undefined();
  }

  Napi::Value isMaster(const Napi::CallbackInfo &info) { return Napi::Boolean::New(info.Env(), m_IsMaster); }
  Napi::Value isLight(const Napi::CallbackInfo &info) { return Napi::Boolean::New(info.Env(), m_IsLight); }
  Napi::Value isDummy(const Napi::CallbackInfo &info) { return Napi::Boolean::New(info.Env(), m_IsDummy); }
  Napi::Value author(const Napi::CallbackInfo &info) { return Napi::String::New(info.Env(), m_Author); }
  Napi::Value description(const Napi::CallbackInfo &info) { return Napi::String::New(info.Env(), m_Description); }
  Napi::Value masters(const Napi::CallbackInfo &info) {
    Napi::Array result = Napi::Array::New(info.Env());
    int idx = 0;
    for (const std::string &master : m_Masters) {
      result.Set(idx++, Napi::String::New(info.Env(), master));
    }

    return result;
  }
  Napi::Value masterList(const Napi::CallbackInfo &info) { return masters(info); }
  Napi::Value revision(const Napi::CallbackInfo &info) { return Napi::Number::New(info.Env(), m_Revision); }

private:

  void setFileName(const Napi::Env &env, const std::string &fileName)
  {
    try {
#if defined(_WIN32)
      m_FileName = toWC(fileName.c_str(), CodePage::UTF8, fileName.size());
#else
      m_FileName = fileName.c_str();
#endif

      ESP::File wrapped(m_FileName);
      m_IsMaster = wrapped.isMaster();
      m_IsLight = wrapped.isLight();
      m_IsDummy = wrapped.isDummy();
      m_Author = wrapped.author();
      m_Description = wrapped.description();
      m_Revision = wrapped.revision();

      std::set<std::string> input = wrapped.masters();
      std::copy(input.begin(), input.end(), std::back_inserter(m_Masters));
    }
    catch (const std::exception& e) {
      rethrow(env, toMB(m_FileName.c_str(), CodePage::UTF8, m_FileName.length()), e);
      // throw Napi::Error::New(env, e.what());
    };
  }
};

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  ESPFile::Init(env, exports);
  return exports;
}

NODE_API_MODULE(ESPFile, InitAll)
