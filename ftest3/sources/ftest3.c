/*****************************************************************************
 *                              File ftest3.c
 *
 *              Simple I/O test bechmark program with Posix threads
 *
 *      Copyright (c) Dmitry V. Stefankov, 2002-2019. All rights reserved.
 *
 *****************************************************************************/
/*
 *   $Source: /usr/local/src/projects/tests/ftest3/RCS/ftest3.c,v $
 *  $RCSfile: ftest3.c,v $
 *   $Author: dstef $
 *     $Date: 2019-08-08 10:07:39+03 $
 * $Revision: 1.18 $
 *   $Locker: root $
 *
 *      $Log: ftest3.c,v $
 *      Revision 1.18  2019-08-08 10:07:39+03  dstef
 *      Updated copyright dates
 *
 *      Revision 1.17  2016-06-26 21:37:03+03  dstef
 *      Added static type for all vars and funcs
 *
 *      Revision 1.16  2016-06-26 21:32:31+03  dstef
 *      Minor updates
 *
 *      Revision 1.15  2016-06-03 21:33:49+03  dstef
 *      Minor fix
 *
 *      Revision 1.14  2016-06-03 21:31:54+03  dstef
 *      Remove duplicated line
 *
 *      Revision 1.13  2016-05-24 21:19:37+03  dstef
 *      Added more checks for strncat,snprintf,strncpy
 *
 *      Revision 1.12  2016-05-23 19:46:12+03  dstef
 *      Updated copyright years
 *
 *      Revision 1.11  2016-05-23 19:45:20+03  dstef
 *      Changed version
 *
 *      Revision 1.10  2016-05-23 19:44:45+03  dstef
 *      Updated some info
 *
 *      Revision 1.9  2016-05-23 19:42:39+03  dstef
 *      Added checks for snprintf, strncat, strncpy
 *      Added standard devices flushed before output
 *
 *      Revision 1.8  2013-03-04 18:07:09+04  dstef
 *      Added own RNG to fix problem (speed on debian linux)
 *
 *      Revision 1.7  2013-01-28 23:09:22+04  dstef
 *      Added auto memory testing for linux
 *
 *      Revision 1.6  2013-01-28 21:19:42+04  dstef
 *      Added auto-memory-testing for Windows platform
 *
 *      Revision 1.5  2013-01-28 19:38:34+04  dstef
 *      Added support for auto memory testing
 *      Minor fixes
 *
 *      Revision 1.4  2013-01-26 22:34:50+04  dstef
 *      Minor fixes for Windows platform
 *
 *      Revision 1.3  2013-01-26 22:20:21+04  dstef
 *      Fixed compile problem for Windows platform
 *
 *      Revision 1.2  2013-01-26 19:57:41+04  dstef
 *      Switch to 64-bit model
 *      Use file stream operations instead handle
 *      Minor changes
 *
 *      Revision 1.1  2013-01-26 18:47:55+04  dstef
 *      Initial revision
 *
 ****************************************************************************/



#if _WIN32
#include <windows.h>
#endif
#ifndef _MSVC
#include  <unistd.h>
#endif
#include  <limits.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/stat.h>
#include  <fcntl.h>
#include  <stdio.h>
#include  <string.h>
#include  <errno.h>
#include  <signal.h>
#include  <time.h>
#ifndef _MSVC
#include  <sys/time.h>
#endif
#ifndef _WIN32
#include  <sys/resource.h>
#include  <pthread.h>
#else
#include  <direct.h>
#include  <io.h>
#endif

#if _WIN32
#include "getopt.h"
#endif

#if (__FreeBSD__)
#include <sys/sysctl.h>
#endif

#if defined(__linux__)
#include <sys/sysinfo.h>
#endif

/* Test directives */
#define  __FORCE_COMPARE_ERROR__  0

/* Definitions (OS-dependent) */

#if _WIN32

#ifndef getcwd
#define  getcwd _getcwd
#endif

#ifndef _commit
int __cdecl _commit(int);
#endif

#ifndef snprintf
#define snprintf _snprintf
#endif

#if     _MSC_VER > 1000
#define  open      _open
#define  read      _read
#define  write     _write
#define  close     _close
#define  fileno    _fileno
#define  unlink    _unlink
#define  lseek     _lseek
#define  chdir     _chdir
#endif

/*
 * Number of micro-seconds between the beginning of the Windows epoch
 * (Jan. 1, 1601) and the Unix epoch (Jan. 1, 1970).
 */
#if defined(_MSC_VER) || defined(__WATCOMC__)
#define EPOCH_FILETIME 11644473600000000Ui64
#else
#define EPOCH_FILETIME 11644473600000000ULL
#endif

#endif


#ifndef _WIN32

#define SIGNAL(s,handler)	{\
	sa.sa_handler = handler; \
	if (sigaction(s,&sa,NULL) < 0) { \
	  fprintf( stderr, "Couldn't establish signal handler (%d): %m", s); \
	} \
   }

#endif


/* Definitions */
#define  MAX_PRIMES_NUM     1000000
#define  MIN_REQ_SSIZE      (128*1024)
#define  MAX_NUM_THREADS    500
#define  MAX_NUM_ELEMS      100
#define  DEF_NUM_THREADS    1
#define  DEF_NUM_ELEMS      1
#define  DEF_BUF_SIZE       4
#define  DEF_BUF_PART_SIZE  1
#define  MAX_BUF_PART_SIZE  512
#define  DEF_FILE_SIZE      16
#define  FILENAME_SIZE	    255
#define  DIRNAME_SIZE	    FILENAME_SIZE
#define  AUTO_BUF_SIZE      (1*1000)

#define  GET_TIME_RETRY_COUNT  7

#define  DEF_PASS_COUNT 1

#define  ALARM_SECS	3600
#define  MS_PER_SEC     1000

#define  ONE_KILOBYTE  1024
#define  ONE_MEGABYTE  (ONE_KILOBYTE*1024)
#define  ONE_GIGABYTE  (ONE_MEGABYTE*1024)

#ifndef _WIN32
typedef  long long int  __int64;
#endif


/* Data structures */

struct buf_elem {
   __int64  msize;
   __int64  fsize;
   __int64  rb_size;
   void * r_pbuf;
   void * w_pbuf;
   int  retry_attempts;
   char filename[FILENAME_SIZE];
   __int64 chksum;
   FILE * fp;
};

struct  thread_param
{
   int  thread_id;
   __int64  max_file_size;
   int  passes_count;
   int  loop_forever;
   int  memory_test_only;
   int  fill_rand;
   int  rw_mult_bufs;
   int  dsync_flag;
   int  fsync_flag;   
   int  flags;
   int  fmode;
   int  time_profile;
   int  retry_attempts;
   int  increment_filenames;
};

struct  stat_info
{
  __int64  read_bytes;
  __int64  write_bytes;
  unsigned long int   compare_errors;
  unsigned long int   other_errors;
  unsigned long int   passes_count;
  double  read_file_sliding_average_time;
  double  write_file_sliding_average_time;
  double  read_file_sliding_average_speed;
  double  write_file_sliding_average_speed;
};


/*------------------------------- GNU C library -----------------------------*/
#if _WIN32
extern int       opterr;
extern int       optind;
extern char     *optarg;
#endif


/* Local data */

extern  int        optind;
extern  int        opterr;
extern  char     * optarg;

static int  max_elem_num  = DEF_NUM_ELEMS;
static __int64  max_buf_size  = DEF_BUF_SIZE;
static __int64  max_file_size = DEF_FILE_SIZE;
static int  max_threads_num = DEF_NUM_THREADS;
static __int64  auto_buf_size  = AUTO_BUF_SIZE;

#if _WIN32
int                 flags = O_RDWR|O_CREAT|O_TRUNC|O_BINARY;
#if _MSVC
static      int    fmode = _S_IREAD | _S_IWRITE;
#else
static      mode_t fmode = _S_IRUSR|_S_IWUSR;
#endif
static const char    dir_sep_str[] = "\\";
static char          work_dir[DIRNAME_SIZE] = "c:\\temp";
static HANDLE        g_hEventObj = NULL;
static HANDLE        g_hWaitingThread = NULL;
static DWORD         g_dwThreadId = 0;
static DWORD         g_dwThreadArg = 0;
static DWORD         threads_array[MAX_NUM_THREADS];
static HANDLE        g_hEventThreadsObj[MAX_NUM_THREADS];
static HANDLE        g_ThreadHandles[MAX_NUM_THREADS];
static DWORD         dwDefThreadWaitTime = (ALARM_SECS * 2) * MS_PER_SEC;
#else
static int           flags = O_RDWR|O_CREAT|O_TRUNC;
static mode_t        fmode = S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH;
static const char    dir_sep_str[] = "/";
static char  work_dir[DIRNAME_SIZE] = "/tmp";
static size_t        default_stack_size = 0;
static size_t        new_stack_size = 0;
static pthread_attr_t      stack_size_custom_attr[MAX_NUM_THREADS];
static pthread_t     threads_array[MAX_NUM_THREADS];
#endif

static unsigned char * fill_pattern = NULL;
static char         * out_dir = NULL;
static char         * LogFile = NULL;
static int           delete_flag = 1;
static int           verbose = 0;
static int           quiet = 0;
static int           check_work_dir_presence = 1;
static int           shadow_buffers_copy = 1;
static int           use_alarm = 0;
static unsigned int  io_alarm_secs = ALARM_SECS;
static int           user_break_enable = 1;
static int           user_break_exit = 0;
static int           loop_forever = 0;
static int           rw_mult_bufs = 0;
static int           dsync_flag = 0;
static int           fsync_flag = 0;
static int           fill_rand = 0;
static int           stop_on_error = 0;
static int           time_profiling = 0;
static int           collect_summary_statistics = 1;
static int           enable_random_sleep_in_thread = 0;
static unsigned long int   g_passes_count = DEF_PASS_COUNT;
static unsigned long int   us_wait_to_run_thread = 0;
static int           emul_old_ftest = 0;
static int           memory_test_only = 0;
static int           retry_attempts = 1;
static int           increment_filenames = 0;
static int           auto_memory_test = 0;
static int           use_unix_rng = 0;
static int           use_mswin_rng = 0;


static unsigned long int  g_total_exec_time = 0;

static volatile struct stat_info  summary_stat;

