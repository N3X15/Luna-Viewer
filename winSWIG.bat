@echo OFF
swig -c++ -lua -o ".\indra\newview\LuaBase.cpp" ".\indra\newview\LuaBase.swig"
PAUSE