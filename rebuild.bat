@echo off

cmake -E remove_directory build
cmake -E make_directory build

call build.bat