static const char prog_ver[] = "1.1.0";
static const char rcs_id[] = "$Id: ftest3.c,v 1.18 2019-08-08 10:07:39+03 dstef Exp root $";

static struct buf_elem  shadow_buffers[MAX_NUM_ELEMS*MAX_NUM_THREADS];

static unsigned long int primes_found = 0;
static unsigned long int max_int_num = 0;
static unsigned long int primes_table[MAX_PRIMES_NUM];

/*----------------------- Functions ---------------------------------------*/


static unsigned long int next = 1;

static int unix_rand(void) // RAND_MAX assumed to be 32767
{
    next = next * 1103515245 + 12345;
    return (unsigned int)(next/65536) % 32768;
}

static void unix_srand(unsigned int seed)
{
    next = seed;
}


static void mswin_srand ( unsigned int seed )
{
    next = (unsigned long)seed;
    //next = seed;
}

static int mswin_rand ( void )
{
    next = (next * 214013L + 2531011L);
    return( (next >> 16) & 0x7fff );
}



#if defined(_WIN32) || defined(_MINGW32)
static void sleep(unsigned long int seconds)
{
   Sleep( seconds * 1000 );
}
#endif

#if defined(_WIN32) && !defined(_MINGW32)
/*#if defined(_WIN32)*/
static int gettimeofday(struct timeval *tv, struct timezone *tz)
{
    FILETIME        ft;
    LARGE_INTEGER   li;
    __int64         t;

    if (tv == NULL) return(-1);
    if (tv)
    {
        memset( &ft, 0, sizeof(ft) );
        GetSystemTimeAsFileTime(&ft);
        li.LowPart  = ft.dwLowDateTime;
        li.HighPart = ft.dwHighDateTime;
        t  = li.QuadPart / 10;   /* In micro-second intervals */
        t -= EPOCH_FILETIME;     /* Offset to the Epoch time */
        tv->tv_sec  = (long)(t / 1000000);
        tv->tv_usec = (long)(t % 1000000);
    }
    (void) tz;
    return 0;
}
#endif

/*
 * Print standard message from signal handler
 */
static void print_message_signal( char * msg )
{
   if (msg == NULL) return;
#if _WIN32
   write( fileno(stdin), msg, (unsigned)strlen(msg) );
#else
   else write( STDIN_FILENO, msg, strlen(msg) );
#endif
}


/*
 * Print error message from signal handler
 */
static void print_error_message_signal( char * msg )
{
   if (msg == NULL) return;
#if _WIN32
   write( fileno(stderr), msg, (unsigned)strlen(msg) );
#else
   write( STDERR_FILENO, msg, strlen(msg) );
#endif
}


/*
 * Write message to logfile
 */
static int write_log( char * filename, char * buf )
{ 
   int           res = -1;
   FILE        * fp;
   time_t        curtime;
   struct tm   * cur_tm;
   char          time_buf[64];
   
   if ((filename == NULL) || (buf == NULL))
     return(res);
   
   res = -2;
   fp = fopen( filename, "at" );
   if (fp != NULL) {
     time(&curtime);
     cur_tm = localtime(&curtime);
     memset( time_buf, 0, sizeof(time_buf) );
     strftime( time_buf, sizeof(time_buf)-1, "%a %b %d %H:%M:%S %Y", cur_tm );  
     fprintf( fp, "%s:", time_buf ); 
     fprintf( fp, " %s", buf );
     fclose(fp);
     res = 0;
   }
   
   return( res );                
}

                   
/*
 * Init random numbers generator
 */
static void init_rnd_gen(int ofs)
{ 
  if (use_unix_rng) {
    unix_srand(time(NULL)+ofs );
    return;
  }

  if (use_mswin_rng) {
    mswin_srand((unsigned)time(NULL)+ofs );
    return;
  }

#if _WIN32
         srand( (unsigned)time(NULL)+ofs );
#else
         srandom( time(NULL)+ofs );
#endif
}


/*
 * Get next random number
 */
static unsigned char next_rnd_num(void)
{
  unsigned char rn;

  if (use_unix_rng) {
    rn = unix_rand() & 0xFF;
    return rn;
  }

  if (use_mswin_rng) {
    rn = mswin_rand() & 0xFF;
    return rn;
  }

#if _WIN32
  rn = rand() & 0xFF;
#else
  rn = random() & 0xFF;
#endif  
  return(rn);
}


/*
 * Get next random number
 */
static unsigned int next_rnd_num_word(void)
{
  unsigned int rn;

  if (use_unix_rng) {
    rn = unix_rand() & 0xFFFF;
    return rn;
  }

  if (use_mswin_rng) {
    rn = mswin_rand() & 0xFFFF;
    return rn;
  }

#if _WIN32
  rn = rand() & 0xFFFF;
#else
  rn = random() & 0xFFFF;
#endif  
  return(rn);
}


/*
 * Synchronize changes to file
 */
static void sync_file_changes(FILE * fp)
{
#if _WIN32
   _commit(fileno(fp));
#else
   fsync(fileno(fp));
#endif
}

    

/*
 *  Close all open files
 */
static void close_files( struct buf_elem * buffers )
{
  int    i;
  int    res;
  int    retry_count = 0;
  char  msg_buf[128];

  for(i=0; i<max_elem_num; i++) {  
    if ((buffers[i].w_pbuf != NULL) && (buffers[i].fp != NULL)) {  
      retry_count = buffers[i].retry_attempts;
     try_close: 
      res = fclose(buffers[i].fp);
      if (res == -1) {
        memset( msg_buf, 0, sizeof(msg_buf) );
        snprintf( msg_buf, sizeof(msg_buf)-1, 
                  "ERR [close]: %s for buffer %d\n", strerror(errno),i );
        print_error_message_signal( msg_buf );        
        if (--retry_count) {
          sleep(1);
          goto try_close;
        }
      }
      if (delete_flag) {
          res = unlink(buffers[i].filename);     
          if (res == -1) {
              memset( msg_buf, 0, sizeof(msg_buf) );
              snprintf( msg_buf, sizeof(msg_buf)-1, 
                        "ERR [unlink]: %s for buffer %d\n", strerror(errno),i );
              print_error_message_signal( msg_buf );        
          }
      }
      else {
          if (verbose) printf( "Remain temporary files. Remove them manually.\n" );
      }
    }
  }
}


/*
 *  Free all buffers
 */
static void free_buffers( struct buf_elem * buffers )
{
  int  i;
  
  for(i=0; i<max_elem_num; i++) {  
    if (buffers[i].w_pbuf != buffers[i].r_pbuf) {
      if (buffers[i].r_pbuf != NULL) free(buffers[i].r_pbuf);
    }  
    if (buffers[i].w_pbuf != NULL) free(buffers[i].w_pbuf);
  }
}


/*
 *  Close all open files (global)
 */
static void shadow_close_files( void )
{
  int    i;
  int    res;
  int    retry_count = 0;
  char  msg_buf[128];

  if (verbose) printf( "Shadow_close_files.\n" );

  for(i=0; i<(max_elem_num*max_threads_num); i++) {  
    if ((shadow_buffers[i].w_pbuf != NULL) && (shadow_buffers[i].fp != NULL)) {  
      retry_count = shadow_buffers[i].retry_attempts;
     try_close: 
      res = fclose(shadow_buffers[i].fp);
      if (res == -1) {
        memset( msg_buf, 0, sizeof(msg_buf) );
        snprintf( msg_buf, sizeof(msg_buf)-1, 
                  "ERR [shadow_close]: %s for buffer %d\n", strerror(errno),i );
        print_error_message_signal( msg_buf );        
        if (--retry_count) {
          sleep(1);
          goto try_close;
        }
      }
      if (delete_flag) {
          res = unlink(shadow_buffers[i].filename);     
          if (res == -1) {
              memset( msg_buf, 0, sizeof(msg_buf) );
              snprintf( msg_buf, sizeof(msg_buf)-1, 
                        "ERR [unlink]: %s for buffer %d\n", strerror(errno),i );
              print_error_message_signal( msg_buf );        
          }
      }
      else {
          if (verbose) printf( "Remain temporary files. Remove them manually.\n" );
      }
    }
  }
}

  
/*
 *  Free all buffers (global)
 */
static void shadow_free_buffers( void )
{
  int  i;

  if (verbose) printf( "Shadow_free_buffers.\n" );
  
  for(i=0; i<(max_elem_num*max_threads_num); i++) {  
    if (shadow_buffers[i].w_pbuf != shadow_buffers[i].r_pbuf) {
      if (shadow_buffers[i].r_pbuf != NULL)
        free(shadow_buffers[i].r_pbuf);
    }
    if (shadow_buffers[i].w_pbuf != NULL)
       free(shadow_buffers[i].w_pbuf);
  }
}

  
/*
 *  Terminate program
 */
static void term(int sig)
{
   char  msg_buf[128];
  
   memset( msg_buf, 0, sizeof(msg_buf) );
   snprintf( msg_buf, sizeof(msg_buf)-1, "\nsignal() %d\n", sig );
   print_message_signal( msg_buf );
   if (user_break_enable) {
      user_break_exit++;
      if (verbose) print_message_signal("User want to break!\n");
   }
   else {
#if 1
       shadow_close_files();
       shadow_free_buffers();   
#endif   
       exit(1);
   }
}


/*
 * Thread for i/o completion waiting
 */
#if _WIN32 
static DWORD WaitingThread( LPDWORD lpdwParam )
{
  DWORD  dwRes;

  g_hEventObj = CreateEvent(  NULL,         /* no security attributes */
                              FALSE,        /* manual-reset event */
                              FALSE,        /* initial state is signaled */
                              NULL          /* object name */
  ); 

  if (g_hEventObj == NULL) { 
    return(0);
  }

  dwRes = WaitForSingleObject( g_hEventObj, *lpdwParam );

  switch( dwRes ) {
    case WAIT_TIMEOUT:
      if (verbose) printf( "I/O timeout in wait_thread.\n" );
      CloseHandle(g_hEventObj);
      term( SIGTERM );
      break;
   case WAIT_OBJECT_0:
      break;
   case WAIT_ABANDONED:
      break;
    default:
      break;
  }

  CloseHandle(g_hEventObj);

  return(0);
}
#endif

/*
 * Set signal timer alarm
 */
