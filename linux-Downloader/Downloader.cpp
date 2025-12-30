#include "UpgradeApi.h"
#include "UpgradeApiLinux.h"

int main(const int argc, const char** argv)
{
    return start_jni_app(CLASS_NAME_DOWNLOADER, argc, argv);
}
