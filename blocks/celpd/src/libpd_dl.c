#include "libpd_dl.h"

#ifdef WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
// #include <android/log.h>
#endif

PFN_LIBPD_INIT DL_libpd_init;
PFN_LIBPD_CLEAR_SEARCH_PATH DL_libpd_clear_search_path;
PFN_LIBPD_ADD_TO_SEARCH_PATH DL_libpd_add_to_search_path;
PFN_LIBPD_OPENFILE DL_libpd_openfile;
PFN_LIBPD_CLOSEFILE DL_libpd_closefile;
PFN_LIBPD_GETDOLLARZERO DL_libpd_getdollarzero;
PFN_LIBPD_BLOCKSIZE DL_libpd_blocksize;
PFN_LIBPD_INIT_AUDIO DL_libpd_init_audio;
PFN_LIBPD_PROCESS_RAW DL_libpd_process_raw;
PFN_LIBPD_PROCESS_SHORT DL_libpd_process_short;
PFN_LIBPD_PROCESS_FLOAT DL_libpd_process_float;
PFN_LIBPD_PROCESS_DOUBLE DL_libpd_process_double;
PFN_LIBPD_ARRAYSIZE DL_libpd_arraysize;
PFN_LIBPD_READ_ARRAY DL_libpd_read_array;
PFN_LIBPD_WRITE_ARRAY DL_libpd_write_array;
PFN_LIBPD_BANG DL_libpd_bang;
PFN_LIBPD_FLOAT DL_libpd_float;
PFN_LIBPD_SYMBOL DL_libpd_symbol;
PFN_LIBPD_SET_FLOAT DL_libpd_set_float;
PFN_LIBPD_SET_SYMBOL DL_libpd_set_symbol;
PFN_LIBPD_LIST DL_libpd_list;
PFN_LIBPD_MESSAGE DL_libpd_message;
PFN_LIBPD_START_MESSAGE DL_libpd_start_message;
PFN_LIBPD_ADD_FLOAT DL_libpd_add_float;
PFN_LIBPD_ADD_SYMBOL DL_libpd_add_symbol;
PFN_LIBPD_FINISH_LIST DL_libpd_finish_list;
PFN_LIBPD_FINISH_MESSAGE DL_libpd_finish_message;
PFN_LIBPD_EXISTS DL_libpd_exists;
PFN_LIBPD_BIND DL_libpd_bind;
PFN_LIBPD_UNBIND DL_libpd_unbind;
PFN_LIBPD_NOTEON DL_libpd_noteon;
PFN_LIBPD_CONTROLCHANGE DL_libpd_controlchange;
PFN_LIBPD_PROGRAMCHANGE DL_libpd_programchange;
PFN_LIBPD_PITCHBEND DL_libpd_pitchbend;
PFN_LIBPD_AFTERTOUCH DL_libpd_aftertouch;
PFN_LIBPD_POLYAFTERTOUCH DL_libpd_polyaftertouch;
PFN_LIBPD_MIDIBYTE DL_libpd_midibyte;
PFN_LIBPD_SYSEX DL_libpd_sysex;
PFN_LIBPD_SYSREALTIME DL_libpd_sysrealtime;

t_libpd_printhook* DL_libpd_printhook;
t_libpd_banghook* DL_libpd_banghook;
t_libpd_floathook* DL_libpd_floathook;
t_libpd_symbolhook* DL_libpd_symbolhook;
t_libpd_listhook* DL_libpd_listhook;
t_libpd_messagehook* DL_libpd_messagehook;

t_libpd_noteonhook* DL_libpd_noteonhook;
t_libpd_controlchangehook* DL_libpd_controlchangehook;
t_libpd_programchangehook* DL_libpd_programchangehook;
t_libpd_pitchbendhook* DL_libpd_pitchbendhook;
t_libpd_aftertouchhook* DL_libpd_aftertouchhook;
t_libpd_polyaftertouchhook* DL_libpd_polyaftertouchhook;
t_libpd_midibytehook* DL_libpd_midibytehook;

