#include "precompiled.h"

#if _WIN32
#include <processthreadsapi.h>

bool try_large_pages = true;
bool large_pages_enabled = false;

struct deleter {
  deleter(std::size_t size) : size(size) {}
  void operator()(void *p) { VirtualFree(p, size, MEM_RELEASE); }
  std::size_t size;
};

struct token_handle {
  token_handle() : hToken(INVALID_HANDLE_VALUE) {
    if (!OpenProcessToken(GetCurrentProcess(),
          TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
      // DWORD error = GetLastError();
      // std::cout << "OpenProcessToken error " << error << std::endl;
      throw std::runtime_error("OpenProcessToken failed");
    }
  }

  ~token_handle() {
    if (hToken != INVALID_HANDLE_VALUE) {
      if (!CloseHandle(hToken)) {
        // DWORD error = GetLastError();
        // std::cout << "CloseHandle error " << error << std::endl;
      }
    }
  }
  HANDLE operator()() const { return hToken; }
  HANDLE hToken;
};

template <typename T>
std::unique_ptr<T[], deleter> allocate_large_array(std::size_t count) {

  void *p = nullptr;
  if (try_large_pages && !large_pages_enabled) {
    try {
      SIZE_T page_size = GetLargePageMinimum();
      if (!page_size) {
        // std::cout << "No support for large pages" << std::endl;
        throw std::runtime_error("Large pages not supported");
      }
      token_handle token;
      TOKEN_PRIVILEGES tp{1, {{{}, SE_PRIVILEGE_ENABLED}}};
      if (!LookupPrivilegeValue(
            NULL, "SeLockMemoryPrivilege", &tp.Privileges[0].Luid)) {
        // DWORD error = GetLastError();
        // std::cout << "LookupPrivilegeValue error " << error << std::endl;
        throw std::runtime_error("LookupPrivilegeValue failed");
      }
      int status = AdjustTokenPrivileges(token(), FALSE, &tp, 0, nullptr, 0);
      int error = GetLastError();
      if (!status || error) {
        // std::cout << "AdjustTokenPrivileges error " << error << std::endl;
        throw std::runtime_error("AdjustTokenPrivileges failed");
      }
      large_pages_enabled = true;
    } catch (std::runtime_error &e) {
      try_large_pages = false;
      std::cout << "Large pages not available, " << e.what() << std::endl;
    }
  }
  if (try_large_pages) {
    std::size_t page_size = GetLargePageMinimum();
    std::size_t size = (count * sizeof(T) + (page_size - 1)) & -page_size;
    p = VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE | MEM_LARGE_PAGES,
      PAGE_READWRITE);
    if (p) {
      return std::unique_ptr<T[], deleter>((T *)p, deleter(size));
    }
    try_large_pages = false;
  }
  std::size_t size = count * sizeof(T);
  p = VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  if (p) {
    return std::unique_ptr<T[], deleter>((T *)p, deleter(size));
  }
  DWORD error = GetLastError();
  std::cout << "VirtualAlloc error " << error << std::endl;
  std::exit(1);
}

#else

// TODO: huge page support not implemented
template <typename T>
std::unique_ptr<T[]> allocate_large_array(std::size_t count) {
  return std::make_unique<T[]>(count);
}

#endif
