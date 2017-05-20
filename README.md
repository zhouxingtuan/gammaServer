# gammaServer
A server framework with multi threads. Base on epoll and provides socket,http,https,curl. Can be easily extend with C++ or Scripts(Lua/JavaScript/Python or others). Support Linux Only.

# How to Build
1) install curl (version >= 7.50.1). https://github.com/curl/curl.git 
2) git clone https://github.com/jemalloc/jemalloc.git third/jemalloc/ , and build jemalloc
3) goto src/gamma and build

# client connection
codes was in the client directory. main.cpp shows the way to use the client. 
depends on pthread, Support Linux/Mac/IOS/Android.

# developing plans
1) complete and test framework (done)
2) add C++ module (done)
3) add lua/luajit module (done, testing)
4) add javascript(V8 engine) module (waiting)
5) add python module (ing...)

