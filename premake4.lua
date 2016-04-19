if os.is64bit() then
    THEDIR="x64"
else
    THEDIR="x86"
end

solution "positive"
    libdirs { "lib",                         -- 执行程序链接时lib的目录
              "lib/" .. THEDIR}
    --   implibdir "lib"                          -- 所有的dll的lib库都生成到此目录下
    --   defines { "_WIN32" }                     -- 定义了哪些宏
    configurations { "Debug", "Release" }    -- 
    flags { "NoIncrementalLink" }            -- 取消增量连接

    configuration "Debug"
    defines { "DEBUG", "_DEBUG" }         -- Poco要用到宏'_DEBUG'
    flags { "Symbols" }
    objdir "build/intermediate/Debug"
    targetsuffix "d"

    configuration "Release"
    defines { "NDEBUG" }
    flags { "Optimize" }
    objdir "build/intermediate/Release"
	

include  "server"
include  "test"