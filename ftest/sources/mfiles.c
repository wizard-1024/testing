/*****************************************************************************
 *                              File mfiles.c                                 
 *
 *                   Very complex I/O test bechmark program 
 *
 *      Copyright (c) Dmitry V. Stefankov, 2005. All rights reserved.
 *
 *****************************************************************************/
/*
 *   $Source: /root/projects/tests/RCS/mfiles.c,v $
 *  $RCSfile: mfiles.c,v $
 *   $Author: dstef $
 *     $Date: 2005-07-25 12:55:58+04 $
 * $Revision: 1.5 $
 *   $Locker: root $
 *
 *      $Log: mfiles.c,v $
 *      Revision 1.5  2005-07-25 12:55:58+04  dstef
 *      Added 2-second pause after read, before read
 *
 *      Revision 1.4  2005-07-21 20:50:50+04  dstef
 *      Added options for wait interval
 *
 *      Revision 1.3  2005-07-19 15:57:21+04  dstef
 *      Added read-back from file
 *      Added checksums comparison on write-read
 *
 *      Revision 1.2  2005-07-18 19:04:13+04  dstef
 *      Help updated
 *
 *      Revision 1.1  2005-07-18 19:02:40+04  dstef
 *      Initial revision
 *
 ****************************************************************************/



#include  <unistd.h>
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
#include  <sys/time.h>
#include  <sys/resource.h>
#include  <pthread.h>


/* Definitions (OS-dependent) */




#define SIGNAL(s,handler)	{\
	sa.sa_handler = handler; \
	if (sigaction(s,&sa,NULL) < 0) { \
	  fprintf( stderr, "Couldn't establish signal handler (%d): %m", s); \
	} \
   }



/* Definitions */

#define  DEF_FILE_SIZE  32
#define  FILENAME_SIZE	255
#define  DIRNAME_SIZE	FILENAME_SIZE

#define  MIN_REQ_SSIZE      (128*1024)
#define  MAX_WORK_THREADS   500
#define  DEF_WORK_THREADS   4
#define  DEF_WAIT_TIME      5
#define  BUF_SIZE           4096
#define  WR_WAIT_DELAY      2

#define  DEF_PASS_COUNT 1

#define  ALARM_SECS	3600
#define  MS_PER_SEC     1000

#define  KILOBYTES  1024
#define  MEGABYTES  (KILOBYTES*1024)

/* Data structures */

struct  thread_param
{
   int  thread_id;
   int  max_file_size;
   int  max_sleep_time_before;
   int  max_sleep_time_after;
   int  passes_count;
   int  loop_forever;
   int  fill_rand;
};


/*------------------------------- GNU C library -----------------------------*/


/* Local data */

extern  int        optind;
extern  int        opterr;
extern  char     * optarg;

int  max_work_threads = DEF_WORK_THREADS;
int  max_file_size = DEF_FILE_SIZE;

const char    dir_sep_str[] = "/";
char  work_dir[DIRNAME_SIZE] = "/tmp";
char *              out_dir = NULL;

int           verbose = 0;
unsigned int  io_alarm_secs = ALARM_SECS;

const char prog_ver[] = "1.0";
const char rcs_id[] = "$Id: mfiles.c,v 1.5 2005-07-25 12:55:58+04 dstef Exp root $";



/*----------------------- Functions ---------------------------------------*/


/*
 * Print standard message from signal handler
 */
void print_message_signal( char * msg )
{
   if (msg == NULL) return;
   else write( STDIN_FILENO, msg, strlen(msg) );
}


/*
 * Print error message from signal handler
 */
void print_error_message_signal( char * msg )
{
   if (msg == NULL) return;

   write( STDERR_FILENO, msg, strlen(msg) );
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
    srandom( time(NULL)+ofs );
}

/*
 * Get next random number
 */
unsigned char next_rnd_num(void)
{
  unsigned char rn;

  rn = random() & 0xFF;
  return(rn);
}


    
/*
 *  Terminate program
 */
static void term(int sig)
{
   char  msg_buf[128];
  
   snprintf( msg_buf, sizeof(msg_buf), "\nsignal() %d\n", sig );
   print_message_signal( msg_buf );
   exit(1);
}


/* 
 *   Wait seconds 
 */
int wait_nseconds(long int nsecs)
{
  int res = -1;
  
  struct timeval  wait_tmout;
  wait_tmout.tv_sec = nsecs;
  wait_tmout.tv_usec = 0;

  if (nsecs == 0) return(res);
    
  res = select( 0, NULL, NULL, NULL, &wait_tmout );
  
  return(res);
}


