# gammaServer
A server framework with multi threads. Base on epoll and provides socket,http,https,curl. Can be easily extend with C++ or Scripts(Lua/JavaScript/Python or others). Support Linux Only.

# How to Build
1) install curl (version >= 7.50.1). git@github.com:curl/curl.git 
2) git clone git@github.com:jemalloc/jemalloc.git third/jemalloc/ , and build jemalloc
3) goto src/gamma and build

# developing plans
1) complete and test framework
2) add C++ module
3) add lua/luajit module
4) add javascript(V8 engine) module
5) add python module