#if _WIN32 
static unsigned int alarm(unsigned int seconds)
{
  BOOL   fRes;

  if (seconds == 0) {
    fRes = SetEvent(g_hEventObj);
    return(0);
  }

  g_dwThreadArg = (DWORD)io_alarm_secs * MS_PER_SEC;

  g_hWaitingThread = CreateThread( NULL,    // SD
                                   0,       // initial stack size
  (LPTHREAD_START_ROUTINE)WaitingThread,    // thread function
                           &g_dwThreadArg,  // thread argument
                                   0,       // creation option
                       &g_dwThreadId        // thread identifier
                                  );

  if (g_hWaitingThread == NULL) {
    if (verbose) printf( "ERR [alarm]: cannot run wait_thread.\n" );
    return(0);
  }

  return(0);
} 
#endif


/*
 *  Time execution control thread 
 */
static void * time_exec_task_thread(void * arg)
{
  unsigned long int wait_time;
  
  if (verbose) {
    printf( "time_exec_task_thread started.\n" );
  }
  
  if (g_total_exec_time == 0) goto done;

  wait_time = g_total_exec_time;
  if (verbose) {
    printf( "time_exec_task_thread wait time: %lu seconds\n", wait_time );
  }
  
  while(wait_time--) {
    sleep(1);
  }

  if (verbose) {
    printf( "time_exec_task_thread set terminate flag.\n" );
  }
  user_break_exit = 1;
  
done:

  if (verbose) {
    printf( "time_exec_task_thread terminated.\n" );
  }
  return(NULL);
}


/*
 *  Working thread 
 */