/*
 *  Create temporary file (unique for this catalog)
 */
int  create_unique_file( char * template, char * result_file_name_buf, 
                         int file_buf_len, int thread_id  )
{
    int  fd = -1;
    int  save_errno;
    char temp_file_name[256];
    
    if (template == NULL) {
      printf( "ERROR: create_uniq_file: bad template!\n" );
      return(-2);
    }

    if ((result_file_name_buf == NULL) || (file_buf_len == 0)) {
      printf( "ERROR: create_uniq_file: bad file_name_buf!!\n" );
      return(-3);
    }
    
    /* Make template for filename */
    strncpy( temp_file_name, template, sizeof(temp_file_name));

    /* open a file to store this message */
    fd = mkstemp(temp_file_name);
    save_errno = errno;
    if (fd < 0) {
	printf( "ERROR: %d: create_uniq_file: cannot create temporary file (%s)!\n", 
		thread_id, strerror(save_errno) );
	return(-4);
    }

    if (verbose) {
        printf( "%d: create_uniq_file: unique_file_name = %s\n", 
	        thread_id, temp_file_name );
        printf( "%d: create_uniq_file: fd=%d\n", thread_id, fd );
    }
    
    strncpy( result_file_name_buf, temp_file_name, (size_t)file_buf_len );

    return(fd);
}


/*
 *  Main work thread
 */
void * worker_thread( void * arg )
{
    struct thread_param  * p_thread_param;
    int    res;
    int    wait_time;
    int    new_fd;
    int    filesize;
    int    len;
    int    fill_len;
    int    result;
    int    count;
    int    passes_count = DEF_PASS_COUNT;
    int    pass_num = 1;
    unsigned char * p;
    unsigned long int r_chksum;
    unsigned long int w_chksum;
    FILE * fp;
    char   template[FILENAME_SIZE];
    char   filename[FILENAME_SIZE];
    char   buffer[BUF_SIZE];

    if (arg == NULL) {
        printf( "ERROR: cannot find thread parameters!\n" );
        return( NULL);
    }
        
    p_thread_param = (struct thread_param *)arg;
    if (verbose) {
       printf( "%d: started\n", p_thread_param->thread_id );
    }
    passes_count = p_thread_param->passes_count;

forever:
    /* Wait before write */
    wait_time = next_rnd_num() % (p_thread_param->max_sleep_time_before+1);
    if (verbose) printf( "%d: wait_before %d sec(s)\n", 
                         p_thread_param->thread_id, wait_time );
    res = wait_nseconds(wait_time);

    /* Write file */
    memset( template, 0, sizeof(template) );
    if (out_dir == NULL) strncpy( template, work_dir, sizeof(template)-1 );
    else strncpy( template, out_dir, sizeof(template)-1 );
    strncat( template, "/file.XXXXXXXX", sizeof(template)-1 );
    new_fd = create_unique_file( template, filename, sizeof(filename),
                                 p_thread_param->thread_id );
    if (new_fd < 0) {
       printf( "error!\n" );
    }                             
    res = close(new_fd);                             
    filesize = next_rnd_num() % (p_thread_param->max_file_size+1);
    if (verbose) printf( "%d: file_size = %d KB\n",
                         p_thread_param->thread_id, filesize );
    fp = fopen( filename, "wb" );
    if (fp == NULL) {
       printf( "%d: ERROR: cannot open file %s!\n",
               p_thread_param->thread_id, filename );
    }
    else {
        len = filesize * KILOBYTES;
	w_chksum = 0;
        while( len ) {
             if (len > sizeof(buffer)) count = sizeof(buffer);
             else count = len;
             if (p_thread_param->fill_rand) {
                 fill_len = count;
                 p = buffer;
                 while( fill_len--) *p++ = next_rnd_num();
             }
             else 
                 memset( buffer, p_thread_param->thread_id, count );
	     fill_len = count;
	     p = buffer;
	     while( fill_len--)	w_chksum += *p++;
             result = fwrite( buffer, 1, count, fp );
             if (result == count) {
                 len -= result;
                 if (verbose) printf( "%d: %d bytes written\n",
                                      p_thread_param->thread_id,  result );
             }
             else {
                 printf( "%d: ERROR: cannot write file %s!\n",
                         p_thread_param->thread_id, filename );
                 break;        
             }
        }
        fclose(fp);
	if (verbose) printf( "%d: w_chksum=%lu\n",
	                     p_thread_param->thread_id, w_chksum );
	
        if (verbose) printf( "%d: wait_befor_read %d sec(s)\n", 
                         p_thread_param->thread_id, WR_WAIT_DELAY );
        res = wait_nseconds(WR_WAIT_DELAY);
    
        fp = fopen( filename, "rb" );
	if (fp == NULL) {
            printf( "%d: ERROR: cannot open file %s!\n",
                    p_thread_param->thread_id, filename );
	}
	else {
            len = filesize * KILOBYTES;
   	    r_chksum = 0;
            while( len ) {
                if (len > sizeof(buffer)) count = sizeof(buffer);
                else count = len;
                result = fread( buffer, 1, count, fp );
                if (result == count) {
                    len -= result;
                    if (verbose) printf( "%d: %d bytes read\n",
                                         p_thread_param->thread_id,  result );
                }
                else {
                    printf( "%d: ERROR: cannot read file %s!\n",
                            p_thread_param->thread_id, filename );
                    break;        
                }
	        fill_len = count;
	        p = buffer;
	        while( fill_len--) r_chksum += *p++;
            }
            fclose(fp);
	    if (verbose) printf( "%d: r_chksum=%lu\n",
	                         p_thread_param->thread_id, r_chksum );
	    if (r_chksum != w_chksum) {
	        printf( "%d: ERROR: mismatch chksums!\n", p_thread_param->thread_id );
	    }
	    else {
	        if (verbose) printf( "%d: chksum OK\n", p_thread_param->thread_id );
	    }
	} 			     
    }
    
    
    
    /* Wait after write */
    wait_time = next_rnd_num() % (p_thread_param->max_sleep_time_after+1);
    if (verbose) printf( "%d: wait_after %d sec(s)\n", 
                         p_thread_param->thread_id, wait_time );
    res = wait_nseconds(wait_time);
    
    /* Remove working file */
    if (verbose) printf( "%d: remove file %s\n",
                         p_thread_param->thread_id, filename );
    unlink( filename );

    if (verbose) {
        printf( "%d: cycle %d completed\n", 
                 p_thread_param->thread_id, pass_num );
    }
    pass_num++;
        
    if (p_thread_param->loop_forever) goto forever;
    if (--passes_count) goto forever;
    
    p_thread_param = arg;
    if (verbose) {
       printf( "%d: finished\n", p_thread_param->thread_id );
    }
    
    return(NULL);
}


