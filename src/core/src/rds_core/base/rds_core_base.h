#pragma once

#include "rds_core-config.h"

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#endif

#include "../detect_platform/rds_detect_platform.h"

#if RDS_OS_WINDOWS
#define NOMINMAX 1
#include <WinSock2.h> // WinSock2.h must include before windows.h to avoid winsock1 define
#include <ws2tcpip.h> // struct sockaddr_in6
#pragma comment(lib, "Ws2_32.lib")
#include <Windows.h>
#include <intsafe.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h> // struct sockaddr_in
#endif

#include <cassert>
#include <exception>
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <atomic>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>
#include <algorithm>
#include <functional>

#include <limits>
#include <stdint.h>
#include <exception>
#include <stdio.h>
#include <signal.h>
#include <codecvt>
#include <limits>

#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <nmsp_core.h>