static void * working_task_thread(void * arg)
{
  struct thread_param  my_thread_param;  
  int                 i;
  size_t              j;
  int                 l;
  int                 use_increment_filenames = 0;
  unsigned long int   filename_num = 0;
  int                 forever_flag = 0;
  int                 retry_count = 0;
  int                 time_res;
  size_t              slen;
  int                 found_errors = 0;
  int                 my_sleep_secs;
  char                mult_rw_op;
  char                temp_space[32];
  char                num_space[32];
  char                log_buf[256];
  __int64             len;
  __int64             filesize;
  __int64             this_len;
  __int64             this_count;
  __int64             count;
  unsigned long int   passes_count;
  unsigned long int   pass_num = 0;
  __int64             chksum;
  unsigned char *     p;
  struct timeval      start_time;
  struct timeval      end_time;        
#if _WIN32
  DWORD               cur_pid;
  DWORD               this_ptid;
#else
  pid_t               cur_pid;
  pthread_t           this_ptid;
#endif
  double read_file_time = 0.0;
  double write_file_time = 0.0;
  double read_file_speed = 0.0;
  double write_file_speed = 0.0;
  struct timeval      timeout;
  struct stat_info    thread_stat;
  struct buf_elem  buffers[MAX_NUM_ELEMS];

  if (arg == NULL) {
        printf( "ERROR: cannot find thread parameters!\n" );
        return( NULL);
  }

  memset( &thread_stat, 0, sizeof(thread_stat) );
#if _WIN32
  cur_pid = GetCurrentProcessId();
  this_ptid = GetCurrentThreadId();
#else
  cur_pid = getpid();
  this_ptid = pthread_self();
#endif
  memset( &my_thread_param, 0, sizeof(my_thread_param) );
  memcpy( &my_thread_param, (struct thread_param *)arg, sizeof(struct thread_param) );
  if (verbose) {
       printf( "Thread %d: (process %u) started.\n", 
               my_thread_param.thread_id, (unsigned int)cur_pid );
       fflush(stdout);
  }
  passes_count = my_thread_param.passes_count;
  forever_flag = my_thread_param.loop_forever;
  use_increment_filenames = my_thread_param.increment_filenames;

forever:
/* Initialize */
  if (user_break_exit) goto free_all;
  if (LogFile != NULL) {
    memset( log_buf, 0, sizeof(log_buf) );
    snprintf( log_buf, sizeof(log_buf)-1, "Thread %d: Started pass %lu.\n", 
                                    my_thread_param.thread_id, pass_num+1 );
    write_log( LogFile, log_buf );
  }
  if (verbose) {
    printf( "Thread %d: Init structures. %u buffers, each size %u MBytes\n",
                      my_thread_param.thread_id, max_elem_num, max_buf_size );
    fflush(stdout);
  }
    
  memset( buffers, 0, sizeof(buffers) );    
  for(i=0; i<max_elem_num; i++) {
    buffers[i].msize = max_buf_size * ONE_MEGABYTE;
    buffers[i].rb_size = 0;
    buffers[i].fsize = (__int64)max_file_size * ONE_MEGABYTE;
    buffers[i].r_pbuf = NULL;
    buffers[i].w_pbuf = NULL;    
    buffers[i].retry_attempts = my_thread_param.retry_attempts;
    buffers[i].fp = NULL;
    buffers[i].chksum = -1;
    memset( buffers[i].filename,0,sizeof(buffers[i].filename) );
    if (shadow_buffers_copy) {
       memcpy( &shadow_buffers[my_thread_param.thread_id*max_elem_num+i],
               &buffers[i], sizeof(struct buf_elem) );
    }
  }
  if (enable_random_sleep_in_thread) {
     my_sleep_secs = next_rnd_num() & 0x0f;
     if (verbose) {
             printf( "Thread %d: sleep for %d seconds.\n", 
                      my_thread_param.thread_id, my_sleep_secs );
             fflush(stdout);
     }
     memset( &timeout, 0, sizeof(timeout) );
     timeout.tv_usec = 0;
     timeout.tv_sec = my_sleep_secs;
     select(0,NULL,NULL,NULL,&timeout);
  }

/* Get memory from system */
  if (user_break_exit) goto free_all;
  if (verbose) {
      printf( "Thread %d: Allocate memory buffers.\n", my_thread_param.thread_id );
      fflush(stdout);
  }
  
  for(i=0; i<max_elem_num; i++) {
     retry_count = my_thread_param.retry_attempts;
   try_malloc:
     //printf( "malloc: buffers[i].msize=%llu\n", buffers[i].msize);
     buffers[i].w_pbuf = malloc(buffers[i].msize);
     if (buffers[i].w_pbuf == NULL) {
       thread_stat.other_errors++;
       fprintf( stderr, "Thread %d: ERR [malloc]: %s for buffer %d (retry %d from %d)\n", 
                         my_thread_param.thread_id,strerror(errno),i,
			 my_thread_param.retry_attempts-retry_count+1, 
			 my_thread_param.retry_attempts );
       fflush(stderr);
       if (stop_on_error) {
           found_errors++;
           goto free_all;
       }
       if (--retry_count) {
         sleep(1);
         goto try_malloc;
       }
     }
     else {
       if (verbose) {
          if (my_thread_param.rw_mult_bufs) printf( "Thread %d: Fill write_buffer %d\n",
                                                    my_thread_param.thread_id,i );     
          else printf( "Thread %d: Fill read_write_buffer %d\n", 
                        my_thread_param.thread_id,i );     
          fflush(stdout);
       }
       if (shadow_buffers_copy)
          shadow_buffers[my_thread_param.thread_id*max_elem_num+i].w_pbuf = 
	                                                    buffers[i].w_pbuf;
       if (my_thread_param.fill_rand) {
         init_rnd_gen(i);
         len = buffers[i].msize;
         p = buffers[i].w_pbuf;
         while( len--) {
           *p++ = next_rnd_num();
         }
       }
       else {
         if (fill_pattern != NULL) {
             len = buffers[i].msize;
             p = buffers[i].w_pbuf;
             slen = strlen((const char *)fill_pattern);
             while( len ) {
                j = slen;
                if ((unsigned)slen > len) j = len;
                memcpy( p, fill_pattern, j );
                p += j; len -= j;
             }
         }
         else {
             memset( buffers[i].w_pbuf, i, buffers[i].msize );
         }
       }
     }
     if (!my_thread_param.rw_mult_bufs) {
         buffers[i].r_pbuf = buffers[i].w_pbuf;
         if (shadow_buffers_copy)
            shadow_buffers[my_thread_param.thread_id*max_elem_num+i].r_pbuf = 
	                                                      buffers[i].r_pbuf;
     }	 
  }

  if (my_thread_param.rw_mult_bufs) {
   for(i=0; i<max_elem_num; i++) {
     retry_count = my_thread_param.retry_attempts;
   try_malloc_rw:    
     buffers[i].r_pbuf = malloc(buffers[i].msize);
     if (buffers[i].r_pbuf == NULL) {
       thread_stat.other_errors++;
       fprintf( stderr, "Thread %d: ERR [malloc]: %s for buffer %d (retry %d from %d)\n", 
                         my_thread_param.thread_id,strerror(errno),i,
                         my_thread_param.retry_attempts-retry_count+1, 
			 my_thread_param.retry_attempts );
       fflush(stderr);
       if (stop_on_error) {
           found_errors++;
           goto free_all;
       }
       if (--retry_count) {
         sleep(1);
         goto try_malloc_rw;
       }
     }
     else {
       if (shadow_buffers_copy)
          shadow_buffers[my_thread_param.thread_id*max_elem_num+i].r_pbuf = 
	                                                    buffers[i].r_pbuf;
       if (verbose) {
                    printf( "Thread %d: Fill read_buffer %d\n",
                             my_thread_param.thread_id,i );     
                    fflush(stdout);
       }
       if (my_thread_param.fill_rand) {
         init_rnd_gen(i);
         len = buffers[i].msize;
         p = buffers[i].r_pbuf;
         while( len--) {
           *p++ = next_rnd_num();
         }
       }
       else
         if (fill_pattern != NULL) {
             len = buffers[i].msize;
             p = buffers[i].r_pbuf;
             slen = (unsigned)strlen((const char *)fill_pattern);
             while( len ) {
                j = slen;
                if ((unsigned)slen > len) j = len;
                memcpy( p, fill_pattern, j );
                p += j; len -= j;
             }
         }
         else {
           memset( buffers[i].r_pbuf, i, buffers[i].msize );
         }
     }
   }
  }

/* Check for memory-test-only option*/
 if (my_thread_param.memory_test_only) goto free_memory;
 
/* Create file interface */
  if (user_break_exit) goto free_all;
  if (verbose) {
    printf( "Thread %d: Create files. %u files, each size = %u Mbytes\n", 
                        my_thread_param.thread_id,max_elem_num, max_file_size );
    fflush(stdout);
  }

  for(i=0; i<max_elem_num; i++) { 
     if (buffers[i].w_pbuf != NULL) {
       memset( buffers[i].filename, 0, sizeof(buffers[i].filename) );
       if (out_dir != NULL) strncpy( buffers[i].filename, out_dir, sizeof(buffers[i].filename)-1 );       
       else                 strncpy( buffers[i].filename, work_dir, sizeof(buffers[i].filename)-1 );
       strncat( buffers[i].filename, dir_sep_str, sizeof(buffers[i].filename)-1-strlen(buffers[i].filename) );
       memset( num_space, 0, sizeof(num_space) );
       snprintf( num_space, sizeof(num_space)-1, "%d_%d_", (unsigned int)cur_pid, my_thread_param.thread_id );
       strncat( buffers[i].filename, num_space, sizeof(buffers[i].filename)-1-strlen(buffers[i].filename) );
       memset( temp_space, 0, sizeof(temp_space) );
       snprintf( temp_space, sizeof(temp_space)-1, "%d", i );
       strncat( buffers[i].filename, temp_space, sizeof(buffers[i].filename)-1-strlen(buffers[i].filename) );
       if (use_increment_filenames) {
         memset( temp_space, 0, sizeof(temp_space) );
         snprintf( temp_space, sizeof(temp_space)-1, "_%lu", filename_num );
         strncat( buffers[i].filename, temp_space, sizeof(buffers[i].filename)-1-strlen(buffers[i].filename) );
       }
       strncat( buffers[i].filename,".DUMMY", sizeof(buffers[i].filename)-1-strlen(buffers[i].filename) );
       if (shadow_buffers_copy) {     	      
          memcpy(shadow_buffers[my_thread_param.thread_id*max_elem_num+i].filename,
                 buffers[i].filename,
		 sizeof(buffers[i].filename) );
       }		 
       if (verbose) {
         printf( "Thread %d: filename = '%s'\n", 
                        my_thread_param.thread_id, buffers[i].filename );
         fflush(stdout);
       }

     retry_count = my_thread_param.retry_attempts;
   try_open:    
       buffers[i].fp = fopen( buffers[i].filename, "a+b" );
       if (buffers[i].fp == NULL) {
         thread_stat.other_errors++;
         fprintf( stderr, "Thread %d: ERR [open]: %s for file %s (retry %d from %d)\n",  
              my_thread_param.thread_id,strerror(errno),buffers[i].filename,
              my_thread_param.retry_attempts-retry_count+1, 
	      my_thread_param.retry_attempts );
         fflush(stderr);
         if (stop_on_error) {
             found_errors++;
             goto free_all;
         }
         if (--retry_count) {
           sleep(1);
           goto try_open;
         }
       }
       if (shadow_buffers_copy)      	      
          shadow_buffers[my_thread_param.thread_id*max_elem_num+i].fp = 
	                                                    buffers[i].fp;
     }
  } /*for*/
  
  /* Update counter */
  if (use_increment_filenames) filename_num++;


/* Write test */
  if (user_break_exit) goto free_all;
  if (verbose) {
              printf( "Thread %d: Write files from memory buffers.\n",
                       my_thread_param.thread_id );
              fflush(stdout);
  }

  for(i=0; i<max_elem_num; i++) {  
    if ((buffers[i].w_pbuf != NULL) && (buffers[i].fp != NULL)) {
      filesize = buffers[i].fsize;
      retry_count = my_thread_param.retry_attempts;
   try_lseek_write:    
      fflush(buffers[i].fp);
      if (fseek( buffers[i].fp, 0, SEEK_SET ) == -1) {
        thread_stat.other_errors++;
        fprintf( stderr, "Thread %d: ERR [seek/w]: %s for file %d (retry %d from %d)\n",
                          my_thread_param.thread_id,strerror(errno), i,
                          my_thread_param.retry_attempts-retry_count+1, 
			  my_thread_param.retry_attempts );
        fflush(stderr);
        if (stop_on_error) {
            found_errors++;
            goto free_all;
        }
        if (--retry_count) {
          sleep(1);
          goto try_lseek_write;
        }
        continue;
      }      
      chksum = 0;
      if (verbose) { 
          printf( "Thread %d: Write file %d: ",my_thread_param.thread_id,i );
          fflush(stdout);
      }
      else if (quiet == 0) printf( "w" );
      if (my_thread_param.time_profile) {
          memset( &start_time, 0, sizeof(start_time) );
          time_res = -1;
          for( l=0; l<GET_TIME_RETRY_COUNT; l++ ) {
              time_res = gettimeofday( &start_time, NULL );
              if (time_res == 0) break;
          }
          if (time_res != 0) {
            fprintf( stderr, "Thread %d: ERR [start_time not available on_write]: %s for file %d\n",
                             my_thread_param.thread_id,strerror(errno), i );
            fflush(stderr);
          }
      }
      if (1) {
       while( filesize ) {
        mult_rw_op = '*';
        if ( filesize < buffers[i].msize) {
            this_len = filesize;
            mult_rw_op = '.';
        }
        else {
            this_len = buffers[i].msize;
        }
        p = buffers[i].w_pbuf;
        len = this_len;
        while( len--) { 
          chksum += *p++;
        }
        if (quiet == 0) printf( "%c", mult_rw_op );
        if (use_alarm) alarm(io_alarm_secs);
        retry_count = my_thread_param.retry_attempts;
   try_write:
        this_count = this_len / ONE_MEGABYTE;
        count = fwrite( buffers[i].w_pbuf, ONE_MEGABYTE, this_count, buffers[i].fp );
        len = count * ONE_MEGABYTE;
	if (len == this_len) thread_stat.write_bytes += len;
        if (use_alarm) alarm(0);
        if (len != this_len) {
          thread_stat.other_errors++;
          fprintf( stderr, "\nThread %d: ERR [write]: %s for buffer %d (retry %d from %d)\n", 
                            my_thread_param.thread_id,strerror(errno), i,
                            my_thread_param.retry_attempts-retry_count+1, 
			    my_thread_param.retry_attempts );
          fflush(stderr);
          if (stop_on_error) {
              found_errors++;
              goto free_all;
          }
          if (--retry_count) {
            sleep(1);
            goto try_write;
          }
          break;
        }
        if (my_thread_param.fsync_flag) 
	  sync_file_changes(buffers[i].fp);
        filesize -= this_len;
       }/*while*/
      }
      if (my_thread_param.time_profile) {
          memset( &end_time, 0, sizeof(end_time) );
          time_res = -1;
          for( l=0; l<GET_TIME_RETRY_COUNT; l++ ) {
              time_res = gettimeofday( &end_time, NULL );
              if (time_res == 0) break;
          }
          if (time_res == 0) {
            //if (verbose) 
              write_file_time = (double)((end_time.tv_sec - start_time.tv_sec)*1000000 +
                               end_time.tv_usec - start_time.tv_usec)/1000000;
#if 0
              if (write_file_time == 0) write_file_time = 1;
#endif
              if (write_file_time == 0) write_file_time = (double)1.0/CLOCKS_PER_SEC;
              if (write_file_time <  0) write_file_time = (double)1.0/CLOCKS_PER_SEC;
              //printf( "%llu\n", buffers[i].fsize );
              write_file_speed = (buffers[i].fsize/1000000) / write_file_time;
              printf( "\nThread %d: write_time_prof %.3f sec for file %d (speed %.3f MB/sec)\n", 
                      my_thread_param.thread_id, write_file_time, i, write_file_speed );
              fflush(stdout);
              if (summary_stat.write_file_sliding_average_time == 0.0) {
                summary_stat.write_file_sliding_average_time = write_file_time;
                summary_stat.write_file_sliding_average_speed = write_file_speed;
              }
              else {
                 summary_stat.write_file_sliding_average_time = (
                 summary_stat.write_file_sliding_average_time + write_file_time) / 2;
                 summary_stat.write_file_sliding_average_speed = (
                 summary_stat.write_file_sliding_average_speed + write_file_speed) / 2;
              }
          }
          else {
              fprintf( stderr, "Thread %d: ERR [end_time not available]: %s for file %d\n",
                               my_thread_param.thread_id,strerror(errno), i );
              fflush(stderr);
          }
      }  
      buffers[i].chksum = chksum;
      if (verbose) {
        printf( "\nThread %d: Additive sum %llu for file %d\n", 
                 my_thread_param.thread_id,buffers[i].chksum, i ); 
        fflush(stdout);
      }
    }
  } /*for*/


/* Read test */
  if (user_break_exit > 1) goto free_all;
  if (verbose) { printf( "Thread %d: Read files to memory buffers.\n",
                        my_thread_param.thread_id );
                 fflush(stdout);
  }

  for(i=0; i<max_elem_num; i++) {  
    if ((buffers[i].r_pbuf != NULL) && (buffers[i].fp != NULL)) {
        retry_count = my_thread_param.retry_attempts;
   try_lseek_read:
      fflush(buffers[i].fp);
      if (fseek( buffers[i].fp, 0, SEEK_SET ) == -1) {
        thread_stat.other_errors++;
        fprintf( stderr, "Thread %d: ERR [seek/r]: %s for file %d (retry %d from %d)\n", 
                          my_thread_param.thread_id,strerror(errno), i,
                          my_thread_param.retry_attempts-retry_count+1, 
			  my_thread_param.retry_attempts );
        fflush(stderr);
        if (stop_on_error) {
            found_errors++;
            goto free_all;
        }
        if (--retry_count) {
          sleep(1);
          goto try_lseek_read;
        }
        continue;
      }
      chksum = 0;      
      filesize = buffers[i].fsize;
      if (verbose) { 
          printf( "Thread %d: Read file %d: ",my_thread_param.thread_id,i );
          fflush(stdout);
      }
      else if (quiet == 0) printf( "r" );
      if (my_thread_param.time_profile) {
          memset( &start_time, 0, sizeof(start_time) );
          time_res = -1;
          for( l=0; l<GET_TIME_RETRY_COUNT; l++ ) {
              time_res = gettimeofday( &start_time, NULL );
              if (time_res == 0) break;
          }
          if (time_res != 0) {
            fprintf( stderr, "Thread %d: ERR [start_time not available on_read]: %s for file %d\n",
                             my_thread_param.thread_id,strerror(errno), i );
            fflush(stderr);
          }
      }
      while( filesize ) {
        mult_rw_op = '*';      
        if ( filesize < buffers[i].msize) {
            this_len = filesize;
            mult_rw_op = '.';
        }
        else {
            this_len = buffers[i].msize;
        }
        if (quiet == 0) { 
          printf( "%c", mult_rw_op );  
          fflush(stdout);
        }
        if (use_alarm) alarm(io_alarm_secs);        
        retry_count = my_thread_param.retry_attempts;
     try_read:
        this_count = this_len / ONE_MEGABYTE;

        count = fread( buffers[i].r_pbuf, ONE_MEGABYTE, this_count, buffers[i].fp );
        len = count * ONE_MEGABYTE;
        if (len == this_len) thread_stat.read_bytes += len;
        if (use_alarm) alarm(0);
        if (len != this_len) {
          thread_stat.other_errors++;
          fprintf( stderr, "\nThread %d: ERR [read]: %s for buffer %d (retry %d from %d)\n", 
                             my_thread_param.thread_id,strerror(errno), i,
                             my_thread_param.retry_attempts-retry_count+1, 
			     my_thread_param.retry_attempts );
          fflush(stderr);
          if (stop_on_error) {
              found_errors++;
              goto free_all;
          }
          if (--retry_count) {
            sleep(1);
            goto try_read;
          }
          break;
        }
        p = buffers[i].r_pbuf;
        len = this_len;
        while( len--) {
          chksum += *p++;        
        }
        filesize -= this_len;
      }
      if (my_thread_param.time_profile) {
          memset( &end_time, 0, sizeof(end_time) );
          time_res = -1;
          for( l=0; l<GET_TIME_RETRY_COUNT; l++ ) {
              time_res = gettimeofday( &end_time, NULL );
              if (time_res == 0) break;
          }
          if (time_res == 0) {
            //if (verbose) 
              read_file_time = (double)((end_time.tv_sec - start_time.tv_sec)*1000000 +
                            end_time.tv_usec - start_time.tv_usec)/1000000;
#if 0
              if (read_file_time == 0) read_file_time = 1;
#endif
              if (read_file_time == 0) read_file_time = (double)1.0/CLOCKS_PER_SEC;
              if (read_file_time <  0) read_file_time = (double)1.0/CLOCKS_PER_SEC;
              read_file_speed = (buffers[i].fsize/1000000) / read_file_time;
              printf( "\nThread %d: read_time_prof %.3f sec for file %d (speed %.3f MB/sec)\n", 
                      my_thread_param.thread_id, read_file_time, i, read_file_speed ); 
              fflush(stdout);
              if (summary_stat.read_file_sliding_average_time == 0.0) {
                summary_stat.read_file_sliding_average_time = read_file_time;
                summary_stat.read_file_sliding_average_speed = read_file_speed;
              }
              else {
                 summary_stat.read_file_sliding_average_time = (
                 summary_stat.read_file_sliding_average_time + read_file_time) / 2;
                 summary_stat.read_file_sliding_average_speed = (
                 summary_stat.read_file_sliding_average_speed + read_file_speed) / 2;
              }
          }
          else {
              fprintf( stderr, "Thread %d: ERR [end_time not available on_read]: %s for file %d\n",
                               my_thread_param.thread_id,strerror(errno), i );
              fflush(stderr);
          }
      }  
      if (verbose) { printf( "\nThread %d: Additive sum %llu for file %d\n", 
                             my_thread_param.thread_id,chksum, i );
                     fflush(stdout);
      }
#if __FORCE_COMPARE_ERROR__
      chksum = 123;
#endif
      if (buffers[i].chksum != chksum)  {
        thread_stat.compare_errors++;
        if (verbose) printf( "ERROR on chksum!!!\n" );
        fprintf( stderr, "\nThread %d: ERR [chksum]: mismatch additive sums, write=%llu, read=%llu\n",
                          my_thread_param.thread_id,buffers[i].chksum, chksum );
        fflush(stderr);
        if (stop_on_error) {
            found_errors++;
            goto free_all;                          
        }
      }
    }
  }

/* Look temporary results */  
#if 0
  sleep(15);
#endif

free_all:
/* Close file interface */  
  if (verbose) {
    printf( "Thread %d: Close and delete files.\n",my_thread_param.thread_id );
    fflush(stdout);
  }
  close_files(buffers);    

free_memory:
/* Return memory to system */
  if (verbose) {
    printf( "Thread %d: Free memory buffers.\n",my_thread_param.thread_id );
    fflush(stdout);
  }
  free_buffers(buffers);

  if (shadow_buffers_copy) {
    for(i=0; i<max_elem_num; i++) {
       memset( &shadow_buffers[my_thread_param.thread_id*max_elem_num+i],
               0, sizeof(struct buf_elem) );
    }	       
  }
  
/* Check condition to go */    
  if (LogFile != NULL) {
    memset( log_buf, 0, sizeof(log_buf) );
    snprintf( log_buf, sizeof(log_buf)-1, "Thread %d: Finished pass %lu.\n", 
                                          my_thread_param.thread_id,pass_num+1 );
    write_log( LogFile, log_buf );
  }
  pass_num++;
  if (verbose) { printf( "Thread %d: Pass %lu finished.\n", 
                        my_thread_param.thread_id,pass_num );
                 fflush(stdout);
  }
  
  if (user_break_exit) { 
     if (verbose) {
         printf( "Thread %d: (process %d) terminated by user break.\n", 
                  my_thread_param.thread_id, (unsigned int)cur_pid );
         fflush(stdout);
     }
     goto done;
  }
  if (stop_on_error && found_errors) { 
     fprintf( stderr, "Thread %d: (process %d) terminated on encountered error.\n", 
                      my_thread_param.thread_id, (unsigned int)cur_pid );
     fflush(stderr);
     goto done;
  }
  if (my_thread_param.loop_forever) goto forever;  
  if (forever_flag) {
    fprintf( stderr, "Thread %d: ERR [unexpected end of forever-loop]: loop_forever flag was reset?!\n",
                      my_thread_param.thread_id );
    fflush(stderr);
  }
  passes_count--;  
  if (passes_count) goto forever;

done:
  if (verbose) {
       printf( "Thread %d: (process %u) terminated.\n", 
               my_thread_param.thread_id, (unsigned int)cur_pid );
       fflush(stdout);
  }

#if _WIN32  
  SetEvent( g_hEventThreadsObj[my_thread_param.thread_id]);
#endif

  if (collect_summary_statistics) {
     summary_stat.passes_count += pass_num;
     summary_stat.read_bytes += thread_stat.read_bytes;
     summary_stat.write_bytes += thread_stat.write_bytes;
     summary_stat.compare_errors += thread_stat.compare_errors;
     summary_stat.other_errors += thread_stat.other_errors;
  }
  
  return(NULL);
}


