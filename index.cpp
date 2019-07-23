#include "esptk/src/espfile.h"
#include "nbind/nbind.h"
#include <vector>
#include <string>
#if defined(_WIN32)
#include "string_cast.h"
#include <filesystem>
namespace fs = std::filesystem;

const std::wstring exstring = L".tmp";
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

const std::string exstring = ".tmp";
#endif

class ESPFile {
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
  
  ESPFile(const std::string &fileName)
#if defined(_WIN32)
    : m_FileName(toWC(fileName.c_str(), CodePage::UTF8, fileName.size()))
#else
    : m_FileName(fileName.c_str())
#endif
  {
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
  
  void setLightFlag(bool enable) {
    {
      ESP::File file(m_FileName);
      file.setLight(enable);
      file.write(m_FileName + exstring);
    }
    fs::rename(m_FileName + exstring, m_FileName);
  }

  bool isMaster() const { return m_IsMaster; }
  bool isLight() const { return m_IsLight; }
  bool isDummy() const { return m_IsDummy; }
  std::string author() const { return m_Author; }
  std::string description() const { return m_Description; }
  std::vector<std::string> masters() const { return m_Masters; }
  std::vector<std::string> masterList() const { return m_Masters; }
  float revision() const { return m_Revision; }
};

NBIND_CLASS(ESPFile) {
  construct<std::string>();
  method(setLightFlag);
  getter(isMaster);
  getter(isLight);
  getter(isDummy);
  getter(author);
  getter(description);
  getter(masters);
  getter(masterList);
  getter(revision);
}
