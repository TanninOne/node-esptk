#include "esptk/src/espfile.h"
#include "nbind/nbind.h"
#include "string_cast.h"
#include <vector>
#include <string>

class ESPFile {
private:
  bool m_IsMaster;
  bool m_IsLight;
  bool m_IsDummy;
  std::string m_Author;
  std::string m_Description;
  std::vector<std::string> m_Masters;

public:
  ESPFile(const std::string &fileName) {
    ESP::File wrapped(toWC(fileName.c_str(), CodePage::UTF8, fileName.size()));
    m_IsMaster = wrapped.isMaster();
    m_IsLight = wrapped.isLight();
    m_IsDummy = wrapped.isDummy();
    m_Author = wrapped.author();
    m_Description = wrapped.description();

    std::set<std::string> input = wrapped.masters();
    std::copy(input.begin(), input.end(), std::back_inserter(m_Masters));
  }

  bool isMaster() const { return m_IsMaster; }
  bool isLight() const { return m_IsLight; }
  bool isDummy() const { return m_IsDummy; }
  std::string author() const { return m_Author; }
  std::string description() const { return m_Description; }
  std::vector<std::string> masters() const { return m_Masters; }
  std::vector<std::string> masterList() const { return m_Masters; }
};

NBIND_CLASS(ESPFile) {
  construct<std::string>();
  getter(isMaster);
  getter(isLight);
  getter(isDummy);
  getter(author);
  getter(description);
  getter(masters);
  getter(masterList);
}
