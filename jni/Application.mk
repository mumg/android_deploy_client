APP_OPTIM := release
APP_PLATFORM := android-21
APP_STL := c++_static
APP_CPPFLAGS += -frtti -fPIE -fexceptions -fPIC -DANDROID_WCHAR -DANDROID_GOOGLE -DANDROID
APP_ABI := armeabi-v7a arm64-v8a x86_64
APP_MODULES := android_updater_module android_updater