/*
 * Search for prime numbers
 */
static void prime_numbers( unsigned long int max_int_num )
{
  unsigned long int l;
  unsigned long int i;
  unsigned long int n;
  int               found;

  /* Check Parameters */
  if (max_int_num > MAX_PRIMES_NUM) max_int_num = MAX_PRIMES_NUM;
  if (verbose) printf( "Search for prime numbers upto %lu.\n", max_int_num );    
  
  /* Init table */  
  memset(primes_table,0,sizeof(primes_table) );
  primes_table[primes_found] = 2;
  primes_found++;

  /* Search for numbers */
  for( l=3; l<=max_int_num; l++ ) {
     found = 1;
     //printf( "l=%lu\n",l);
     for( i=2; i<l; i++ ) {
        n = l/i;
	//printf( "l,i,n=%lu,%lu,%lu\n",l,i,n );
	if (n*i == l) {
	   found = 0;
	   break;
	}
     }
     if (found) {
	primes_table[primes_found] = l;
	primes_found++;
     }
  }/*for*/
}

 
/*
 *  Print help screen
 */
static void usage(void)
{
  fprintf( stderr, "\n" );
  fprintf( stderr, "File I/O testing program (with pthreads), version %s\n", prog_ver );
  fprintf( stderr, "Copyright (C) 2002-2019 Dmitry Stefankov. All rights reserved.\n" );
  fprintf( stderr, "Usage: ftest3 [-ABDdEeFfGghlMmOqrSRuyvWzYZ] [-n filecount] [-s bufsize] [-f filesize]\n" );
  fprintf( stderr, "       [-p passes] [-t workdir] [-a io_tmout_secs] [-L logfile] [-j threads]\n" );
  fprintf( stderr, "       [-w usec] [-P max_int_num] [-o actv_mon_tmout] [-T pattern]\n" );
  fprintf( stderr, "       [-J retry_attempts] [-N run_seconds] [-b auto_buf_size)\n" );
  fprintf( stderr, "       -s   memory buffer size in MegaBytes (default=%d)\n", DEF_BUF_SIZE );
  fprintf( stderr, "       -f   file size in MegaBytes (default=%d)\n", DEF_FILE_SIZE );
  fprintf( stderr, "       -A   use alarm control for r/w ops (default=no)\n" );
  fprintf( stderr, "       -d   direct writing (no caching)\n" );
  fprintf( stderr, "       -h   this help\n" );
  fprintf( stderr, "       -e   memory operations only (no file operations!)\n" );
  fprintf( stderr, "       -l   run forever\n" );
  fprintf( stderr, "       -q   quiet mode (full silence)\n" );
  fprintf( stderr, "       -E   stop on first encountered error\n" );
  fprintf( stderr, "       -m   separate buffers for read and write\n" );
  fprintf( stderr, "       -r   fill buffers with random values\n" );  
  fprintf( stderr, "       -y   synchronous writes (set flag on file_open)\n" );
  fprintf( stderr, "       -Y   synchronous writes (force write using fsync)\n" );
  fprintf( stderr, "       -v   verbose output\n" );
  fprintf( stderr, "       -B   auto all memory testing (default=no)\n" );
  fprintf( stderr, "       -b   auto memory buffer size in MegaBytes (default=%d)\n", AUTO_BUF_SIZE );
  fprintf( stderr, "       -z   don't delete temporary files\n" );
  fprintf( stderr, "       -Z   enable time profiling on the full file read/write operations\n" );
  fprintf( stderr, "       -M   disable summary statistics collection (default=enable)\n" );
  fprintf( stderr, "       -j   number of threads\n" );
  fprintf( stderr, "       -g   use built-in RNG (like unix)\n" );
  fprintf( stderr, "       -G   use built-in RNG (like Microsoft Windows)\n" );
  fprintf( stderr, "       -w   wait microsec before run each thread (default=0)\n" );
  fprintf( stderr, "       -u   disable async. user break signal (default=enable)\n" );
  fprintf( stderr, "       -P   max.int.num to search prime numbers (def.=0, rec.=10000+)\n" );
  fprintf( stderr, "       -O   use active monitoring of threads state\n" );
  fprintf( stderr, "       -o   active monitoring time interval (default=none)\n" );
  fprintf( stderr, "       -R   re-run terminated threads (default=no)\n" );
  fprintf( stderr, "       -S   enable to sleep N random seconds in each thread (default=no)\n" );
  fprintf( stderr, "       -T   text string pattern to fill a buffer (default=none)\n" );
  fprintf( stderr, "       -J   number of retry attempts on error (default=%d)\n", retry_attempts );
  fprintf( stderr, "       -W   use an incremental filenames (default=none)\n" );
  fprintf( stderr, "       -n   number of files per thread (sequentially processing)\n" );
  fprintf( stderr, "       -p   number of cycles (passes)\n" );
  fprintf( stderr, "       -a   timeout for i/o operations\n" );
  fprintf( stderr, "       -t   working catalog\n" );
  fprintf( stderr, "       -L   logfile instead console output (default=none)\n" );
  fprintf( stderr, "       -D   disable built-in checking for working catalog presence (default=check)\n" );
  fprintf( stderr, "       -N   number of seconds to run (default=%lu)\n", g_total_exec_time );
  fprintf( stderr, "Default parameters:\n" );
  fprintf( stderr, "   Passes number:    min=1, def=%d, max=unlimited\n", DEF_PASS_COUNT );
  fprintf( stderr, "   Files/buffers:    min=1, def=%d, max=%d\n", DEF_NUM_ELEMS, MAX_NUM_ELEMS );
  fprintf( stderr, "   Buffer size:      min=1 MB, def=%d MB, max=unlimited\n", DEF_BUF_SIZE );
  fprintf( stderr, "   File   size:      min=1 MB, def=%d MB, max=unlimited\n", DEF_FILE_SIZE );
  fprintf( stderr, "   Buf.part.size:    min=1 MB, def=%d MB, max=%d\n", DEF_BUF_PART_SIZE, MAX_BUF_PART_SIZE );
  fprintf( stderr, "   Threads number:   min=1, def=%d, max=%d\n", DEF_NUM_THREADS, MAX_NUM_THREADS );
  fprintf( stderr, "   Working catalog:  %s\n", work_dir );
  fprintf( stderr, "   I/O op. timeout:  %d seconds\n", ALARM_SECS );
  fprintf( stderr, "   Time exec.limit:  %lu seconds\n", g_total_exec_time );
  fprintf( stderr, "   Auto Buffer size: min=1 MB, def=%d MB, max=unlimited\n", AUTO_BUF_SIZE );
  fprintf( stderr, "Screen output symbols:\n" );
  fprintf( stderr, "   w=writing, r=reading, *=full buffer, .=partial buffer" );
  fprintf( stderr, "\n" );
  fprintf( stderr, "Abbreviations:\n" );
  fprintf( stderr, "   MB/sec=MegaBytes per second" );    
  fprintf( stderr, "\n" );
  fprintf( stderr, "Sample command line:\n" );
  fprintf( stderr, "   ./ftest3 -vluSRO -j 2 -s 100 -f 6500 -t /test_dir -o 60\n" );
  fprintf( stderr, "   ./ftest3 -vlr -s 400 -f 1600 -t /media1\n" );
  fprintf( stderr, "   ./ftest3 -vlr -s 500 -T abc\n" );
  fprintf( stderr, "   ./ftest3 -Bv\n" );
  fprintf( stderr, "   ./ftest3 -Bvrl -b 4000 -t /usr/tmp\n" );
  fprintf( stderr, "\n" );
  exit(1);
}


