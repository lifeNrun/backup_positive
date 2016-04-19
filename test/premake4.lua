project "test"
   kind "ConsoleApp"
   targetname "test"
   defines { "SUPPORT_STATIC" }
   language "C++"

   files { 
--      "src/parserIni.cpp",
         "src/test.cpp",
	  "../include/iniparser/*",
      "include/*.h"
   }
if  _ACTION == "gmake" then
   links { "iniparser","pthread", "dl" }
--   links { "PocoDataSQLite", "PocoData", "PocoNet", "PocoXML", "PocoUtil", "PocoFoundation","inotifytools", "protobuf", "protobuf-lite", "pcap", "pthread", "dl" }
   excludes {
      "src/Protect.cpp",
      "src/WinSystemInfo.cpp",
      "src/RegMonitor.cpp",
      "src/SystemInfo_POSIX.cpp",
      "src/SystemInfo_WIN.cpp",
      "src/WGMDriver_POSIX.cpp",
      "src/WGMDriver_WIN.cpp",
      "../MonitorIPC/MonitorIPC.cpp",
      "../MonitorIPC/MonitorIPCApp.cpp",
      "../wglib/FileWatcherWin32.cpp", 
      "../wglib/FileWatcherOSX.cpp",
      "../wglib/ParseFun.cpp",
      "src/linuxserverstate.cpp"
   }
else
   links { "protobuf", "protobuf-lite", "fltLib" }
   excludes {
      "src/Protect.cpp", 
	  "src/linuxserverstate.cpp",
      "src/WGMDriver_WIN.cpp",
      "src/WGMDriver_POSIX.cpp",
      "../MonitorIPC/MonitorIPC.cpp",
      "../MonitorIPC/MonitorIPCApp.cpp",
      "../wglib/FileWatcherLinux.cpp", 
      "../wglib/FileWatcherOSX.cpp",
      "../wglib/ParseFun.cpp"
   }
end
includedirs { "include", "../include/iniparser","../include"}
objdir "intermediate"