/*
 *  Print help screen
 */
void usage(void)
{
  fprintf( stderr, "\n" );
  fprintf( stderr, "Filesystem I/O testing program, version %s\n", prog_ver );
  fprintf( stderr, "Copyright (C) 2005 Dmitry Stefankov. All rights reserved.\n" );
  fprintf( stderr, "Usage: mfiles [-dhlrv] [-n threads] [-f filesize]\n" );
  fprintf( stderr, "       [-p passes] [-t workdir] [-a io_tmout_secs] [-L logfile]\n" );
  fprintf( stderr, "       [-u wait_before_max_secs] [-w wait_after_max_secs]\n" );  
  fprintf( stderr, "       -h   this help\n" );      
  fprintf( stderr, "       -l   run forever\n" );      
  fprintf( stderr, "       -r   fill files with random values\n" );      
  fprintf( stderr, "       -v   verbose output\n" );      
  fprintf( stderr, "Default parameters:\n" );
  fprintf( stderr, "   Passes number:    min=1, def=%d, max=unlimited\n", DEF_PASS_COUNT );    
  fprintf( stderr, "   Threads:          min=1, def=%d, max=%d\n", DEF_WORK_THREADS, MAX_WORK_THREADS );  
  fprintf( stderr, "   File   size:      min=1 KB, def=%d KB, max=unlimited\n", DEF_FILE_SIZE );
  fprintf( stderr, "   Working catalog:  %s\n", work_dir );
  fprintf( stderr, "   I/O op. timeout:  %d seconds\n", ALARM_SECS );
  fprintf( stderr, "   Wait before write:  %d seconds\n", DEF_WAIT_TIME );  
  fprintf( stderr, "   Wait after write:   %d seconds\n", DEF_WAIT_TIME );    
  fprintf( stderr, "\n" );    
  exit(1);
}



/*
 *  Main program stream
 */
