/*
 *  This file is part of x48, an emulator of the HP-48sx Calculator.
 *  Copyright (C) 1994  Eddie C. Dost  (ecd@dressler.de)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* $Log: main.c,v $
 * Revision 1.11  1995/01/11  18:20:01  ecd
 * major update to support HP48 G/GX
 *
 * Revision 1.10  1994/12/07  20:20:50  ecd
 * changed initialization
 *
 * Revision 1.10  1994/12/07  20:20:50  ecd
 * changed initialization
 *
 * Revision 1.9  1994/11/28  02:00:51  ecd
 * reordered initialization. serial_init() is called after x11 init.
 *
 * Revision 1.8  1994/11/04  03:42:34  ecd
 * added call to parse_options()
 *
 * Revision 1.7  1994/11/02  14:44:28  ecd
 * call to emulate_debug() added
 *
 * Revision 1.6  1994/10/05  08:36:44  ecd
 * new function call to init_nibble_maps()
 *
 * Revision 1.5  1994/09/30  12:37:09  ecd
 * deleted saturn.intenable = 1; statement
 *
 * Revision 1.4  1994/09/18  22:47:20  ecd
 * fixed typo
 *
 * Revision 1.3  1994/09/18  15:29:22  ecd
 * turned off unused rcsid message
 *
 * Revision 1.2  1994/09/13  16:57:00  ecd
 * changed to plain X11
 *
 * Revision 1.1  1994/09/13  15:05:05  ecd
 * Initial revision
 *
 * $Id: main.c,v 1.11 1995/01/11 18:20:01 ecd Exp ecd $
 */

#include <string.h>
#include <jni.h>
#include <android/log.h> 

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <fcntl.h>

#include "x48.h"
#include "hp48.h"
#include "debugger.h"

#include <locale.h>

char  *progname;
char  *res_name;
char  *res_class;

int    saved_argc;
char **saved_argv;

saturn_t saturn;
int nb;
int exit_state = 1;

void
#ifdef __FunctionProto__
signal_handler(int sig)
#else
signal_handler(sig)
int sig;
#endif
{
  switch (sig) {
    case SIGINT:
      enter_debugger |= USER_INTERRUPT;
	//LOGI("-sigint handler-");
      break;
    case SIGALRM:
		got_alarm = 1;
      break;
    case SIGPIPE:
      exit_x48(0);
      exit (0);
    default:
      break;
  }
}

void
#ifdef __FunctionProto__
save_options(int argc, char **argv)
#else
save_options(argc, argv)
int    argc;
char **argv;
#endif
{
  int l;

  saved_argc = argc;
  saved_argv = (char **)malloc((argc + 2) * sizeof(char *));
  if (saved_argv == (char **)0)
    {
      LOGE( "%s: malloc failed in save_options(), exit\n", progname);
      exit (1);
    }
  saved_argv[argc] = (char *)0;
  while (argc--)
    {
      l = strlen(argv[argc]) + 1;
      saved_argv[argc] = (char *)malloc(l);
      if (saved_argv[argc] == (char *)0)
        {
          LOGE( "%s: malloc failed in save_options(), exit\n",
                  progname);
          exit (1);
        }
      memcpy(saved_argv[argc], argv[argc], l);
    }
}

int
#ifdef __FunctionProto__
main(int argc, char **argv)
#else
main(argc, argv)
int argc;
char **argv;
#endif
{
  char *name;
  sigset_t set;
  struct sigaction sa;
  long flags;
  struct itimerval it;

/*setlocale(LC_CTYPE, "");
printf("%s\n", setlocale(LC_ALL, NULL));
printf("%s\n", setlocale(LC_ALL, "en_US.utf8"));
printf("%s\n", nl_langinfo(CODESET));*/

  name = (char *)0;
  /*
   *  Get the name we are called.
   */
 /* progname = strrchr(argv[0], '/');
  if (progname == NULL)
    progname = argv[0];
  else
    progname++;*/

  /*
   * save command line options
   */
  //save_options(argc, argv);

  /*
   *  Open up the display
   */
 /* if (InitDisplay(argc, argv) < 0) {
    exit (1);
  }
*/
  /*
   * initialize emulator stuff
   */
   printf("0001\n");
  init_emulator();
  printf("0010\n");
  
  /*
   *  Create the HP-48 window
   */
  /*if (CreateWindows(saved_argc, saved_argv) < 0) {
    LOGE( "%s: can\'t create window\n", progname);
    exit (1);
  }
*/
  /*
   * can't be done before windows exist
   */
  init_active_stuff();

  printf("0020\n");

  /*
   *  install a handler for SIGALRM
   */
  sigemptyset(&set);
  sigaddset(&set, SIGALRM);
  sa.sa_handler = signal_handler;
  sa.sa_mask = set;
#ifdef SA_RESTART
  sa.sa_flags = SA_RESTART;
#endif
  sigaction(SIGALRM, &sa, (struct sigaction *)0);

  /*
   *  install a handler for SIGINT
   */
  sigemptyset(&set);
  sigaddset(&set, SIGINT);
  sa.sa_handler = signal_handler;
  sa.sa_mask = set;
#ifdef SA_RESTART
  sa.sa_flags = SA_RESTART;
#endif
  sigaction(SIGINT, &sa, (struct sigaction *)0);

  /*
   *  install a handler for SIGPIPE
   */
  sigemptyset(&set);
  sigaddset(&set, SIGPIPE);
  sa.sa_handler = signal_handler;
  sa.sa_mask = set;
#ifdef SA_RESTART
  sa.sa_flags = SA_RESTART;
#endif
  sigaction(SIGPIPE, &sa, (struct sigaction *)0);

  /*
   * set the real time interval timer
   */
  it.it_interval.tv_sec = 0;
  it.it_interval.tv_usec = 20000;
  it.it_value.tv_sec = 0;
  it.it_value.tv_usec = 20000;
  setitimer(ITIMER_REAL, &it, (struct itimerval *)0);

  /*
   * Set stdin flags to not include O_NDELAY and O_NONBLOCK
   */
  flags = fcntl(STDIN_FILENO, F_GETFL, 0);
  flags &= ~O_NDELAY;
  flags &= ~O_NONBLOCK;
  fcntl(STDIN_FILENO, F_SETFL, flags);

 

  
  do {

    if (!exec_flags)
      emulate ();
    else
      emulate_debug ();

	 debug();

  } while (1);

  return 0;
}


