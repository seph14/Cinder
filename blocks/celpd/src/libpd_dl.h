#ifndef __LIBPD_DL_H__
#define __LIBPD_DL_H__

#ifdef __cplusplus
extern "C"
{
#endif

struct _atom;
typedef struct _atom t_atom;

//  Returns 0 if loaded with no errors, -1 if init error, or x where x is the 
//  number of unresolved symbols.  Will only set a DLL handle if returned 0.
int  libpd_dll_load(const char* dllname, void** handle);
void libpd_dll_close(void* handle);

typedef void (*PFN_LIBPD_INIT)(void);
extern PFN_LIBPD_INIT DL_libpd_init;
#define libpd_init DL_libpd_init

typedef void (*PFN_LIBPD_CLEAR_SEARCH_PATH)(void);
extern PFN_LIBPD_CLEAR_SEARCH_PATH DL_libpd_clear_search_path;
#define libpd_clear_search_path DL_libpd_clear_search_path

typedef void (*PFN_LIBPD_ADD_TO_SEARCH_PATH)(const char *sym);
extern PFN_LIBPD_ADD_TO_SEARCH_PATH DL_libpd_add_to_search_path;
#define libpd_add_to_search_path DL_libpd_add_to_search_path

typedef void* (*PFN_LIBPD_OPENFILE)(const char *basename, const char *dirname);
extern PFN_LIBPD_OPENFILE DL_libpd_openfile;
#define libpd_openfile DL_libpd_openfile

typedef void (*PFN_LIBPD_CLOSEFILE)(void *p);
extern PFN_LIBPD_CLOSEFILE DL_libpd_closefile;
#define libpd_closefile DL_libpd_closefile

typedef int (*PFN_LIBPD_GETDOLLARZERO)(void *p);
extern PFN_LIBPD_GETDOLLARZERO DL_libpd_getdollarzero;
#define libpd_getdollarzero DL_libpd_getdollarzero

typedef int (*PFN_LIBPD_BLOCKSIZE)(void);
extern PFN_LIBPD_BLOCKSIZE DL_libpd_blocksize;
#define libpd_blocksize DL_libpd_blocksize

typedef int (*PFN_LIBPD_INIT_AUDIO)(int inChans, int outChans, int sampleRate);
extern PFN_LIBPD_INIT_AUDIO DL_libpd_init_audio;
#define libpd_init_audio DL_libpd_init_audio

typedef int (*PFN_LIBPD_PROCESS_RAW)(float *inBuffer, float *outBuffer);
extern PFN_LIBPD_PROCESS_RAW DL_libpd_process_raw;
#define libpd_process_raw DL_libpd_process_raw

typedef int (*PFN_LIBPD_PROCESS_SHORT)(int ticks, short *inBuffer, short *outBuffer);
extern PFN_LIBPD_PROCESS_SHORT DL_libpd_process_short;
#define libpd_process_short DL_libpd_process_short

typedef int (*PFN_LIBPD_PROCESS_FLOAT)(int ticks, float *inBuffer, float *outBuffer);
extern PFN_LIBPD_PROCESS_FLOAT DL_libpd_process_float;
#define libpd_process_float DL_libpd_process_float

typedef int (*PFN_LIBPD_PROCESS_DOUBLE)(int ticks, double *inBuffer, double *outBuffer);
extern PFN_LIBPD_PROCESS_DOUBLE DL_libpd_process_double;
#define libpd_process_double DL_libpd_process_double

typedef int (*PFN_LIBPD_ARRAYSIZE)(const char *name);
extern PFN_LIBPD_ARRAYSIZE DL_libpd_arraysize;
#define libpd_arraysize DL_libpd_arraysize

typedef int (*PFN_LIBPD_READ_ARRAY)(float *dest, const char *src, int offset, int n);
extern PFN_LIBPD_READ_ARRAY DL_libpd_read_array;
#define libpd_read_array DL_libpd_read_array

typedef int (*PFN_LIBPD_WRITE_ARRAY)(const char *dest, int offset, float *src, int n);
extern PFN_LIBPD_WRITE_ARRAY DL_libpd_write_array;
#define libpd_write_array DL_libpd_write_array

typedef int (*PFN_LIBPD_BANG)(const char *recv);
extern PFN_LIBPD_BANG DL_libpd_bang;
#define libpd_bang DL_libpd_bang

typedef int (*PFN_LIBPD_FLOAT)(const char *recv, float x);
extern PFN_LIBPD_FLOAT DL_libpd_float;
#define libpd_float DL_libpd_float

typedef int (*PFN_LIBPD_SYMBOL)(const char *recv, const char *sym);
extern PFN_LIBPD_SYMBOL DL_libpd_symbol;
#define libpd_symbol DL_libpd_symbol

typedef void (*PFN_LIBPD_SET_FLOAT)(t_atom *v, float x);
extern PFN_LIBPD_SET_FLOAT DL_libpd_set_float;
#define libpd_set_float DL_libpd_set_float

typedef void (*PFN_LIBPD_SET_SYMBOL)(t_atom *v, const char *sym);
extern PFN_LIBPD_SET_SYMBOL DL_libpd_set_symbol;
#define libpd_set_symbol DL_libpd_set_symbol

typedef int (*PFN_LIBPD_LIST)(const char *recv, int argc, t_atom *argv);
extern PFN_LIBPD_LIST DL_libpd_list;
#define libpd_list DL_libpd_list

typedef int (*PFN_LIBPD_MESSAGE)(const char *recv, const char *msg, int argc, t_atom *argv);
extern PFN_LIBPD_MESSAGE DL_libpd_message;
#define libpd_message DL_libpd_message

typedef int (*PFN_LIBPD_START_MESSAGE)(int max_length);
extern PFN_LIBPD_START_MESSAGE DL_libpd_start_message;
#define libpd_start_message DL_libpd_start_message

typedef void (*PFN_LIBPD_ADD_FLOAT)(float x);
extern PFN_LIBPD_ADD_FLOAT DL_libpd_add_float;
#define libpd_add_float DL_libpd_add_float

typedef void (*PFN_LIBPD_ADD_SYMBOL)(const char *sym);
extern PFN_LIBPD_ADD_SYMBOL DL_libpd_add_symbol;
#define libpd_add_symbol DL_libpd_add_symbol

typedef int (*PFN_LIBPD_FINISH_LIST)(const char *recv);
extern PFN_LIBPD_FINISH_LIST DL_libpd_finish_list;
#define libpd_finish_list DL_libpd_finish_list

typedef int (*PFN_LIBPD_FINISH_MESSAGE)(const char *recv, const char *msg);
extern PFN_LIBPD_FINISH_MESSAGE DL_libpd_finish_message;
#define libpd_finish_message DL_libpd_finish_message

typedef int (*PFN_LIBPD_EXISTS)(const char *sym);
extern PFN_LIBPD_EXISTS DL_libpd_exists;
#define libpd_exists DL_libpd_exists

typedef void* (*PFN_LIBPD_BIND)(const char *sym);
extern PFN_LIBPD_BIND DL_libpd_bind;
#define libpd_bind DL_libpd_bind

typedef void (*PFN_LIBPD_UNBIND)(void *p);
extern PFN_LIBPD_UNBIND DL_libpd_unbind;
#define libpd_unbind DL_libpd_unbind

typedef int (*PFN_LIBPD_NOTEON)(int channel, int pitch, int velocity);
extern PFN_LIBPD_NOTEON DL_libpd_noteon;
#define libpd_noteon DL_libpd_noteon

typedef int (*PFN_LIBPD_CONTROLCHANGE)(int channel, int controller, int value);
extern PFN_LIBPD_CONTROLCHANGE DL_libpd_controlchange;
#define libpd_controlchange DL_libpd_controlchange

typedef int (*PFN_LIBPD_PROGRAMCHANGE)(int channel, int value);
extern PFN_LIBPD_PROGRAMCHANGE DL_libpd_programchange;
#define libpd_programchange DL_libpd_programchange

typedef int (*PFN_LIBPD_PITCHBEND)(int channel, int value);
extern PFN_LIBPD_PITCHBEND DL_libpd_pitchbend;
#define libpd_pitchbend DL_libpd_pitchbend

typedef int (*PFN_LIBPD_AFTERTOUCH)(int channel, int value);
extern PFN_LIBPD_AFTERTOUCH DL_libpd_aftertouch;
#define libpd_aftertouch DL_libpd_aftertouch

typedef int (*PFN_LIBPD_POLYAFTERTOUCH)(int channel, int pitch, int value);
extern PFN_LIBPD_POLYAFTERTOUCH DL_libpd_polyaftertouch;
#define libpd_polyaftertouch DL_libpd_polyaftertouch

typedef int (*PFN_LIBPD_MIDIBYTE)(int port, int byte);
extern PFN_LIBPD_MIDIBYTE DL_libpd_midibyte;
#define libpd_midibyte DL_libpd_midibyte

typedef int (*PFN_LIBPD_SYSEX)(int port, int byte);
extern PFN_LIBPD_SYSEX DL_libpd_sysex;
#define libpd_sysex DL_libpd_sysex

typedef int (*PFN_LIBPD_SYSREALTIME)(int port, int byte);
extern PFN_LIBPD_SYSREALTIME DL_libpd_sysrealtime;
#define libpd_sysrealtime DL_libpd_sysrealtime

typedef void (*t_libpd_printhook)(const char *recv);
typedef void (*t_libpd_banghook)(const char *recv);
typedef void (*t_libpd_floathook)(const char *recv, float x);
typedef void (*t_libpd_symbolhook)(const char *recv, const char *sym);
typedef void (*t_libpd_listhook)(const char *recv, int argc, t_atom *argv);
typedef void (*t_libpd_messagehook)(const char *recv, const char *msg, int argc, t_atom *argv);

//  XXX check indirection
extern t_libpd_printhook* DL_libpd_printhook;
#define libpd_printhook (*DL_libpd_printhook)
extern t_libpd_banghook* DL_libpd_banghook;
#define libpd_banghook (*DL_libpd_banghook)
extern t_libpd_floathook* DL_libpd_floathook;
#define libpd_floathook (*DL_libpd_floathook)
extern t_libpd_symbolhook* DL_libpd_symbolhook;
#define libpd_symbolhook (*DL_libpd_symbolhook)
extern t_libpd_listhook* DL_libpd_listhook;
#define libpd_listhook (*DL_libpd_listhook)
extern t_libpd_messagehook* DL_libpd_messagehook;
#define libpd_messagehook (*DL_libpd_messagehook)

typedef void (*t_libpd_noteonhook)(int channel, int pitch, int velocity);
typedef void (*t_libpd_controlchangehook)(int channel, int controller, int value);
typedef void (*t_libpd_programchangehook)(int channel, int value);
typedef void (*t_libpd_pitchbendhook)(int channel, int value);
typedef void (*t_libpd_aftertouchhook)(int channel, int value);
typedef void (*t_libpd_polyaftertouchhook)(int channel, int pitch, int value);
typedef void (*t_libpd_midibytehook)(int port, int byte);

extern t_libpd_noteonhook* DL_libpd_noteonhook;
#define libpd_noteonhook (*DL_libpd_noteonhook)
extern t_libpd_controlchangehook* DL_libpd_controlchangehook;
#define libpd_controlchangehook (*DL_libpd_controlchangehook)
extern t_libpd_programchangehook* DL_libpd_programchangehook;
#define libpd_programchangehook (*DL_libpd_programchangehook)
extern t_libpd_pitchbendhook* DL_libpd_pitchbendhook;
#define libpd_pitchbendhook (*DL_libpd_pitchbendhook)
extern t_libpd_aftertouchhook* DL_libpd_aftertouchhook;
#define libpd_aftertouchhook (*DL_libpd_aftertouchhook)
extern t_libpd_polyaftertouchhook* DL_libpd_polyaftertouchhook;
#define libpd_polyaftertouchhook (*DL_libpd_polyaftertouchhook)
extern t_libpd_midibytehook* DL_libpd_midibytehook;
#define libpd_midibytehook (*DL_libpd_midibytehook)

#ifdef __cplusplus
}
#endif

#endif