/*
 *  Run Posix thread (pthread) or Windows thread (Win32)
 */
static int run_thread(int thread_no)
{
#ifdef _WIN32  
    HANDLE              hThread = NULL;
#endif
    int                 result_code;
    int                 j;
    void              * arg;
    void              * p;
    struct timeval      timeout;
    struct thread_param worker_param;      
#if _WIN32   
    DWORD               dwThreadId = 0;
#else
    pthread_t           thread_id;
#endif

    if (g_total_exec_time) {
#if _WIN32   
       hThread = CreateThread(
                    NULL,                   // SD
                    0,                      // initial stack size
  (LPTHREAD_START_ROUTINE)time_exec_task_thread,    // thread function
                   NULL,                   // thread argument
                    0,                     // creation option
                  &dwThreadId              // thread identifier
                    );
       if (verbose) {   
           printf( "time_exec_thread: create_thread result code id = %lu\n", dwThreadId );
       }
       if ((hThread == NULL) || (dwThreadId == 0)) {
           printf( "time_exec_thread: create_thread_error = %lu!\n", GetLastError() );
           return(4);
       }
       hThread = NULL;
#else
       result_code = pthread_create(&thread_id, NULL, time_exec_task_thread, NULL );
       if (verbose) {   
           printf( "time_exec_thread: create_thread result code = %d\n", result_code );
       }
       if (result_code != 0) {
           printf( "time_exec_thread: create_thread_error = %d!\n", result_code );
           return(10);
       }
#endif
    }
  
    j = thread_no;
    memset( &worker_param, 0, sizeof(worker_param) );
    worker_param.thread_id = j;
    worker_param.max_file_size = max_file_size;
    worker_param.passes_count = g_passes_count;
    worker_param.loop_forever = loop_forever;
    worker_param.memory_test_only = memory_test_only;
    worker_param.fill_rand = fill_rand;
    worker_param.rw_mult_bufs = rw_mult_bufs;
    worker_param.fsync_flag = fsync_flag;
    worker_param.dsync_flag = dsync_flag;
    worker_param.flags = flags;
    worker_param.fmode = fmode;
    worker_param.time_profile = time_profiling;
    worker_param.retry_attempts = retry_attempts;
    worker_param.increment_filenames = increment_filenames;
    p = malloc( sizeof(worker_param) );
    if (p == NULL) {
       printf( "thread %d: not enough memory to run thread!\n", j );
       return(3);
    }
    memcpy( p, &worker_param, sizeof(worker_param) );

    arg = p;
    if (max_int_num) prime_numbers(max_int_num);
    result_code = 0;
#if 1       
    if (us_wait_to_run_thread) {
          memset( &timeout, 0, sizeof(timeout) );
	  timeout.tv_usec = us_wait_to_run_thread % 1000000;
	  timeout.tv_sec = us_wait_to_run_thread / 1000000;
	  select(0,NULL,NULL,NULL,&timeout);
    }
#endif

#if _WIN32   
       hThread = CreateThread(
                    NULL,                   // SD
                    0,                      // initial stack size
  (LPTHREAD_START_ROUTINE)working_task_thread,    // thread function
                  arg,                     // thread argument
                    0,                     // creation option
           &threads_array[j]               // thread identifier
                    );
       if (verbose) {   
           printf( "thread %d: create_thread result code id = %lu\n", 
                   j, threads_array[j] );
       }
       g_ThreadHandles[j] = hThread;
       if ((hThread == NULL) || (threads_array[j] == 0)) {
           printf( "thread %d: create_thread_error = %lu!\n", 
                   j, GetLastError() );
           return(4);
       }
#else
       default_stack_size = 0;
       new_stack_size = 0;
       pthread_attr_init(&stack_size_custom_attr[j]);
       pthread_attr_getstacksize(&stack_size_custom_attr[j],&default_stack_size);
       if (verbose) {
           printf( "thread %d: default thread stacksize = %u\n", 
                    j, default_stack_size);   
       }
       if (default_stack_size < MIN_REQ_SSIZE) {
          pthread_attr_setstacksize(&stack_size_custom_attr[j],(size_t)MIN_REQ_SSIZE);    
          pthread_attr_getstacksize(&stack_size_custom_attr[j],&new_stack_size);    
          if (verbose) {
              printf( "thread %d: new thread stacksize = %u\n",  
                       j, new_stack_size);   
          }
          if (new_stack_size < MIN_REQ_SSIZE) {
              printf( "thread %d: not enough stack size to run thread!\n", j );
              return(2);
          }
       }

       result_code = pthread_create(&threads_array[j], &stack_size_custom_attr[j], working_task_thread, arg );
       if (verbose) {   
           printf( "thread %d: create_thread result code = %d\n", 
                   j, result_code );
       }
       if (result_code != 0) {
           printf( "thread %d: create_thread_error = %d!\n", 
                   j, result_code );
           return(6);
       }
#endif

   return(result_code);
}


#ifndef _WIN32
/*
 *  Get run-time limit specific value
 */
static int  get_rlimit_value( int val, struct rlimit * p )
{
   int res;
   
   if (p == NULL) return(-2);
   
   memset( p, 0, sizeof(struct rlimit) );
   res = getrlimit( val, p );
   
   return(res);
}

/*
 *  Print run-time limit specific value
 */
static void  print_rlimit_value( const char * str, int value )
{
    struct rlimit   rlp;
    int             res;
    char            cur_buf[32];
    char            max_buf[32];
    char            msg_buf[128];
    
    if (str == NULL) return;
  
    res = get_rlimit_value( value, &rlp );
    if (res == 0) {
        memset( cur_buf, 0, sizeof(cur_buf) );
        if (rlp.rlim_cur == RLIM_INFINITY) 
            strncpy(cur_buf, "INF", sizeof(cur_buf)-1 );
        else
#if defined(__linux__) 
            snprintf( cur_buf, sizeof(cur_buf)-1, "%lu", rlp.rlim_cur );
#else        
            snprintf( cur_buf, sizeof(cur_buf)-1, "%llu", rlp.rlim_cur );
#endif            
        memset( max_buf, 0, sizeof(max_buf) );
        if (rlp.rlim_max == RLIM_INFINITY) 
            strncpy(max_buf, "INF", sizeof(max_buf)-1 );
        else
#if defined(__linux__)
            snprintf( max_buf, sizeof(max_buf)-1, "%lu", rlp.rlim_max );
#else
            snprintf( max_buf, sizeof(max_buf)-1, "%llu", rlp.rlim_max );
#endif            
        memset( msg_buf, 0, sizeof(msg_buf) );
        snprintf( msg_buf, sizeof(msg_buf)-1, "%s  cur=%s, max=%s\n", 
                  str, cur_buf, max_buf );
        printf( msg_buf );
    }
}


