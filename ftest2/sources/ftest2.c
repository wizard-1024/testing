/*****************************************************************************
 *                              File ftest2.c                                 
 *
 *              Simple I/O test bechmark program with Posix threads
 *
 *      Copyright (c) Dmitry V. Stefankov, 2006-2007. All rights reserved.
 *
 *****************************************************************************/
/*
 *   $Source: /usr/local/src/projects/tests/ftest2/RCS/ftest2.c,v $
 *  $RCSfile: ftest2.c,v $
 *   $Author: dstef $
 *     $Date: 2007-03-14 15:57:39+03 $
 * $Revision: 1.17 $
 *   $Locker: root $
 *
 *      $Log: ftest2.c,v $
 *      Revision 1.17  2007-03-14 15:57:39+03  dstef
 *      Minor fix for Windows MGNU compiler
 *
 *      Revision 1.16  2007-03-14 14:38:03+03  dstef
 *      Added switch -M to disable summary statistics collection
 *
 *      Revision 1.15  2007-03-14 14:34:23+03  dstef
 *      Added summary statistics
 *
 *      Revision 1.14  2007-02-09 14:22:57+03  dstef
 *      Set defaults for buffers and threads to 1
 *      Updated help
 *
 *      Revision 1.13  2007-02-03 13:32:40+03  dstef
 *      Added support for 64-bit file sizes (Microsoft Windows)
 *
 *      Revision 1.12  2007-02-02 23:19:53+03  dstef
 *      Added support for file with sizes > 4 Gbytes
 *
 *      Revision 1.11  2006-11-16 15:07:51+03  dstef
 *      Added support for Visual Studio 2005
 *
 *      Revision 1.10  2006-11-16 13:50:31+03  dstef
 *      Added buffers zeroing
 *
 *      Revision 1.9  2006-07-04 20:39:53+04  dstef
 *      More clear code for thread run
 *      Added -S option (random sleeping)
 *
 *      Revision 1.8  2006-06-28 19:45:00+04  dstef
 *      Added active monitoring and re-run thread tasks
 *      Minor fixes
 *
 *      Revision 1.7  2006-06-28 17:35:47+04  dstef
 *      Added user mode waste time option (-P xxx)
 *
 *      Revision 1.6  2006-06-27 11:43:42+04  dstef
 *      Added wait time option before run thread
 *
 *      Revision 1.5  2006-06-26 10:29:19+04  dstef
 *      Added more user controlled break
 *
 *      Revision 1.4  2006-06-25 14:32:08+04  dstef
 *      Fixed some problems for Win32 platform
 *
 *      Revision 1.3  2006-06-25 12:47:49+04  dstef
 *      Porting to Win32 platform
 *
 *      Revision 1.2  2006-06-25 11:26:50+04  dstef
 *      Added shadow copy of buffers to close/delete on user break
 *
 *      Revision 1.1  2006-06-25 00:08:30+04  dstef
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
#include  <io.h>
#endif

#if _WIN32
#include "getopt.h"
#endif

#if 1
#define  FILE_SIZE_MORE_4G  1
#endif
#if 0
#define  FILE_SIZE_MORE_4G  0
#endif

/* Definitions (OS-dependent) */

#if _WIN32

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
#define  DEF_FILE_SIZE      16
#define  FILENAME_SIZE	    255
#define  DIRNAME_SIZE	    FILENAME_SIZE

#define  DEF_PASS_COUNT 1

#define  ALARM_SECS	3600
#define  MS_PER_SEC     1000

#define  KILOBYTES  1024
#define  MEGABYTES  (KILOBYTES*1024)


/* Data structures */

struct buf_elem {
   unsigned long int msize;
#if FILE_SIZE_MORE_4G   
  #if _MSC_VER > 1000
     __int64  fsize;
  #else
     unsigned long long int fsize;
  #endif
#else
   unsigned long int fsize;
#endif      
   void * r_pbuf;                             
   void * w_pbuf;   
   int  fdes;
   char filename[FILENAME_SIZE];
   unsigned long int chksum;   
};

