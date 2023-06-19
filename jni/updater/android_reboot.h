#ifndef ANDROID_REBOOT_HEADER_INCLUDED
#define ANDROID_REBOOT_HEADER_INCLUDED

typedef mmlBool(*android_reboot_check_t)();

void android_updater_reboot_register_checker(android_reboot_check_t checker);

void android_updater_reboot_schedule(const mmlInt32 begin, const mmlInt32 end);

void android_updater_reboot();

#endif //ANDROID_REBOOT_HEADER_INCLUDED