void
Java_org_ab_x48_X48_registerClass( JNIEnv* env, jobject caller, jobject callback )
{
	LOGI("--registerClass--");
	android_env = env;
	android_callback = callback;
	jclass x48 = (*android_env)->GetObjectClass(env, android_callback);
	LOGI("--x48 registered--");
	refreshMainScreen = (*android_env)->GetMethodID(android_env, x48, "refreshMainScreen", "([S)V");
	waitEvent = (*android_env)->GetMethodID(android_env, x48, "waitEvent", "()I");
	refreshIcons = (*android_env)->GetMethodID(android_env, x48, "refreshIcons", "([Z)V");
	emulatorReady = (*android_env)->GetMethodID(android_env, x48, "emulatorReady", "()V");
	pauseEvent = (*android_env)->GetMethodID(android_env, x48, "pauseEvent", "()V");
	LOGI("--methods registered--");
}

jstring
Java_org_ab_x48_X48_stopHPEmulator( JNIEnv* env, jobject thiz )
{
	exit (0);
	//exit_state = 0;
}

jstring
Java_org_ab_x48_X48_saveState( JNIEnv* env, jobject thiz )
{
	LOGI("save_state");
	write_files();
}

jstring
Java_org_ab_x48_X48_resetHPEmulator( JNIEnv* env, jobject thiz )
{
	do_reset();
}


jstring
Java_org_ab_x48_X48_startHPEmulator( JNIEnv* env, jobject thiz )
{

	LOGI("init_emulator");
exit_state = 1;
sigset_t set;
  struct sigaction sa;
  long flags;
  struct itimerval it;

  init_emulator();
  LOGI("init_active_stuff");

   init_active_stuff();

   
  sigemptyset(&set);
  sigaddset(&set, SIGALRM);
  sa.sa_handler = signal_handler;
  sa.sa_mask = set;
#ifdef SA_RESTART
  sa.sa_flags = SA_RESTART;
#endif
  sigaction(SIGALRM, &sa, (struct sigaction *)0);

  
  sigemptyset(&set);
  sigaddset(&set, SIGINT);
  sa.sa_handler = signal_handler;
  sa.sa_mask = set;
#ifdef SA_RESTART
  sa.sa_flags = SA_RESTART;
#endif
  sigaction(SIGINT, &sa, (struct sigaction *)0);


  sigemptyset(&set);
  sigaddset(&set, SIGPIPE);
  sa.sa_handler = signal_handler;
  sa.sa_mask = set;
#ifdef SA_RESTART
  sa.sa_flags = SA_RESTART;
#endif
  sigaction(SIGPIPE, &sa, (struct sigaction *)0);

 

  it.it_interval.tv_sec = 0;
  it.it_interval.tv_usec = 20000;
  it.it_value.tv_sec = 0;
  it.it_value.tv_usec = 20000;
  setitimer(ITIMER_REAL, &it, (struct itimerval *)0);

 
  flags = fcntl(STDIN_FILENO, F_GETFL, 0);
  flags &= ~O_NDELAY;
  flags &= ~O_NONBLOCK;
  fcntl(STDIN_FILENO, F_SETFL, flags);

 LOGI("emulate loop");

(*android_env)->CallVoidMethod(android_env, android_callback, emulatorReady);
  
  do {

    if (!exec_flags)
      emulate ();
    else
      emulate_debug ();

	 debug();

  } while (exit_state);
  LOGI("exit loop");

    return (*env)->NewStringUTF(env, "Hello from JNI !");
}

jint
Java_org_ab_x48_X48_buttonPressed( JNIEnv*  env,
                                      jobject  this,
                                      jint     x) {
	button_pressed(x);

}


jint
Java_org_ab_x48_X48_buttonReleased( JNIEnv*  env,
                                      jobject  this,
                                      jint     x) {
	button_released(x);

}


jint
Java_org_ab_x48_X48_loadProg( JNIEnv*  env,
                                      jobject  this,
                                      jstring     desc) {
	const char* cDesc = (*env)->GetStringUTFChars(env, desc, 0);
	int c = read_bin_file(cDesc);
	(*env)->ReleaseStringUTFChars(env, desc, cDesc);
	return c;
}