struct  thread_param
{
   int  thread_id;
   int  max_file_size;
   int  passes_count;
   int  loop_forever;
   int  fill_rand;
   int  rw_mult_bufs;
   int  sync_flag;
   int  flags;
   int  fmode;
};

struct  stat_info
{
#if FILE_SIZE_MORE_4G  
  #if _MSC_VER > 1000
     __int64  read_bytes;
     __int64  write_bytes;
  #else
     unsigned long long int   read_bytes;
     unsigned long long int   write_bytes;
  #endif
#else
  unsigned long int   read_bytes;
  unsigned long int   write_bytes;
#endif  
  unsigned long int   compare_errors;
  unsigned long int   other_errors;
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

int  max_elem_num  = DEF_NUM_ELEMS;
int  max_buf_size  = DEF_BUF_SIZE;
int  max_file_size = DEF_FILE_SIZE;
int  max_threads_num = DEF_NUM_THREADS;

#if _WIN32
int                 flags = O_RDWR|O_CREAT|O_TRUNC|O_BINARY;
#if _MSVC
     int    fmode = _S_IREAD | _S_IWRITE;
#else
     mode_t fmode = _S_IRUSR|_S_IWUSR;
#endif
const char    dir_sep_str[] = "\\";
char          work_dir[DIRNAME_SIZE] = "c:\\temp";
HANDLE        g_hEventObj = NULL;
HANDLE        g_hWaitingThread = NULL;
DWORD         g_dwThreadId = 0;
DWORD         g_dwThreadArg = 0;
DWORD         threads_array[MAX_NUM_THREADS];
HANDLE        g_hEventThreadsObj[MAX_NUM_THREADS];
HANDLE        g_ThreadHandles[MAX_NUM_THREADS];
DWORD         dwDefThreadWaitTime = (ALARM_SECS * 2) * MS_PER_SEC;
#else
int           flags = O_RDWR|O_CREAT|O_TRUNC;
mode_t        fmode = S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH;
const char    dir_sep_str[] = "/";
char  work_dir[DIRNAME_SIZE] = "/tmp";
size_t        default_stack_size = 0;
size_t        new_stack_size = 0;
pthread_attr_t      stack_size_custom_attr[MAX_NUM_THREADS];
pthread_t     threads_array[MAX_NUM_THREADS];
#endif

char         * out_dir = NULL;
char         * LogFile = NULL;
int           delete_flag = 1;
int           verbose = 0;
int           shadow_buffers_copy = 1;
int           use_alarm = 0;
unsigned int  io_alarm_secs = ALARM_SECS;
int           user_break_enable = 1;
int           user_break_exit = 0;
int           loop_forever = 0;
int           rw_mult_bufs = 0;
int           sync_flag = 0;
int           fill_rand = 0;
int           collect_summary_statistics = 1;
int           enable_random_sleep_in_thread = 0;
unsigned long int   passes_count = DEF_PASS_COUNT;
unsigned long int   us_wait_to_run_thread = 0;

volatile struct stat_info  summary_stat;

const char prog_ver[] = "1.3.1";
const char rcs_id[] = "$Id: ftest2.c,v 1.17 2007-03-14 15:57:39+03 dstef Exp root $";

struct buf_elem  shadow_buffers[MAX_NUM_ELEMS*MAX_NUM_THREADS];

unsigned long int primes_found = 0;
unsigned long int max_int_num = 0;
unsigned long int primes_table[MAX_PRIMES_NUM];

  
/*----------------------- Functions ---------------------------------------*/


/*
 * Print standard message from signal handler
 */
void print_message_signal( char * msg )
{
   if (msg == NULL) return;
#if _WIN32
   write( fileno(stdin), msg, strlen(msg) );
#else
   else write( STDIN_FILENO, msg, strlen(msg) );
#endif
}


/*
 * Print error message from signal handler
 */
void print_error_message_signal( char * msg )
{
   if (msg == NULL) return;
#if _WIN32
   write( fileno(stderr), msg, strlen(msg) );
#else
   write( STDERR_FILENO, msg, strlen(msg) );
#endif
}


/*
 * Write message to logfile
 */
int write_log( char * filename, char * buf )
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
     strftime( time_buf, sizeof(time_buf), "%a %b %d %H:%M:%S %Y", cur_tm );  
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
void init_rnd_gen(int ofs)
{ 
#if _WIN32
         srand( (unsigned)time(NULL)+ofs );
#else
         srandom( time(NULL)+ofs );
#endif
}

/*
 * Get next random number
 */
unsigned char next_rnd_num(void)
{
  unsigned char rn;
#if _WIN32
  rn = rand() & 0xFF;
#else
  rn = random() & 0xFF;
#endif  
  return(rn);
}

/*
 * Synchronize changes to file
 */
void sync_file_changes(int fd)
{
#if _WIN32
   _commit(fd);
#else
   fsync(fd);
#endif
}

    

/*
 *  Close all open files
 */
void close_files( struct buf_elem * buffers )
{
  int    i;
  int    res;
  char  msg_buf[128];

  for(i=0; i<max_elem_num; i++) {  
    if ((buffers[i].w_pbuf != NULL) && (buffers[i].fdes != -1)) {  
      res = close(buffers[i].fdes);
      if (res == -1) {
        snprintf( msg_buf, sizeof(msg_buf), 
                  "ERR [close]: %s for buffer %d\n", strerror(errno),i );
        print_error_message_signal( msg_buf );        
      }
      if (delete_flag) {
          res = unlink(buffers[i].filename);     
          if (res == -1) {
              snprintf( msg_buf, sizeof(msg_buf), 
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
void free_buffers( struct buf_elem * buffers )
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
void shadow_close_files( void )
{
  int    i;
  int    res;
  char  msg_buf[128];

  if (verbose) printf( "Shadow_close_files.\n" );

  for(i=0; i<(max_elem_num*max_threads_num); i++) {  
    if ((shadow_buffers[i].w_pbuf != NULL) && (shadow_buffers[i].fdes != -1)) {  
      res = close(shadow_buffers[i].fdes);
      if (res == -1) {
        snprintf( msg_buf, sizeof(msg_buf), 
                  "ERR [close]: %s for buffer %d\n", strerror(errno),i );
        print_error_message_signal( msg_buf );        
      }
      if (delete_flag) {
          res = unlink(shadow_buffers[i].filename);     
          if (res == -1) {
              snprintf( msg_buf, sizeof(msg_buf), 
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
void shadow_free_buffers( void )
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
  
   snprintf( msg_buf, sizeof(msg_buf), "\nsignal() %d\n", sig );
   print_message_signal( msg_buf );
   if (user_break_enable) {
      user_break_exit = 1;
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
DWORD WaitingThread( LPDWORD lpdwParam )
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
unsigned int alarm(unsigned int seconds)
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
 *  Working thread 
 */
void * working_task_thread(void * arg)
{
  struct thread_param  my_thread_param;  
  int                 i;
  int                 my_sleep_secs;
  char                mult_rw_op;
  char                temp_space[20];
  char                num_space[20];
  char                log_buf[256];
  unsigned long int   len;
#if FILE_SIZE_MORE_4G  
  #if _MSC_VER > 1000
     __int64  filesize;
  #else
     unsigned long long int   filesize;
  #endif
#else
  unsigned long int   filesize;
#endif  
  unsigned long int   this_len;
  unsigned long int   passes_count;
  unsigned long int   pass_num = 0;
  unsigned long int   chksum;
  unsigned char *     p;
#if _WIN32
  DWORD               cur_pid;
  DWORD               this_ptid;
#else
  pid_t               cur_pid;
  pthread_t           this_ptid;
#endif
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
  memcpy( &my_thread_param, (struct thread_param *)arg, sizeof(struct thread_param) );
  if (verbose) {
       printf( "Thread %d: (process %u) started.\n", 
               my_thread_param.thread_id, cur_pid );
  }
  passes_count = my_thread_param.passes_count;

forever:
/* Initialize */
  if (user_break_exit) goto free_all;
  if (LogFile != NULL) {
    snprintf( log_buf, sizeof(log_buf), "Thread %d: Started pass %lu.\n", 
                                    my_thread_param.thread_id, pass_num+1 );
    write_log( LogFile, log_buf );
  }
  if (verbose)
    printf( "Thread %d: Init structures. %u buffers, each size %u MBytes\n",
                      my_thread_param.thread_id, max_elem_num, max_buf_size );
    
  memset( buffers, 0, sizeof(buffers) );    
  for(i=0; i<max_elem_num; i++) {
    buffers[i].msize = max_buf_size * MEGABYTES;
#if FILE_SIZE_MORE_4G      
   #if _MSC_VER > 1000
      buffers[i].fsize = (__int64)max_file_size * MEGABYTES;
   #else
      buffers[i].fsize = (unsigned long long int)max_file_size * MEGABYTES;
   #endif
#else
    buffers[i].fsize = max_file_size * MEGABYTES;
#endif    
    buffers[i].r_pbuf = NULL;
    buffers[i].w_pbuf = NULL;    
    buffers[i].fdes = -1;
    buffers[i].chksum = -1;
    memset( buffers[i].filename,0,sizeof(buffers[i].filename) );
    if (shadow_buffers_copy) {
       memcpy( &shadow_buffers[my_thread_param.thread_id*max_elem_num+i],
               &buffers[i], sizeof(struct buf_elem) );
    }
  }
  if (enable_random_sleep_in_thread) {
     my_sleep_secs = next_rnd_num() & 0x0f;
     if (verbose) printf( "Thread %d: sleep for %d seconds.\n", 
                           my_thread_param.thread_id, my_sleep_secs );
     memset( &timeout, 0, sizeof(timeout) );
     timeout.tv_usec = 0;
     timeout.tv_sec = my_sleep_secs;
     select(0,NULL,NULL,NULL,&timeout);
  }

/* Get memory from system */
  if (user_break_exit) goto free_all;
  if (verbose) printf( "Thread %d: Allocate memory buffers.\n", my_thread_param.thread_id );
  
  for(i=0; i<max_elem_num; i++) {
    buffers[i].w_pbuf = malloc(buffers[i].msize);
     if (buffers[i].w_pbuf == NULL) {
       thread_stat.other_errors++;
       fprintf( stderr, "Thread %d: ERR [malloc]: %s for buffer %d\n", 
                         my_thread_param.thread_id,strerror(errno),i );
     }
     else {
       if (verbose) {
          if (my_thread_param.rw_mult_bufs) printf( "Thread %d: Init write_buffer %d\n",
                                                    my_thread_param.thread_id,i );     
          else printf( "Thread %d: Init read_write_buffer %d\n", 
                        my_thread_param.thread_id,i );     
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
       else
         memset( buffers[i].w_pbuf, i, buffers[i].msize );
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
    buffers[i].r_pbuf = malloc(buffers[i].msize);
     if (buffers[i].r_pbuf == NULL) {
       thread_stat.other_errors++;
       fprintf( stderr, "Thread %d: ERR [malloc]: %s for buffer %d\n", 
                         my_thread_param.thread_id,strerror(errno),i );
     }
     else {
       if (shadow_buffers_copy)
          shadow_buffers[my_thread_param.thread_id*max_elem_num+i].r_pbuf = 
	                                                    buffers[i].r_pbuf;
       if (verbose) printf( "Thread %d: Init read_buffer %d\n",
                             my_thread_param.thread_id,i );     
       if (my_thread_param.fill_rand) {
         init_rnd_gen(i);
         len = buffers[i].msize;
         p = buffers[i].r_pbuf;
         while( len--) {
           *p++ = next_rnd_num();
         }
       }
       else
         memset( buffers[i].r_pbuf, i, buffers[i].msize );
     }
   }
  }

/* Create file interface */
  if (user_break_exit) goto free_all;
  if (verbose)
    printf( "Thread %d: Create files. %u files, each size = %u Mbytes\n", 
                        my_thread_param.thread_id,max_elem_num, max_file_size );

  for(i=0; i<max_elem_num; i++) { 
     if (buffers[i].w_pbuf != NULL) {
       if (out_dir != NULL) strcpy( buffers[i].filename, out_dir );       
       else                 strcpy( buffers[i].filename, work_dir );
       strcat( buffers[i].filename, dir_sep_str );
       sprintf( num_space, "%d_%d_", cur_pid, my_thread_param.thread_id );
       strcat( buffers[i].filename, num_space );
       sprintf( temp_space, "%d", i );
       strcat( buffers[i].filename, temp_space );
       strcat( buffers[i].filename,".DUMMY" );
       if (shadow_buffers_copy) {     	      
          memcpy(shadow_buffers[my_thread_param.thread_id*max_elem_num+i].filename,
                 buffers[i].filename,
		 sizeof(buffers[i].filename) );
       }		 
       buffers[i].fdes = open( buffers[i].filename, my_thread_param.flags, 
                               my_thread_param.fmode );
       if (buffers[i].fdes == -1) {
         thread_stat.other_errors++;
         fprintf( stderr, "Thread %d: ERR [open]: %s for file %s\n",  
              my_thread_param.thread_id,strerror(errno),buffers[i].filename );
       }
       if (shadow_buffers_copy)      	      
          shadow_buffers[my_thread_param.thread_id*max_elem_num+i].fdes = 
	                                                    buffers[i].fdes;
     }
  } /*for*/


/* Write test */
  if (user_break_exit) goto free_all;
  if (verbose) printf( "Thread %d: Write files from memory buffers.\n",
                       my_thread_param.thread_id );

  for(i=0; i<max_elem_num; i++) {  
    if ((buffers[i].w_pbuf != NULL) && (buffers[i].fdes != -1)) {
      filesize = buffers[i].fsize;
      if (lseek( buffers[i].fdes, SEEK_SET, 0 ) == -1) {
        thread_stat.other_errors++;
        fprintf( stderr, "Thread %d: ERR [seek/w]: %s for file %d\n",
                          my_thread_param.thread_id,strerror(errno), i );
        continue;
      }      
      chksum = 0;
      if (verbose) printf( "Thread %d: Write file %d: ",my_thread_param.thread_id,i );      
      else         printf( "w" );
      while( filesize ) {
        mult_rw_op = '*';
        if ( filesize < buffers[i].msize) {
            this_len = (unsigned long int)filesize;
            mult_rw_op = '.';
        }
        else 
            this_len = buffers[i].msize;
        p = buffers[i].w_pbuf;
        len = this_len;
        while( len--) { 
          chksum += *p++;
        }
        printf( "%c", mult_rw_op );
        if (use_alarm) alarm(io_alarm_secs);
        len = write( buffers[i].fdes, buffers[i].w_pbuf, this_len );
        thread_stat.write_bytes += len;
        if (use_alarm) alarm(0);
        if (len != this_len) {
          thread_stat.other_errors++;
          fprintf( stderr, "\nThread %d: ERR [write]: %s for buffer %d\n", 
                            my_thread_param.thread_id,strerror(errno), i );
          break;
        }
        if (my_thread_param.sync_flag) 
	  sync_file_changes(buffers[i].fdes);
        filesize -= this_len;
      }
      buffers[i].chksum = chksum;
      if (verbose) 
        printf( "\nThread %d: Additive sum %lu for file %d\n", 
                 my_thread_param.thread_id,buffers[i].chksum, i ); 
    }
  } /*for*/


/* Read test */
  if (user_break_exit) goto free_all;
  if (verbose) printf( "Thread %d: Read files to memory buffers.\n",
                        my_thread_param.thread_id );

  for(i=0; i<max_elem_num; i++) {  
    if ((buffers[i].r_pbuf != NULL) && (buffers[i].fdes != -1)) {
      if (lseek( buffers[i].fdes, SEEK_SET, 0 ) == -1) {
        thread_stat.other_errors++;
        fprintf( stderr, "Thread %d: ERR [seek/r]: %s for file %d\n", 
                          my_thread_param.thread_id,strerror(errno), i );
        continue;
      }
      chksum = 0;      
      filesize = buffers[i].fsize;
      if (verbose) printf( "Thread %d: Read file %d: ",my_thread_param.thread_id,i );      
      else         printf( "r" );
      while( filesize ) {
        mult_rw_op = '*';      
        if ( filesize < buffers[i].msize) {
            this_len = (unsigned long int)filesize;
            mult_rw_op = '.';
        }
        else
            this_len = buffers[i].msize;
        printf( "%c", mult_rw_op );    
        if (use_alarm) alarm(io_alarm_secs);        
        len = read( buffers[i].fdes, buffers[i].r_pbuf, this_len );
        thread_stat.read_bytes += len;
        if (use_alarm) alarm(0);
        if (len != this_len) {
          thread_stat.other_errors++;
          fprintf( stderr, "\nThread %d: ERR [read]: %s for buffer %d\n", 
                             my_thread_param.thread_id,strerror(errno), i );
          break;
        }
        p = buffers[i].r_pbuf;
        len = this_len;
        while( len--) {
          chksum += *p++;        
        }
        filesize -= this_len;
      }
      if (verbose) printf( "\nThread %d: Additive sum %lu for file %d\n", 
                           my_thread_param.thread_id,chksum, i );
      if (buffers[i].chksum != chksum)  {
        thread_stat.compare_errors++;
        if (verbose) printf( "ERROR on chksum!!!\n" );
        fprintf( stderr, "\nThread %d: ERR [chksum]: mismatch additive sums, write=%lu, read=%lu\n",
                          my_thread_param.thread_id,buffers[i].chksum, chksum );
      }
    }
  }

/* Look temporary results */  
#if 0
  sleep(15);
#endif

free_all:
/* Close file interface */  
  if (verbose) printf( "Thread %d: Close and delete files.\n",my_thread_param.thread_id );
  close_files(buffers);    

/* Return memory to system */
  if (verbose) printf( "Thread %d: Free memory buffers.\n",my_thread_param.thread_id );
  free_buffers(buffers);

  if (shadow_buffers_copy) {
    for(i=0; i<max_elem_num; i++) {
       memset( &shadow_buffers[my_thread_param.thread_id*max_elem_num+i],
               0, sizeof(struct buf_elem) );
    }	       
  }
  
/* Check condition to go */    
  if (LogFile != NULL) {
    snprintf( log_buf, sizeof(log_buf), "Thread %d: Finished pass %lu.\n", 
                                        my_thread_param.thread_id,pass_num+1 );
    write_log( LogFile, log_buf );
  }
  pass_num++;
  if (verbose) printf( "Thread %d: Pass %lu finished.\n", 
                        my_thread_param.thread_id,pass_num );
  
  if (user_break_exit) { 
     if (verbose) {
         printf( "Thread %d: (process %d) terminated by user break.\n", 
                  my_thread_param.thread_id, cur_pid );
     }
     goto done;
  }
  if (my_thread_param.loop_forever) goto forever;  
  if (--passes_count) goto forever;

done:
  if (verbose) {
       printf( "Thread %d: (process %u) terminated.\n", 
               my_thread_param.thread_id, cur_pid );
  }

#if _WIN32  
  SetEvent( g_hEventThreadsObj[my_thread_param.thread_id]);
#endif

  if (collect_summary_statistics) {
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
void prime_numbers( unsigned long int max_int_num )
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
void usage(void)
{
  fprintf( stderr, "\n" );
  fprintf( stderr, "File I/O testing program (with pthreads), version %s\n", prog_ver );
  fprintf( stderr, "Copyright (C) 2006-2007 Dmitry Stefankov. All rights reserved.\n" );
  fprintf( stderr, "Usage: ftest2 [-AdfhlMmOrSRuyvz] [-n filecount] [-s bufsize] [-f filesize]\n" );
  fprintf( stderr, "       [-p passes] [-t workdir] [-a io_tmout_secs] [-L logfile] [-j threads]\n" );
  fprintf( stderr, "       [-w usec] [-P max_int_num] [-o actv_mon_tmout]\n" );
  fprintf( stderr, "       -A   use alarm control for r/w ops (default=no)\n" );    
  fprintf( stderr, "       -d   direct writing (no caching)\n" );    
  fprintf( stderr, "       -h   this help\n" );      
  fprintf( stderr, "       -l   run forever\n" );      
  fprintf( stderr, "       -m   separate buffers for read and write\n" );      
  fprintf( stderr, "       -r   fill buffers with random values\n" );      
  fprintf( stderr, "       -y   synchronous writes\n" );      
  fprintf( stderr, "       -v   verbose output\n" );      
  fprintf( stderr, "       -z   don't delete temporary files\n" );        
  fprintf( stderr, "       -M   disable summary statistics collection (default=enable)\n" );
  fprintf( stderr, "       -j   number of threads\n" );
  fprintf( stderr, "       -w   wait microsec before run each thread (default=0)\n" );
  fprintf( stderr, "       -u   disable async. user break signal (default=enable)\n" );
  fprintf( stderr, "       -P   max.int.num to search prime numbers (def.=0, rec.=10000+)\n" );
  fprintf( stderr, "       -O   use active monitoring of threads state\n" );
  fprintf( stderr, "       -o   active monitoring time interval (default=none)\n" );
  fprintf( stderr, "       -R   re-run terminated threads (default=no)\n" );
  fprintf( stderr, "       -S   enable to sleep N random seconds in each thread (default=no)\n" ); 
  fprintf( stderr, "Default parameters:\n" );
  fprintf( stderr, "   Passes number:    min=1, def=%d, max=unlimited\n", DEF_PASS_COUNT );    
  fprintf( stderr, "   Files/buffers:    min=1, def=%d, max=%d\n", DEF_NUM_ELEMS, MAX_NUM_ELEMS );  
  fprintf( stderr, "   Buffer size:      min=1 MB, def=%d MB, max=unlimited\n", DEF_BUF_SIZE );
  fprintf( stderr, "   File   size:      min=1 MB, def=%d MB, max=unlimited\n", DEF_FILE_SIZE );
  fprintf( stderr, "   Threads number:   min=1, def=%d, max=%d\n", DEF_NUM_THREADS, MAX_NUM_THREADS );
  fprintf( stderr, "   Working catalog:  %s\n", work_dir );
  fprintf( stderr, "   I/O op. timeout:  %d seconds\n", ALARM_SECS );
  fprintf( stderr, "Screen output symbols:\n" );
  fprintf( stderr, "   w=writing, r=reading, *=full buffer, .=partial buffer" );      
  fprintf( stderr, "\n" );    
  fprintf( stderr, "Sample command line:\n" );  
  fprintf( stderr, "   ./ftest2 -vluSRO -j 2 -s 100 -f 6500 -t /test_dir -o 60" );
  fprintf( stderr, "   ./ftest2 -vlr -s 400 -f 1600 -t /media1" );
  fprintf( stderr, "   ./ftest2 -vlr -s 500" );
  fprintf( stderr, "\n" );    
  exit(1);
}


/*
 *  Run Posix thread (pthread) or Windows thread (Win32)
 */
int run_thread(int thread_no)
{
#ifdef _WIN32  
    HANDLE              hThread;
#endif
    int                 result_code;
    int                 j;
    void              * arg;
    void              * p;
    struct timeval      timeout;
    struct thread_param worker_param;      
  
    j = thread_no;
    memset( &worker_param, 0, sizeof(worker_param) );
    worker_param.thread_id = j;
    worker_param.max_file_size = max_file_size;
    worker_param.passes_count = passes_count;
    worker_param.loop_forever = loop_forever;
    worker_param.fill_rand = fill_rand;
    worker_param.rw_mult_bufs = rw_mult_bufs;
    worker_param.sync_flag = sync_flag;
    worker_param.flags = flags;
    worker_param.fmode = fmode;
       
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
           printf( "thread %d: default thread stacksize = %d\n", 
                    j, default_stack_size);   
       }
       if (default_stack_size < MIN_REQ_SSIZE) {
          pthread_attr_setstacksize(&stack_size_custom_attr[j],(size_t)MIN_REQ_SSIZE);    
          pthread_attr_getstacksize(&stack_size_custom_attr[j],&new_stack_size);    
          if (verbose) {
              printf( "thread %d: new thread stacksize = %d\n",  
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

/* Initialize */
  memset( (void *)&summary_stat, 0, sizeof(summary_stat) );
  
/* Process command line  */  
  opterr = 0;
  while( (op = getopt(argc,argv,"a:Adhf:j:L:lMmn:p:P:Oo:rRs:St:uvw:yz")) != -1)
    switch(op) {
      case 'A':
               use_alarm = 1;
               break; 
      case 'a':
               io_alarm_secs = (unsigned int)atol(optarg);
               if (io_alarm_secs == 0) io_alarm_secs = ALARM_SECS;
      	       break;    
      case 't': 
               out_dir = optarg;
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
      case 'n':
                max_elem_num = atoi(optarg);
                if (max_elem_num > MAX_NUM_ELEMS) max_elem_num = MAX_NUM_ELEMS;
                if (max_elem_num == 0) max_elem_num = 1;
                break;
      case 'p':
                passes_count = atol(optarg);
                if (passes_count == 0) passes_count = 1; 
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
      case 'v':
               verbose = 1;
      	       break;       
      case 'w':
               us_wait_to_run_thread = atol(optarg);
               break;	       
      case 'z':
               delete_flag = 0;
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
               sync_flag = 1;
               break;      	       
      case 'P':
               max_int_num = atol(optarg);
               if (max_int_num > MAX_PRIMES_NUM) max_int_num = MAX_PRIMES_NUM;
               break;         
      case 'h':
               usage();
               break;   
      default:
               break;
    }

   /* Check some settings */
   if (threads_active_control && (threads_actv_ctl_timeout == -1)) {
       fprintf( stderr, "You must set threads monitoring time interval!\n" );
       return(7);
   }
    
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

         dwRes = WaitForSingleObject( g_hEventThreadsObj[j], dwDefThreadWaitTime );

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
  
  /* Output summary results */
  if (collect_summary_statistics) {
      printf( "\nSummary statistics:\n" );
#if FILE_SIZE_MORE_4G  
      printf( "Read_Bytes=%llu", summary_stat.read_bytes );
      printf( "   " );
      printf( "Write_Bytes=%llu\n", summary_stat.write_bytes );
#else
      printf( "Read_Bytes=%lu   Write_Bytes=%lu\n",      
              summary_stat.read_bytes, summary_stat.write_bytes );
#endif
      printf( "Compare_Errors=%lu   Other_Errors=%lu\n",
              summary_stat.compare_errors, summary_stat.other_errors );
  }

  return(0);
}
