/*****************************************************************************
 *                              File ftest.c                                 
 *
 *                   Simple I/O test bechmark program 
 *
 *      Copyright (c) Dmitry V. Stefankov, 2000-2006. All rights reserved.
 *
 *****************************************************************************/
/*
 *   $Source: /root/projects/tests/RCS/ftest.c,v $
 *  $RCSfile: ftest.c,v $
 *   $Author: dstef $
 *     $Date: 2006-03-23 13:13:13+03 $
 * $Revision: 1.13 $
 *   $Locker: root $
 *
 *      $Log: ftest.c,v $
 *      Revision 1.13  2006-03-23 13:13:13+03  dstef
 *      Added -z switch (don't delete temporary files)
 *
 *      Revision 1.12  2006-02-17 18:58:39+03  dstef
 *      Added suport for Sun platforms
 *
 *      Revision 1.11  2003-10-10 15:13:53+04  dstef
 *      Fixed compile problems for Linux
 *
 *      Revision 1.10  2003-10-10 14:17:34+04  dstef
 *      Fixed bug with != for options
 *      Fixed Linux compile problem for time structures
 *      Removed unused variables
 *
 *      Revision 1.9  2003-07-01 14:01:12+04  dstef
 *      Added logfile
 *      Added write_message for signal handler
 *
 *      Revision 1.8  2003-02-04 02:28:43+03  dstef
 *      Added Win32 thread to emulate I/O alarm option
 *
 *      Revision 1.7  2003-01-22 19:58:59+03  dstef
 *      Minor changes
 *
 *      Revision 1.6  2003/01/22 16:56:47  root
 *      Minor changes
 *
 *      Revision 1.5  2003/01/22 16:51:19  root
 *      Some clean-up code (OS-dependent)
 *      Fixed bug about pass number message
 *
 *      Revision 1.4  2002-12-09 16:55:01+03  dstef
 *      Fixed some bugs for MSVC6 (missing pmode parameter for open and more)
 *
 *      Revision 1.3  2002-12-09 15:50:01+03  dstef
 *      Added more support for VC6 and Mingw32 compilers (Windows version)
 *
 *      Revision 1.2  2002-12-09 05:19:52+03  dstef
 *      Ported to Microsoft Windows platform using Mingw32 GNU C v2.95
 *
 *      Revision 1.1  2002-09-06 23:53:23+04  dstef
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
#else
#include  <io.h>
#endif

#if _WIN32
#include "getopt.h"
#endif


/* Definitions (OS-dependent) */

#if _WIN32

#ifndef _commit
int __cdecl _commit(int);
#endif

#ifndef snprintf
#define snprintf _snprintf
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

#define  MAX_NUM_ELEMS  100
#define  DEF_NUM_ELEMS  4
#define  DEF_BUF_SIZE   4
#define  DEF_FILE_SIZE  16
#define  FILENAME_SIZE	255
#define  DIRNAME_SIZE	FILENAME_SIZE

#define  DEF_PASS_COUNT 1

#define  ALARM_SECS	3600
#define  MS_PER_SEC     1000

#define  KILOBYTES  1024
#define  MEGABYTES  (KILOBYTES*1024)

/* Data structures */

