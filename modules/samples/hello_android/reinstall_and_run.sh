
APK_PATH=apk/armeabi-v7a/bin/hello_android-debug.apk
PACKAGE=com.sample.hello_android
ACTIVITY=MyNativeActivity

adb -d uninstall $PACKAGE
adb -d install $APK_PATH
adb -d shell am start -n $PACKAGE/.$ACTIVITY
adb -d logcat $PACKAGE:v *:s

