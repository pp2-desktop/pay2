#include <log4cxx/logger.h>
#include <log4cxx/xml/domconfigurator.h>

using namespace log4cxx;
using namespace log4cxx::xml;
using namespace log4cxx::helpers;

static LoggerPtr __root__logger__(Logger::getLogger("main"));
static LoggerPtr __app__logger__(Logger::getLogger("app"));
static LoggerPtr __packet__logger__(Logger::getLogger("packet"));

// 기본(루트) 로깅 시 사용
/*
#define logt(m) (LOG4CXX_TRACE(__root__logger__, m))
#define logd(m) (LOG4CXX_DEBUG(__root__logger__, m))
#define logi(m) (LOG4CXX_INFO (__root__logger__, m))
#define logw(m) (LOG4CXX_WARN (__root__logger__, m))
#define loge(m) (LOG4CXX_ERROR(__root__logger__, m))
#define logf(m) (LOG4CXX_FATAL(__root__logger__, m))
*/

#define _logt(m) (LOG4CXX_TRACE(__app__logger__, m))
#define _logd(m) (LOG4CXX_DEBUG(__app__logger__, m))
#define _logi(m) (LOG4CXX_INFO (__app__logger__, m))
#define _logw(m) (LOG4CXX_WARN (__app__logger__, m))
#define _loge(m) (LOG4CXX_ERROR(__app__logger__, m))
#define _logf(m) (LOG4CXX_FATAL(__app__logger__, m))

#define _plogt(m) (LOG4CXX_TRACE(__packet__logger__, m))
#define _plogd(m) (LOG4CXX_DEBUG(__packet__logger__, m))
#define _plogi(m) (LOG4CXX_INFO (__packet__logger__, m))
#define _plogw(m) (LOG4CXX_WARN (__packet__logger__, m))
#define _ploge(m) (LOG4CXX_ERROR(__packet__logger__, m))
#define _plogf(m) (LOG4CXX_FATAL(__packet__logger__, m))

class log4cxx_md {
public:
  log4cxx_md();
  ~log4cxx_md();

  void init();

  static log4cxx_md& get() {
    static log4cxx_md obj;
    return obj;
  }

};