/*
 *  Print run-time limits for current process and user
 */
static void  print_rlimit_resources( void )
{
    printf( "rlimit values:\n" );
    print_rlimit_value( " core     (bytes):  ",    RLIMIT_CORE );
    print_rlimit_value( " cpu      (seconds):",    RLIMIT_CPU );
    print_rlimit_value( " data     (bytes):  ",    RLIMIT_DATA );        
    print_rlimit_value( " fsize    (bytes):  ",    RLIMIT_FSIZE );    
#if defined(RLIMIT_MEMLOCK)        
    print_rlimit_value( " memlock  (bytes):  ",    RLIMIT_MEMLOCK );    
#endif    
    print_rlimit_value( " nofile   (num):    ",    RLIMIT_NOFILE );    
#if defined(RLIMIT_RSS)    
    print_rlimit_value( " rss      (bytes):  ",    RLIMIT_RSS );    
#endif    
    print_rlimit_value( " stack    (bytes):  ",    RLIMIT_STACK );    
#if defined(RLIMIT_SBSIZE)    
    print_rlimit_value( " sbsize   (bytes):  ",    RLIMIT_SBSIZE );    
#endif    
}

#endif



static int  detect_x64_platform_bitness( void )
{
  int result = 0;

  if (verbose) printf( "x64_test: sizeof(size_t)=%lu\n", sizeof(size_t) );
  if (sizeof(size_t) == 8) {
    result = 1;
  }

  return(result);
}


static unsigned long int  detect_free_memory_in_MB( void )
{
  unsigned long int  result = 0;
  unsigned long int  free_memory_size = 0;
  int res = 0;
  unsigned long int  hw_page_size = 0;
  size_t  old_size = 0;
  //size_t  new_size = 0;
  __int64  free_pages = 0;
  __int64  all_free_mem = 0;
  __int64  rlimit_data_size = 0;
  unsigned long int app_rlimit_data_size_mb = 0; 


#ifdef _WIN32  
  if (1) {
    SYSTEM_INFO siSysInfo;
    MEMORYSTATUSEX statex;
    GetSystemInfo(&siSysInfo); 
    hw_page_size = siSysInfo.dwPageSize;
    if (verbose) printf("hw_page_size=%lu\n", hw_page_size );
    if (verbose) printf("Minimum application address: 0x%lx\n", siSysInfo.lpMinimumApplicationAddress); 
    statex.dwLength = sizeof (statex);
    GlobalMemoryStatusEx (&statex);
    all_free_mem = statex.ullAvailPhys;
    free_memory_size = all_free_mem / ONE_MEGABYTE;
    if (verbose) printf( "all_free_mem=%llu free_memory_size=%lu\n", all_free_mem, free_memory_size );
    //rlimit_data_size = siSysInfo.lpMaximumApplicationAddress;
    //app_rlimit_data_size_mb = rlimit_data_size / ONE_MEGABYTE;
    result = free_memory_size;
  }
#endif

#if (__FreeBSD__)
  old_size = sizeof(hw_page_size);
  res = sysctlbyname( "hw.pagesize", (void *)&hw_page_size, &old_size, NULL, 0 );
  if (verbose) printf("res=%d hw_page_size=%lu\n", res, hw_page_size );
  if (res == 0) {
    struct rlimit   rlp;
    old_size = sizeof(free_pages);
    res = sysctlbyname( "vm.stats.vm.v_free_count", (void *)&free_pages, &old_size, NULL, 0 );
    if (verbose) printf("res=%d free_pages=%llu\n", res, free_pages );
    all_free_mem = free_pages * hw_page_size;
    free_memory_size = all_free_mem / ONE_MEGABYTE;
    if (verbose) printf( "all_free_mem=%llu free_memory_size=%lu\n", all_free_mem, free_memory_size );
    if (verbose) print_rlimit_resources();
    res = get_rlimit_value( RLIMIT_DATA, &rlp );
    if (res == 0) {
      rlimit_data_size = rlp.rlim_cur;
      if (verbose) printf( "rlimit_data_size=%llu\n", rlimit_data_size );
      app_rlimit_data_size_mb = rlimit_data_size / ONE_MEGABYTE;
      if (verbose) printf( "rlimit_data_size_mb=%lu\n", app_rlimit_data_size_mb );
      if (app_rlimit_data_size_mb < free_memory_size) {
        printf( "ERROR: Application data size not allowed to use auto-memory-testing mode!\n" );
      }
      else {
        printf( "Application data size IS allowed to use auto-memory-testing mode.\n" );
        result = free_memory_size;
      }
    }
  }
#endif

#if defined(__linux__)
    if (1) {
      struct sysinfo  si;
      __int64 free_ram = 0;
      unsigned int mem_unit;
      hw_page_size = getpagesize();
      if (verbose) printf("hw_page_size=%lu\n", hw_page_size );
      res = sysinfo(&si);
      //printf( "res=%d\n", res );
      if (res == 0) {
        mem_unit = si.mem_unit;
        free_ram = si.freeram;
        if (verbose) printf( "mem_unit=%u, freeram=%llu\n", mem_unit, free_ram );
        all_free_mem = free_ram * mem_unit;
        free_memory_size = all_free_mem / ONE_MEGABYTE;
        if (verbose) printf( "all_free_mem=%llu free_memory_size=%lu\n", all_free_mem, free_memory_size );
        result = free_memory_size;
      }
    }
#endif

  return(result);
}



/*
 *  Main program stream
 */
