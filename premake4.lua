if os.is64bit() then
    THEDIR="x64"
else
    THEDIR="x86"
end

solution "positive"
    libdirs { "lib",                         -- ִ�г�������ʱlib��Ŀ¼
              "lib/" .. THEDIR}
    --   implibdir "lib"                          -- ���е�dll��lib�ⶼ���ɵ���Ŀ¼��
    --   defines { "_WIN32" }                     -- ��������Щ��
    configurations { "Debug", "Release" }    -- 
    flags { "NoIncrementalLink" }            -- ȡ����������

    configuration "Debug"
    defines { "DEBUG", "_DEBUG" }         -- PocoҪ�õ���'_DEBUG'
    flags { "Symbols" }
    objdir "build/intermediate/Debug"
    targetsuffix "d"

    configuration "Release"
    defines { "NDEBUG" }
    flags { "Optimize" }
    objdir "build/intermediate/Release"
	

include  "server"
include  "test"