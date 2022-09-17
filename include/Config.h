#include <SKSE/SKSE.h>

namespace DetachedLightning {
    class Debug {
    public:
        spdlog::level::level_enum GetLogLevel() const { return m_logLevel; }
        spdlog::level::level_enum GetFlushLevel() const { return m_flushLevel; }

    private:
        spdlog::level::level_enum m_logLevel{spdlog::level::level_enum::info};
        spdlog::level::level_enum m_flushLevel{spdlog::level::level_enum::trace};
    };

    class Config {
    public:
        const Debug& GetDebug() const { return m_debug; };
        static const Config& GetSingleton();

    private:
        Debug m_debug;
    };
}  // namespace DetachedLightning