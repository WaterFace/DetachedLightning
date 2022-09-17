#include "Config.h"

using namespace DetachedLightning;

const Config& Config::GetSingleton() {
    static Config instance;
    return instance;
}