#ifdef WIN32
#define LOAD_SYMBOL(PTR, PFN, NAME) PTR = (PFN) GetProcAddress(pdlib, NAME); \
										  if (PTR == NULL) ++ret;
#else
#define LOAD_SYMBOL(PTR, PFN, NAME) PTR = (PFN) dlsym(pdlib, NAME); \
										  if (PTR == 0) ++ret;
#endif

int libpd_dll_load(const char* dllname, void** handle)
{
    int ret = 0;

#ifdef WIN32
    HINSTANCE pdlib;
    wchar_t dllname_w[256];
    mbstowcs(dllname_w, dllname, 255);
    pdlib = LoadLibrary(dllname_w);
#else
    void* pdlib = dlopen(dllname, RTLD_LAZY);
#endif

    if (!pdlib)
        return -1;

    LOAD_SYMBOL(DL_libpd_init, PFN_LIBPD_INIT, "libpd_init");
    LOAD_SYMBOL(DL_libpd_clear_search_path, PFN_LIBPD_CLEAR_SEARCH_PATH, "libpd_clear_search_path");
    LOAD_SYMBOL(DL_libpd_add_to_search_path, PFN_LIBPD_ADD_TO_SEARCH_PATH, "libpd_add_to_search_path");
    LOAD_SYMBOL(DL_libpd_openfile, PFN_LIBPD_OPENFILE, "libpd_openfile");
    LOAD_SYMBOL(DL_libpd_closefile, PFN_LIBPD_CLOSEFILE, "libpd_closefile");
    LOAD_SYMBOL(DL_libpd_getdollarzero, PFN_LIBPD_GETDOLLARZERO, "libpd_getdollarzero");
    LOAD_SYMBOL(DL_libpd_blocksize, PFN_LIBPD_BLOCKSIZE, "libpd_blocksize");
    LOAD_SYMBOL(DL_libpd_init_audio, PFN_LIBPD_INIT_AUDIO, "libpd_init_audio");
    LOAD_SYMBOL(DL_libpd_process_raw, PFN_LIBPD_PROCESS_RAW, "libpd_process_raw");
    LOAD_SYMBOL(DL_libpd_process_short, PFN_LIBPD_PROCESS_SHORT, "libpd_process_short");
    LOAD_SYMBOL(DL_libpd_process_float, PFN_LIBPD_PROCESS_FLOAT, "libpd_process_float");
    LOAD_SYMBOL(DL_libpd_process_double, PFN_LIBPD_PROCESS_DOUBLE, "libpd_process_double");
    LOAD_SYMBOL(DL_libpd_arraysize, PFN_LIBPD_ARRAYSIZE, "libpd_arraysize");
    LOAD_SYMBOL(DL_libpd_read_array, PFN_LIBPD_READ_ARRAY, "libpd_read_array");
    LOAD_SYMBOL(DL_libpd_write_array, PFN_LIBPD_WRITE_ARRAY, "libpd_write_array");
    LOAD_SYMBOL(DL_libpd_bang, PFN_LIBPD_BANG, "libpd_bang");
    LOAD_SYMBOL(DL_libpd_float, PFN_LIBPD_FLOAT, "libpd_float");
    LOAD_SYMBOL(DL_libpd_symbol, PFN_LIBPD_SYMBOL, "libpd_symbol");
    LOAD_SYMBOL(DL_libpd_set_float, PFN_LIBPD_SET_FLOAT, "libpd_set_float");
    LOAD_SYMBOL(DL_libpd_set_symbol, PFN_LIBPD_SET_SYMBOL, "libpd_set_symbol");
    LOAD_SYMBOL(DL_libpd_list, PFN_LIBPD_LIST, "libpd_list");
    LOAD_SYMBOL(DL_libpd_message, PFN_LIBPD_MESSAGE, "libpd_message");
    LOAD_SYMBOL(DL_libpd_start_message, PFN_LIBPD_START_MESSAGE, "libpd_start_message");
    LOAD_SYMBOL(DL_libpd_add_float, PFN_LIBPD_ADD_FLOAT, "libpd_add_float");
    LOAD_SYMBOL(DL_libpd_add_symbol, PFN_LIBPD_ADD_SYMBOL, "libpd_add_symbol");
    LOAD_SYMBOL(DL_libpd_finish_list, PFN_LIBPD_FINISH_LIST, "libpd_finish_list");
    LOAD_SYMBOL(DL_libpd_finish_message, PFN_LIBPD_FINISH_MESSAGE, "libpd_finish_message");
    LOAD_SYMBOL(DL_libpd_exists, PFN_LIBPD_EXISTS, "libpd_exists");
    LOAD_SYMBOL(DL_libpd_bind, PFN_LIBPD_BIND, "libpd_bind");
    LOAD_SYMBOL(DL_libpd_unbind, PFN_LIBPD_UNBIND, "libpd_unbind");
    LOAD_SYMBOL(DL_libpd_noteon, PFN_LIBPD_NOTEON, "libpd_noteon");
    LOAD_SYMBOL(DL_libpd_controlchange, PFN_LIBPD_CONTROLCHANGE, "libpd_controlchange");
    LOAD_SYMBOL(DL_libpd_programchange, PFN_LIBPD_PROGRAMCHANGE, "libpd_programchange");
    LOAD_SYMBOL(DL_libpd_pitchbend, PFN_LIBPD_PITCHBEND, "libpd_pitchbend");
    LOAD_SYMBOL(DL_libpd_aftertouch, PFN_LIBPD_AFTERTOUCH, "libpd_aftertouch");
    LOAD_SYMBOL(DL_libpd_polyaftertouch, PFN_LIBPD_POLYAFTERTOUCH, "libpd_polyaftertouch");
    LOAD_SYMBOL(DL_libpd_midibyte, PFN_LIBPD_MIDIBYTE, "libpd_midibyte");
    LOAD_SYMBOL(DL_libpd_sysex, PFN_LIBPD_SYSEX, "libpd_sysex");
    LOAD_SYMBOL(DL_libpd_sysrealtime, PFN_LIBPD_SYSREALTIME, "libpd_sysrealtime");

    LOAD_SYMBOL(DL_libpd_printhook, t_libpd_printhook*, "libpd_printhook");
    LOAD_SYMBOL(DL_libpd_banghook, t_libpd_banghook*, "libpd_banghook");
    LOAD_SYMBOL(DL_libpd_floathook, t_libpd_floathook*, "libpd_floathook");
    LOAD_SYMBOL(DL_libpd_symbolhook, t_libpd_symbolhook*, "libpd_symbolhook");
    LOAD_SYMBOL(DL_libpd_listhook, t_libpd_listhook*, "libpd_listhook");
    LOAD_SYMBOL(DL_libpd_messagehook, t_libpd_messagehook*, "libpd_messagehook");

    LOAD_SYMBOL(DL_libpd_noteonhook, t_libpd_noteonhook*, "libpd_noteonhook");
    LOAD_SYMBOL(DL_libpd_controlchangehook, t_libpd_controlchangehook*, "libpd_controlchangehook");
    LOAD_SYMBOL(DL_libpd_programchangehook, t_libpd_programchangehook*, "libpd_programchangehook");
    LOAD_SYMBOL(DL_libpd_pitchbendhook, t_libpd_pitchbendhook*, "libpd_pitchbendhook");
    LOAD_SYMBOL(DL_libpd_aftertouchhook, t_libpd_aftertouchhook*, "libpd_aftertouchhook");
    LOAD_SYMBOL(DL_libpd_polyaftertouchhook, t_libpd_polyaftertouchhook*, "libpd_polyaftertouchhook");
    LOAD_SYMBOL(DL_libpd_midibytehook, t_libpd_midibytehook*, "libpd_midibytehook");

    if (ret == 0) {
        *handle = pdlib;
    }

    return ret;
}

void libpd_dll_close(void* handle)
{
    if (handle) {
#ifdef WIN32
        FreeLibrary((HINSTANCE) handle);
#else
        dlclose(handle);
#endif
    }
}