struct buf_elem {
   unsigned long int msize;            
   unsigned long int fsize;                 
   void * r_pbuf;                             
   void * w_pbuf;   
   int  fdes;
   char filename[FILENAME_SIZE];
   unsigned long int chksum;   
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

#if _WIN32
const char    dir_sep_str[] = "\\";
char          work_dir[DIRNAME_SIZE] = "c:\\temp";
HANDLE        g_hEventObj = NULL;
HANDLE        g_hWaitingThread = NULL;
DWORD         g_dwThreadId = 0;
DWORD         g_dwThreadArg = 0;
#else
const char    dir_sep_str[] = "/";
char  work_dir[DIRNAME_SIZE] = "/tmp";
#endif

int           delete_flag = 1;
int           verbose = 0;
unsigned int  io_alarm_secs = ALARM_SECS;

const char prog_ver[] = "1.11";
const char rcs_id[] = "$Id: ftest.c,v 1.13 2006-03-23 13:13:13+03 dstef Exp root $";

struct buf_elem  buffers[MAX_NUM_ELEMS];


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
void close_files( void )
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
void free_buffers( void )
{
   int  i;

  for(i=0; i<max_elem_num; i++) {  
    if (buffers[i].w_pbuf != buffers[i].r_pbuf) 
      free(buffers[i].r_pbuf);
    free(buffers[i].w_pbuf);
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
   close_files();
   free_buffers();   
   exit(1);
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
 *  Print help screen
 */
void usage(void)
{
  fprintf( stderr, "\n" );
  fprintf( stderr, "File I/O testing program, version %s\n", prog_ver );
  fprintf( stderr, "Copyright (C) 2002-2006 Dmitry Stefankov. All rights reserved.\n" );
  fprintf( stderr, "Usage: ftest [-dfhlmryvz] [-n filecount] [-s bufsize] [-f filesize]\n" );
  fprintf( stderr, "       [-p passes] [-t workdir] [-a io_tmout_secs] [-L logfile]\n" );
  fprintf( stderr, "       -d   direct writing (no caching)\n" );    
  fprintf( stderr, "       -h   this help\n" );      
  fprintf( stderr, "       -l   run forever\n" );      
  fprintf( stderr, "       -m   separate buffers for read and write\n" );      
  fprintf( stderr, "       -r   fill buffers with random values\n" );      
  fprintf( stderr, "       -y   synchronous writes\n" );      
  fprintf( stderr, "       -v   verbose output\n" );      
  fprintf( stderr, "       -z   don't delete temporary files\n" );        
  fprintf( stderr, "Default parameters:\n" );
  fprintf( stderr, "   Passes number:    min=1, def=%d, max=unlimited\n", DEF_PASS_COUNT );    
  fprintf( stderr, "   Files/buffers:    min=1, def=%d, max=%d\n", DEF_NUM_ELEMS, MAX_NUM_ELEMS );  
  fprintf( stderr, "   Buffer size:      min=1 MB, def=%d MB, max=unlimited\n", DEF_BUF_SIZE );
  fprintf( stderr, "   File   size:      min=1 MB, def=%d MB, max=unlimited\n", DEF_FILE_SIZE );
  fprintf( stderr, "   Working catalog:  %s\n", work_dir );
  fprintf( stderr, "   I/O op. timeout:  %d seconds\n", ALARM_SECS );
  fprintf( stderr, "Screen output symbols:\n" );
  fprintf( stderr, "   w=writing, r=reading, *=full buffer, .=partial buffer" );      
  fprintf( stderr, "\n" );    
  exit(1);
}

/*
 *  Main program stream
 */
int main( int argc, char ** argv )
{
  int                 op;
  int                 sync_flag = 0;
  int                 i;
  char                mult_rw_op;
#if _WIN32  
  int                 flags = O_RDWR|O_CREAT|O_TRUNC|O_BINARY;
  #if _MSVC
    int    fmode = _S_IREAD | _S_IWRITE;
  #else
    mode_t fmode = _S_IRUSR|_S_IWUSR;
  #endif
#else
  int                 flags = O_RDWR|O_CREAT|O_TRUNC;
  mode_t              fmode = S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH;
  sigset_t            mask;
  struct sigaction    sa;
#endif
  char                temp_space[20];
  char              * LogFile = NULL;
  char                log_buf[256];
  unsigned long int   len;
  unsigned long int   filesize;
  unsigned long int   this_len;
  unsigned long int   passes_count = DEF_PASS_COUNT;
  unsigned long int   pass_num = 0;
  int                 fill_rand = 0;
  unsigned long int   chksum;
  unsigned char *     p;
  char *              out_dir = NULL;
  int                 loop_forever = 0;
  int                 rw_mult_bufs = 0;

/* Process command line  */  
  opterr = 0;
  while( (op = getopt(argc,argv,"a:dhf:L:lmn:p:rs:t:yvz")) != -1)
    switch(op) {
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
               max_file_size = atoi(optarg);
               if (max_file_size == 0) max_file_size = 1; 
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
      case 'v':
               verbose = 1;
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
      case 'h':
               usage();
               break;   
      default:
               break;
    }

#if _WIN32
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

forever:
/* Initialize */
  if (LogFile != NULL) {
    snprintf( log_buf, sizeof(log_buf), "Started pass %lu.\n", pass_num+1 );
    write_log( LogFile, log_buf );
  }
  if (verbose)
    printf( "Init structures. %u buffers, each size %u MBytes\n",
                              max_elem_num, max_buf_size);
    
  for(i=0; i<max_elem_num; i++) {
    buffers[i].msize = max_buf_size * MEGABYTES;
    buffers[i].fsize = max_file_size * MEGABYTES;
    buffers[i].r_pbuf = NULL;
    buffers[i].w_pbuf = NULL;    
    buffers[i].fdes = -1;
    buffers[i].chksum = -1;
    memset( buffers[i].filename,0,sizeof(buffers[i].filename) );
  }

/* Get memory from system */
  if (verbose) printf( "Allocate memory buffers.\n" );
  
  for(i=0; i<max_elem_num; i++) {
    buffers[i].w_pbuf = malloc(buffers[i].msize);
     if (buffers[i].w_pbuf == NULL)
       fprintf( stderr, "ERR [malloc]: %s for buffer %d\n", strerror(errno),i );
     else {
       if (verbose) {
          if (rw_mult_bufs) printf( "Init write_buffer %d\n",i );     
          else              printf( "Init read_write_buffer %d\n",i );     
       }
       if (fill_rand) {
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
     if (!rw_mult_bufs) buffers[i].r_pbuf = buffers[i].w_pbuf;
  }

  if (rw_mult_bufs) {
   for(i=0; i<max_elem_num; i++) {
    buffers[i].r_pbuf = malloc(buffers[i].msize);
     if (buffers[i].r_pbuf == NULL)
       fprintf( stderr, "ERR [malloc]: %s for buffer %d\n", strerror(errno),i );
     else {
       if (verbose) printf( "Init read_buffer %d\n",i );     
       if (fill_rand) {
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
  if (verbose)
    printf( "Create files. %u files, each size = %u Mbytes\n", 
                           max_elem_num, max_file_size );

  for(i=0; i<max_elem_num; i++) { 
     if (buffers[i].w_pbuf != NULL) {
       if (out_dir != NULL) strcpy( buffers[i].filename, out_dir );       
       else                 strcpy( buffers[i].filename, work_dir );
       strcat( buffers[i].filename, dir_sep_str );
       strcat( buffers[i].filename,"bigfile" );
       sprintf( temp_space, "%d", i );
       strcat( buffers[i].filename, temp_space );
       strcat( buffers[i].filename,".DUMMY" );
       buffers[i].fdes = open( buffers[i].filename, flags, fmode );
       if (buffers[i].fdes == -1) 
         fprintf( stderr, "ERR [open]: %s for file %s\n", strerror(errno), 
                           buffers[i].filename );
     }
  }


/* Write test */
  if (verbose) printf( "Write files from memory buffers.\n" );

  for(i=0; i<max_elem_num; i++) {  
    if ((buffers[i].w_pbuf != NULL) && (buffers[i].fdes != -1)) {
      filesize = buffers[i].fsize;
      if (lseek( buffers[i].fdes, SEEK_SET, 0 ) == -1) {
        fprintf( stderr, "ERR [seek/w]: %s for file %d\n", strerror(errno), i );
        continue;
      }      
      chksum = 0;
      if (verbose) printf( "Write file %d: ",i );      
      else         printf( "w" );
      while( filesize ) {
        mult_rw_op = '*';
        if ( filesize < buffers[i].msize) {
            this_len = filesize;
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
        alarm(io_alarm_secs);
        len = write( buffers[i].fdes, buffers[i].w_pbuf, this_len );
        alarm(0);
        if (len != this_len) {
          fprintf( stderr, "\nERR [write]: %s for buffer %d\n", 
                            strerror(errno), i );
          break;
        }
        if (sync_flag) 
	  sync_file_changes(buffers[i].fdes);
        filesize -= this_len;
      }
      buffers[i].chksum = chksum;
      if (verbose) 
        printf( "\nAdditive sum %lu for file %d\n", buffers[i].chksum, i ); 
    }
  }


/* Read test */
  if (verbose) printf( "Read files to memory buffers.\n" );

  for(i=0; i<max_elem_num; i++) {  
    if ((buffers[i].r_pbuf != NULL) && (buffers[i].fdes != -1)) {
      if (lseek( buffers[i].fdes, SEEK_SET, 0 ) == -1) {
        fprintf( stderr, "ERR [seek/r]: %s for file %d\n", strerror(errno), i );
        continue;
      }
      chksum = 0;      
      filesize = buffers[i].fsize;
      if (verbose) printf( "Read file %d: ",i );      
      else         printf( "r" );
      while( filesize ) {
        mult_rw_op = '*';      
        if ( filesize < buffers[i].msize) {
            this_len = filesize;
            mult_rw_op = '.';
        }
        else
            this_len = buffers[i].msize;
        printf( "%c", mult_rw_op );    
        alarm(io_alarm_secs);        
        len = read( buffers[i].fdes, buffers[i].r_pbuf, this_len );
        alarm(0);
        if (len != this_len) {
          fprintf( stderr, "\nERR [read]: %s for buffer %d\n", strerror(errno), i );
          break;
        }
        p = buffers[i].r_pbuf;
        len = this_len;
        while( len--) {
          chksum += *p++;        
        }
        filesize -= this_len;
      }
      if (verbose) printf( "\nAdditive sum %lu for file %d\n", chksum, i );
      if (buffers[i].chksum != chksum)  {
        if (verbose) printf( "ERROR on chksum!!!\n" );
        fprintf( stderr, "\nERR [chksum]: mismatch additive sums, write=%lu, read=%lu\n",
                          buffers[i].chksum, chksum );
      }
    }
  }

/* Look temporary results */  
#if 0
  sleep(15);
#endif

/* Close file interface */  
  if (verbose) printf( "Close and delete files.\n" );
  close_files();    

/* Return memory to system */
  if (verbose) printf( "Free memory buffers.\n" );
  free_buffers();

/* Check condition to go */    
  if (LogFile != NULL) {
    snprintf( log_buf, sizeof(log_buf), "Finished pass %lu.\n", pass_num+1 );
    write_log( LogFile, log_buf );
  }
  pass_num++;
  if (verbose) printf( "Pass %lu finished.\n", pass_num );
  
  if (loop_forever) goto forever;  
  if (--passes_count) goto forever;
  
  return(0);
}
