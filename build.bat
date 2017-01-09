@echo off

set COMPILER="Visual Studio 14 2015"
set BUILD_TYPE=RelWithDebInfo

set THIRD_PARTY_DIR=D:\\Develop\\3rd_party

set IGLOO_ROOT_DIR=-DIGLOO_ROOT_DIR:PATH="%THIRD_PARTY_DIR%\\igloo\\"
set JSON_ROOT_DIR=-DJSONCPP_ROOT_DIR:PATH="%THIRD_PARTY_DIR%\\jsoncpp-master\\"
set VORBIS_ROOT_DIR=-DVORBIS_ROOT_DIR:PATH="%THIRD_PARTY_DIR%\\libvorbis-1.3.5\\"
set OGG_ROOT_DIR=-DOGG_ROOT_DIR:PATH="%THIRD_PARTY_DIR%\\libogg-1.3.2\\"
set OPENSSL_DIRS=-DOPENSSL_ROOT_DIR:PATH="%THIRD_PARTY_DIR%\\OpenSSL\\"
set CURL_DIRS=-DCURL_INCLUDE_DIR:PATH="%THIRD_PARTY_DIR%\\curl-7.51.0\\include" -DCURL_LIBRARY:PATH="%THIRD_PARTY_DIR%\\curl-7.51.0\\libs\libcurl.lib"

set LUA_DIR=%THIRD_PARTY_DIR%\\LuaJIT-2.0.4\\src\\

cmake -E chdir build cmake -G%COMPILER% -DSUBSYSTEM=GUI %IGLOO_ROOT_DIR% %JSON_ROOT_DIR% %VORBIS_ROOT_DIR% %OGG_ROOT_DIR% %CURL_DIRS% %OPENSSL_DIRS% ..
cmake --build build --config %BUILD_TYPE% -- /verbosity:minimal