int main( int argc, char ** argv )
{
  int                 op;
  int                 i;
  sigset_t            mask;
  struct sigaction    sa;
  char              * LogFile = NULL;
  unsigned long int   passes_count = DEF_PASS_COUNT;
  int                 fill_rand = 0;
  int                 loop_forever = 0;
  int                 wait_before_timeout = DEF_WAIT_TIME;
  int                 wait_after_timeout  = DEF_WAIT_TIME;  
  pthread_t           tid;
  size_t              default_stack_size = 0;
  size_t              new_stack_size = 0;
  pthread_attr_t      stack_size_custom_attr;
  int                 result_code;
  void              * arg;
  void              * p;
  struct thread_param worker_param;      
  pthread_t           thread_ids[MAX_WORK_THREADS];

/* Process command line  */  
  opterr = 0;
  while( (op = getopt(argc,argv,"a:hf:L:lmn:p:rt:vu:w:")) != -1)
    switch(op) {
      case 'a':
               io_alarm_secs = (unsigned int)atol(optarg);
               if (io_alarm_secs == 0) io_alarm_secs = ALARM_SECS;
      	       break;    
      case 't': 
               out_dir = optarg;
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
      case 'n':
                max_work_threads = atoi(optarg);
                if (max_work_threads > MAX_WORK_THREADS) max_work_threads = MAX_WORK_THREADS;
                if (max_work_threads == 0) max_work_threads = 1;
                break;
      case 'p':
                passes_count = atol(optarg);
                if (passes_count == 0) passes_count = 1; 
                break;
      case 'r':
               fill_rand = 1;
      	       break;
      case 'v':
               verbose = 1;
      	       break;       
      case 'u':
               wait_before_timeout = atoi(optarg);
               break;
      case 'w':
               wait_after_timeout = atoi(optarg);
               break;
      case 'h':
               usage();
               break;   
      default:
               break;
    }

   sigemptyset(&mask);
   sa.sa_mask = mask;
   sa.sa_flags = 0; 
   SIGNAL(SIGALRM, term );   
   SIGNAL(SIGHUP, term );
   SIGNAL(SIGTERM, term );
   SIGNAL(SIGINT, term );

   /* Init some variables */
   memset( thread_ids, 0, sizeof(thread_ids) );
   init_rnd_gen(1);
   
   /* Run workers */
   for( i=0; i<max_work_threads; i++ ) {

       pthread_attr_init(&stack_size_custom_attr);
       pthread_attr_getstacksize(&stack_size_custom_attr,&default_stack_size);
       if (verbose) {
           printf( "thread %d: default thread stacksize = %d\n", 
                    i, default_stack_size);   
       }
       if (default_stack_size < MIN_REQ_SSIZE) {
          pthread_attr_setstacksize(&stack_size_custom_attr,(size_t)MIN_REQ_SSIZE);    
          pthread_attr_getstacksize(&stack_size_custom_attr,&new_stack_size);    
          if (verbose) {
              printf( "thread %d: new thread stacksize = %d\n",  
                       i, new_stack_size);   
          }
          if (new_stack_size < MIN_REQ_SSIZE) {
              printf( "thread %d: not enough stack size to run thread!\n", i );
              return(2);
          }
       }


       memset( &worker_param, 0, sizeof(worker_param) );
       worker_param.thread_id = i;
       worker_param.max_file_size = max_file_size;
       worker_param.max_sleep_time_before = wait_before_timeout;
       worker_param.max_sleep_time_after = wait_after_timeout;
       worker_param.passes_count = passes_count;
       worker_param.loop_forever = loop_forever;
       worker_param.fill_rand = fill_rand;
       
       p = malloc( sizeof(worker_param) );
       if (p == NULL) {
           printf( "thread %d: not memopy to run thread!\n", i );
           return(4);
       }
       memcpy( p, &worker_param, sizeof(worker_param) );
       
       arg = p;
       
       result_code = pthread_create( &tid, &stack_size_custom_attr, worker_thread, arg );
       if (verbose) {   
           printf( "thread %d: create_thread result code = %d\n", 
                   i, result_code );
       }
       if (result_code != 0) {
           printf( "thread %d: create_thread_error = %d!\n", 
                   i, result_code );
           return(3);
       }
   
       thread_ids[i] = tid;
   }
  

   /* Wait for workers */
   for( i=0; i<max_work_threads; i++ ) {
       pthread_join( thread_ids[i], NULL );
       if (verbose) {
          printf( "thread %d: completed.\n", i );
       }
   }
   
   return(0);
}
