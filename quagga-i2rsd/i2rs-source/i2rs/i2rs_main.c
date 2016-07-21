
#include <zebra.h>

#include <lib/version.h>
#include "getopt.h"
#include "thread.h"
#include "prefix.h"
#include "linklist.h"
#include "if.h"
#include "vector.h"
#include "vty.h"
#include "command.h"
#include "filter.h"
#include "plist.h"
#include "stream.h"
#include "log.h"
#include "memory.h"
#include "privs.h"
#include "sigevent.h"
#include "zclient.h"
#include "vrf.h"

#include "i2rs/i2rs_main.h"
#include "i2rs/i2rs_vty.h"
#include "i2rs/i2rs_zebra.h"
/* ospfd privileges */
zebra_capabilities_t _caps_p [] = 
{
  ZCAP_NET_RAW,
  ZCAP_BIND,
  ZCAP_NET_ADMIN,
};

struct zebra_privs_t i2rsd_privs =
{
  .caps_p = _caps_p,
  .cap_num_p = array_size(_caps_p),
  .cap_num_i = 0
};

/* Configuration filename and directory.  */
char config_default[] =SYSCONFDIR I2RS_DEFAULT_CONFIG;

/* i2rs options. */
struct option longopts[] = 
{
  { "daemon",      no_argument,       NULL, 'd'},
  { "config_file", required_argument, NULL, 'f'},
  { "help",        no_argument,       NULL, 'h'},
  { "vty_port",    required_argument, NULL, 'P'},
  { "version",     no_argument,       NULL, 'v'},
  { 0 }
};

/* OSPFd program name */

/* Master of threads. */
struct thread_master *master;

/* Process ID saved for use by init system */
const char *pid_file = PATH_I2RSD_PID;


/* Help information display. */
static void __attribute__ ((noreturn))
usage (char *progname, int status)
{
  if (status != 0)
    fprintf (stderr, "Try `%s --help' for more information.\n", progname);
  else
    {    
      printf ("Usage : %s [OPTION...]\n\
Daemon which manages OSPF.\n\n\
-d, --daemon       Runs in daemon mode\n\
-f, --config_file  Set configuration file name\n\
-P, --vty_port     Set vty's port number\n\
-h, --help         Display this help and exit\n\
\n\
Report bugs to %s\n", progname, ZEBRA_BUG_ADDRESS);
    }
  exit (status);
}

/* SIGHUP handler. */
static void 
sighup (void)
{
  zlog (NULL, LOG_INFO, "SIGHUP received");
}

/* SIGINT / SIGTERM handler. */
static void
sigint (void)
{
  zlog_notice ("Terminating on signal");
  //i2rs_terminate ();
}

/* SIGUSR1 handler. */
static void
sigusr1 (void)
{
  zlog_rotate (NULL);
}

struct quagga_signal_t i2rs_signals[] =
{
  {
    .signal = SIGHUP,
    .handler = &sighup,
  },
  {
    .signal = SIGUSR1,
    .handler = &sigusr1,
  },  
  {
    .signal = SIGINT,
    .handler = &sigint,
  },
  {
    .signal = SIGTERM,
    .handler = &sigint,
  },
};

/* i2rsd main routine. */
int
main (int argc, char **argv)
{
  char *p;
  char *vty_addr = NULL;
  int vty_port = I2RS_VTY_PORT;
  int daemon_mode = 0;
  char *config_file = NULL;
  char *progname;
  struct thread thread;

  /* Set umask before anything for security */
  umask (0027);

	printf("Start i2rs\n");
  /* get program name */
  progname = ((p = strrchr (argv[0], '/')) ? ++p : argv[0]);

  while (1) 
    {
      int opt;

      opt = getopt_long (argc, argv, "df:i:z:hA:P:u:g:avC", longopts, 0);
    
      if (opt == EOF)
	break;

      switch (opt) 
	{
	case 0:
	  break;
	case 'd':
	  daemon_mode = 1;
	  break;
	case 'f':
	  //config_file = optarg;
	  break;
	case 'P':
          vty_port = atoi (optarg);
  	  break;
	case 'v':
	  print_version (progname);
	  exit (0);
	  break;
	case 'h':
	  usage (progname, 0);
	  break;
	default:
	  usage (progname, 1);
	  break;
	}
    }

  /* Invoked by a priviledged user? -- endo. */
  if (geteuid () != 0)
    {
      errno = EPERM;
      perror (progname);
      exit (1);
    }

  //zlog_default = openzlog (progname, ZLOG_I2RS,LOG_CONS|LOG_NDELAY|LOG_PID, LOG_DAEMON);

	printf("Start i2rs master thread\n");
  /* Initializations. */
  master = thread_master_create ();

	signal_init(master, array_size(i2rs_signals), i2rs_signals);
  /* Library inits. */
  cmd_init (1);
  //debug_init ();
	printf("Start i2rs Vty\n");
  vty_init (master);
  memory_init ();
	printf("Start i2rs Access and prefix\n");
  access_list_init ();
  prefix_list_init ();


  /* i2rs vty inits. */
  //i2rs_vty_init();

  printf("Start i2rs vtyconf\n");
  /* Get configuration file. */
  vty_read_config (config_file, config_default);

  
  /* Change to the daemon program. */
  if (daemon_mode && daemon (0, 0) < 0)
    {
	printf("Error\n");
      zlog_err("OSPFd daemon failed: %s", strerror(errno));
      exit (1);
    }
	printf("Start i2rs pid output\n");
  /* Process id file create. */
  pid_output (pid_file);
	printf("Start i2rs vty socket\n");
  /* Create VTY socket */
//  vty_serv_sock (vty_addr, vty_port, I2RS_VTYSH_PATH);

  /* Print banner. */
  zlog_notice ("i2rsd %s starting: vty@%d", QUAGGA_VERSION, vty_port);
	printf("Start i2rs thread loop\n");
  /* Fetch next active thread. */
  while (thread_fetch (master, &thread))
    thread_call (&thread);

  /* Not reached. */
  return (0);
}