int main( int argc, char ** argv )
{
  int                 op;
  int                 j;
  int                 res;
#ifdef _WIN32  
  WORD 			wVersionRequested;
  WSADATA 		wsaData;
  int 			iErr;
  //HANDLE              hThread;
  DWORD               dwRes;
  BOOL                fResult;
  DWORD               dwExitCode;
#else
  sigset_t            mask;
  struct sigaction    sa;
  struct sched_param  pthread_sched_param;
  int                 pthread_policy;
#endif
  int                 result_code;
  int                 run_thread_again = 0;
  int                 threads_active_control = 0;
  int                 threads_actv_ctl_timeout = -1;
  int                 alive_threads_num;
  struct timeval      timeout;
  char              * s;
  char                temp_dir_buf[DIRNAME_SIZE+2];
  struct timeval      start_time;
  struct timeval      end_time;

/* Initialize */
  memset( (void *)&summary_stat, 0, sizeof(summary_stat) );

/* Process command line  */  
  opterr = 0;
  while( (op = getopt(argc,argv,"a:ABb:dDhEeFf:GgJ:j:L:lMmN:n:p:P:Oo:qrRs:ST:t:uvWw:yYzZ")) != -1)
    switch(op) {
      case 'D':
               check_work_dir_presence = 0;
               break;
      case 'J':
               retry_attempts = atoi(optarg);
	       if (retry_attempts == 0) retry_attempts = 1;
               break;
      case 'e':
               memory_test_only = 1;
               break;
      case 'E':
               stop_on_error = 1;
               break;
      case 'A':
               use_alarm = 1;
               break; 
      case 'a':
               io_alarm_secs = (unsigned int)atol(optarg);
               if (io_alarm_secs == 0) io_alarm_secs = ALARM_SECS;
      	       break;    
      case 'B':
               auto_memory_test = 1;
               break;
      case 'b':
               auto_buf_size = atoi(optarg);
               if (auto_buf_size == 0) auto_buf_size = AUTO_BUF_SIZE;
               break;                
      case 't': 
               out_dir = optarg;
               break;
      case 'T':
               fill_pattern = (unsigned char *)optarg;
               break;
      case 'd':
#ifdef O_DIRECT      
               flags |= O_DIRECT;
#else
               fprintf( stderr, "Warning: -d switch is not supported on this platform.\n" );               
#endif		
               break;
      case 'f':
               max_file_size = strtoul(optarg,NULL,10);
               if (max_file_size == 0) max_file_size = 1; 
               break;
      case 'j':
               max_threads_num = atoi(optarg);
               if (max_threads_num > MAX_NUM_THREADS) max_threads_num = MAX_NUM_THREADS;
	       if (max_threads_num == 0) max_threads_num = 1;
               break;	       
      case 'L':
               LogFile = optarg;
               break;
      case 'l':
               loop_forever = 1;
      	       break; 
      case 'm':
               rw_mult_bufs = 1;
      	       break;       	                      
      case 'M':
               collect_summary_statistics = 0;
               break;
      case 'N':
               g_total_exec_time = atol(optarg);
               loop_forever = 1;
               break;
      case 'n':
                max_elem_num = atoi(optarg);
                if (max_elem_num > MAX_NUM_ELEMS) max_elem_num = MAX_NUM_ELEMS;
                if (max_elem_num == 0) max_elem_num = 1;
                break;
      case 'p':
                g_passes_count = atol(optarg);
                if (g_passes_count == 0) g_passes_count = 1; 
                break;
      case 'r':
               fill_rand = 1;
      	       break;
      case 's':
               max_buf_size = atoi(optarg);
               if (max_buf_size == 0) max_buf_size = 1;
               break;                
      case 'S':
               enable_random_sleep_in_thread = 1;
               init_rnd_gen(0);
               break;         
      case 'O':
               threads_active_control = 1;
	       break;
      case 'o':		
               threads_actv_ctl_timeout = atol(optarg);
	       break;
      case 'R':
               run_thread_again = 1;
               break;    		
      case 'u':
               user_break_enable = 0;
	       break;
      case 'q':
               quiet = 1;
               break;
      case 'v':
               verbose = 1;
      	       break;       
      case 'w':
               us_wait_to_run_thread = atol(optarg);
               break;	       
      case 'z':
               delete_flag = 0;
               break;	      
      case 'Z':
               time_profiling = 1;
               break;
      case 'y':
#ifdef O_FSYNC
               flags |= O_FSYNC;
#else
  #if sun
               flags |= (O_DSYNC|O_RSYNC);
  #else
               fprintf( stderr, "Warning: -y switch is not supported on this platform.\n" );                 
  #endif               
#endif		
               dsync_flag = 1;
               break;
      case 'Y':
               fsync_flag = 1;
               break;      	       
      case 'W':
               increment_filenames = 1;
               break;
      case 'P':
               max_int_num = atol(optarg);
               if (max_int_num > MAX_PRIMES_NUM) max_int_num = MAX_PRIMES_NUM;
               break;         
      case 'g':
               use_unix_rng++;
               break;         
      case 'G':
               use_mswin_rng++;
               break;         
      case 'h':
               usage();
               break;   
      default:
               break;
    }


   /* check some options before starting */
   if (auto_memory_test) {
     unsigned long int free_memory_mbs = 0;
     if (!detect_x64_platform_bitness()) {
     //if (0) {
       fprintf( stderr, "ERROR: Auto-memory-testing mode available only for x64 platform/OS!\n" );
       return(20);
     }
     free_memory_mbs = detect_free_memory_in_MB();
     if (free_memory_mbs == 0) {
       fprintf( stderr, "ERROR: for auto-memory-testing mode cannot detect free memory size for this platform/OS!\n" );
       return(21);
     }
     if ((max_threads_num == DEF_NUM_THREADS) && (max_buf_size == DEF_BUF_SIZE)
         && (max_file_size == DEF_FILE_SIZE)) {
       int new_threads_num = 0;
       int new_buf_size = 0;
       int new_file_size = 0;
       if (verbose) printf( "Default base values not changed, then try to use auto-memory-testing mode.\n" );
       new_buf_size = auto_buf_size;
       if (free_memory_mbs < auto_buf_size) new_buf_size = (free_memory_mbs - 100);
       new_file_size = new_buf_size*2+new_buf_size/2;
       new_threads_num = free_memory_mbs / new_buf_size;
       printf( "New default base values: buf_size=%d file_size=%d threads_num=%d\n", 
               new_buf_size, new_file_size, new_threads_num );
       if (new_threads_num > 0) {
         max_threads_num = new_threads_num;
         max_buf_size = new_buf_size;
         max_file_size = new_file_size;
         printf( "Auto-memory-testing mode IS enabled.\n" );
       }
     }
     else {
       printf( "ERROR: Default base values was changed. Auto-memory-testing mode IS disabled!\n" );
     }
   }

   if (verbose) {
     if (use_unix_rng) {
       printf("Use built-in random-number-generator (like unix).\n" );
     }
     if (use_mswin_rng) {
       printf("Use built-in random-number-generator (like windows).\n" );
     }
     if ((use_mswin_rng==0) && (use_unix_rng==0)) {
       printf("Use system random-number-generator.\n" );
     }
   }

   /* Starting time */
   memset( &start_time, 0, sizeof(start_time) );
   memset( &end_time, 0, sizeof(end_time) );
   res = gettimeofday(&start_time, NULL);
   
   /* Check working directory */
   if (check_work_dir_presence) {
     memset( temp_dir_buf, 0, sizeof(temp_dir_buf) );
     s = getcwd( temp_dir_buf, sizeof(temp_dir_buf)-1 );
     if (s == NULL) {
       fprintf( stderr, "Cannot get working directory (error=%s).\n", strerror(errno) );
       return(11);
     }
     if (out_dir != NULL) { 
       res = chdir(out_dir);
       if (res < 0) {
         fprintf( stderr, "Cannot set a new working directory '%s'(error=%s).\n", 
                  out_dir, strerror(errno) );
         return(12);
       }
     }
     res = chdir(temp_dir_buf);
     if (res < 0) {
       fprintf( stderr, "Cannot set a old working directory '%s'(error=%s).\n", 
                temp_dir_buf, strerror(errno) );
       return(13);
     }
   }
   
   /* Check some settings */
   if (threads_active_control && (threads_actv_ctl_timeout == -1)) {
       fprintf( stderr, "You must set threads monitoring time interval!\n" );
       return(7);
   }

   /* Disable buffering */
   res = setvbuf( stdout, (char *)NULL, _IONBF, 0 );
   if (res != 0) 
     fprintf( stderr, "WARNING! Cannot set unbuffered output stream!" );
   
       
#if _WIN32
   wVersionRequested = MAKEWORD( 1, 1 );
   iErr = WSAStartup( wVersionRequested, &wsaData );
   if ( iErr != 0 ) {
     fprintf(stderr,"ERROR: No Windows Sockests found.\n");
     return(1);
   }
   signal(SIGSEGV,  term );   
   signal(SIGBREAK, term );
   signal(SIGTERM,  term );
   signal(SIGINT,   term );
#else
   sigemptyset(&mask);
   sa.sa_mask = mask;
   sa.sa_flags = 0; 
   SIGNAL(SIGALRM, term );   
   SIGNAL(SIGHUP, term );
   SIGNAL(SIGTERM, term );
   SIGNAL(SIGINT, term );
#endif

  memset( threads_array, 0, sizeof(threads_array) );
  memset( shadow_buffers, 0, sizeof(shadow_buffers) );

#if _WIN32
  dwDefThreadWaitTime = (io_alarm_secs * 2) * MS_PER_SEC;
  for( j=0; j<MAX_NUM_THREADS; j++ ) {
      g_hEventThreadsObj[j] = CreateEvent(  
                              NULL,         /* no security attributes */
                              FALSE,        /* manual-reset event */
                              FALSE,        /* initial state is signaled */
                              NULL          /* object name */
                                         ); 

      if (g_hEventThreadsObj[j] == NULL) { 
         printf( "Cannot create event object for thread %d!\n", j );
         return(5);
      }
  }
#endif

  /* Run subprocesses */  
  for( j=0; j<max_threads_num; j++ ) {

       if (verbose) printf("Try to run thread %d.\n", j );
       result_code = run_thread(j);
       if (result_code != 0) {
          printf( "Thread creation failed. Terminate.\n" );
          return(10);
       }
  }

  /* Active monitoring */
  if (threads_active_control) {
     alive_threads_num = max_threads_num;
     while( 1 ) {
     
         memset( &timeout, 0, sizeof(timeout) );
         timeout.tv_usec = 0;
         timeout.tv_sec = threads_actv_ctl_timeout;
         select(0,NULL,NULL,NULL,&timeout);

	 for( j=0; j<max_threads_num; j++ ) {
	     if (threads_array[j]) {
#if _WIN32  
                 res = 1;
                 fResult = GetExitCodeThread(g_ThreadHandles[j],&dwExitCode);
                 if (fResult && (dwExitCode == STILL_ACTIVE)) res = 0;
#else	 
	         res = pthread_getschedparam( threads_array[j], &pthread_policy,
                                              &pthread_sched_param);
                 //printf( "Thread %d status_result=%d\n",j, res);					      
#endif
	         if (res != 0) {
		    threads_array[j] = 0;
		    alive_threads_num--;
		    if (verbose) printf( "thread [%d]: status done.\n", j );
		    if (run_thread_again == 0) continue;

                    if (user_break_exit) continue;
                    /* Run thread again and again. Forever? */
                    result_code = run_thread(j);
                    if (verbose) {   
                        printf( "thread %d: create_thread result code = %d\n", 
                                j, result_code );
                    }
                    if (result_code != 0) {
                        printf( "thread %d: create_thread_error = %d!\n", 
                                j, result_code );
                        return(6);
                    }
                    alive_threads_num++;
		    continue;
		 }    
	     }/*if*/			  
         }/*for*/				      
	 if (alive_threads_num == 0) {
	    if (verbose) printf( "Terminate. No more active threads.\n" );
	    break;
	}    
     }
     goto done;
  }
  
 /* Passive wait for termination */
  for( j=0; j<max_threads_num; j++ ) {
      if (threads_array[j]) {
#if _WIN32
         if (g_hEventThreadsObj[j] == NULL) { 
             printf( "[ERR] thread %d don't have event object. Skip it.\n", j );
             continue;
         }

         //dwRes = WaitForSingleObject( g_hEventThreadsObj[j], dwDefThreadWaitTime );
	 dwRes = WaitForSingleObject( g_hEventThreadsObj[j], INFINITE );

         switch( dwRes ) {
             case WAIT_TIMEOUT:
                 if (verbose) printf( "I/O timeout in wait_thread.\n" );
                 CloseHandle(g_hEventObj);
                 term( SIGTERM );
                 break;
             case WAIT_OBJECT_0:
                 break;
             case WAIT_ABANDONED:
                 break;
             default:
                 break;
         }
         CloseHandle(g_hEventThreadsObj[j]);
#else
         pthread_join( threads_array[j], NULL );
#endif
         if (verbose) printf( "thread [%d]: status done.\n", j );
      }
  }

done:
// For Windows
#if 0  // _WIN32
   shadow_close_files();
   shadow_free_buffers();   
#endif   

   /* End time */
   res = gettimeofday(&end_time, NULL);
  
  /* Output summary results */
  if (collect_summary_statistics && (quiet == 0)) {
      printf( "\nSummary statistics:\n" );
      printf( "Total_exec_time=%u seconds\n",end_time.tv_sec-start_time.tv_sec);
      if (g_total_exec_time) {
        printf( "Run_exec_limit_time=%lu seconds\n",g_total_exec_time);
      }
      printf( "Passes=%lu\n",summary_stat.passes_count );
      printf( "Read_Bytes=%llu", summary_stat.read_bytes );
      printf( "   " );
      printf( "Write_Bytes=%llu\n", summary_stat.write_bytes );
      printf( "Compare_Errors=%lu   Other_Errors=%lu\n",
              summary_stat.compare_errors, summary_stat.other_errors );
      if (time_profiling) {
         printf( "Sliding average time for file writing = %.3f sec\n",
                 summary_stat.write_file_sliding_average_time );
         printf( "Sliding average time for file reading = %.3f sec\n",
                 summary_stat.read_file_sliding_average_time );
         printf( "Sliding average speed for file writing = %.3f MB/sec\n",
                 summary_stat.write_file_sliding_average_speed );
         printf( "Sliding average speed for file reading = %.3f MB/sec\n",
                 summary_stat.read_file_sliding_average_speed );
      }
  }

  return(0);
}
