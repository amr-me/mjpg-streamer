/*******************************************************************************
#                                                                              #
#      MJPG-streamer allows to stream JPG frames from an input-plugin          #
#      to several output plugins                                               #
#                                                                              #
#      Copyright (C) 2007 Tom Stöveken                                         #
#                                                                              #
# This program is free software; you can redistribute it and/or modify         #
# it under the terms of the GNU General Public License as published by         #
# the Free Software Foundation; version 2 of the License.                      #
#                                                                              #
# This program is distributed in the hope that it will be useful,              #
# but WITHOUT ANY WARRANTY; without even the implied warranty of               #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                #
# GNU General Public License for more details.                                 #
#                                                                              #
# You should have received a copy of the GNU General Public License            #
# along with this program; if not, write to the Free Software                  #
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA    #
#                                                                              #
*******************************************************************************/


























/******************************************************************************
Description.: Display a help message
Input Value.: argv[0] is the program name and the parameter progname
Return Value: -
******************************************************************************/
void help(char *progname)
{
}

/******************************************************************************
Description.: pressing CTRL+C sends signals to this process instead of just
              killing it plugins can tidily shutdown and free allocated
              resources. The function prototype is defined by the system,
              because it is a callback function.
Input Value.: sig tells us which signal was received
Return Value: -
******************************************************************************/
void signal_handler(int sig)
{
}

int split_parameters(char *parameter_string, int *argc, char **argv)
{
}

/******************************************************************************
Description.:
Input Value.:
Return Value:
******************************************************************************/
int main(int argc, char *argv[])
{
    //char *input  = "input_uvc.so --resolution 640x480 --fps 5 --device /dev/video0";
    char *input[MAX_INPUT_PLUGINS];
    char *output[MAX_OUTPUT_PLUGINS];
    int daemon = 0, i, j;
    size_t tmp = 0;

    output[0] = "output_http.so --port 8080";
    global.outcnt = 0;
    global.incnt = 0;

    /* parameter parsing */
    while(1) {
        int option_index = 0, c = 0;
        static struct option long_options[] = {
            {"h", no_argument, 0, 0
            },
            {"help", no_argument, 0, 0},
            {"i", required_argument, 0, 0},
            {"input", required_argument, 0, 0},
            {"o", required_argument, 0, 0},
            {"output", required_argument, 0, 0},
            {"v", no_argument, 0, 0},
            {"version", no_argument, 0, 0},
            {"b", no_argument, 0, 0},
            {"background", no_argument, 0, 0},
            {0, 0, 0, 0}
        };

        c = getopt_long_only(argc, argv, "", long_options, &option_index);

        /* no more options to parse */
        if(c == -1) break;

        /* unrecognized option */
        if(c == '?') {
            help(argv[0]);
            return 0;
        }

        switch(option_index) {
            /* h, help */

        case 1:
            help(argv[0]);
            return 0;
            break;

            /* i, input */

        case 3:
            input[global.incnt++] = strdup(optarg);
            break;

            /* o, output */

        case 5:
            output[global.outcnt++] = strdup(optarg);
            break;

            /* v, version */

        case 7:
            printf("MJPG Streamer Version: %s\n" \
            "Compilation Date.....: %s\n" \
            "Compilation Time.....: %s\n",
















        }
    }











    /* ignore SIGPIPE (send by OS if transmitting to closed TCP sockets) */
    signal(SIGPIPE, SIG_IGN);

    /* register signal handler for <CTRL>+C in order to clean up */
    if(signal(SIGINT, signal_handler) == SIG_ERR) {
    }

    /*
     * messages like the following will only be visible on your terminal
     * if not running in daemon mode
     */






    /* check if at least one output plugin was selected */
    if(global.outcnt == 0) {
        /* no? Then use the default plugin instead */
        global.outcnt = 1;
    }

    /* open input plugin */
    for(i = 0; i < global.incnt; i++) {
        /* this mutex and the conditional variable are used to synchronize access to the global picture buffer */
        if(pthread_mutex_init(&global.in[i].db, NULL) != 0) {
        }
        if(pthread_cond_init(&global.in[i].db_update, NULL) != 0) {
        }

        tmp = (size_t)(strchr(input[i], ' ') - input[i]);
        global.in[i].stop      = 0;
        global.in[i].buf       = NULL;
        global.in[i].size      = 0;
        global.in[i].plugin = (tmp > 0) ? strndup(input[i], tmp) : strdup(input[i]);
        global.in[i].handle = dlopen(global.in[i].plugin, RTLD_LAZY);







        }
        global.in[i].init = dlsym(global.in[i].handle, "input_init");
        if(global.in[i].init == NULL) {
            LOG("%s\n", dlerror());
            exit(EXIT_FAILURE);
        }
        global.in[i].stop = dlsym(global.in[i].handle, "input_stop");
        if(global.in[i].stop == NULL) {
            LOG("%s\n", dlerror());
            exit(EXIT_FAILURE);
        }
        global.in[i].run = dlsym(global.in[i].handle, "input_run");
        if(global.in[i].run == NULL) {
            LOG("%s\n", dlerror());
            exit(EXIT_FAILURE);
        }


















    }

    /* open output plugin */
    for(i = 0; i < global.outcnt; i++) {
        tmp = (size_t)(strchr(output[i], ' ') - output[i]);
        global.out[i].plugin = (tmp > 0) ? strndup(output[i], tmp) : strdup(output[i]);
        global.out[i].handle = dlopen(global.out[i].plugin, RTLD_LAZY);
        if(!global.out[i].handle) {
            LOG("ERROR: could not find output plugin %s\n", global.out[i].plugin);
            LOG("       Perhaps you want to adjust the search path with:\n");
            LOG("       # export LD_LIBRARY_PATH=/path/to/plugin/folder\n");
            LOG("       dlopen: %s\n", dlerror());
            closelog();
            exit(EXIT_FAILURE);
        }
        global.out[i].init = dlsym(global.out[i].handle, "output_init");
        if(global.out[i].init == NULL) {
            LOG("%s\n", dlerror());
            exit(EXIT_FAILURE);
        }
        global.out[i].stop = dlsym(global.out[i].handle, "output_stop");
        if(global.out[i].stop == NULL) {
            LOG("%s\n", dlerror());
            exit(EXIT_FAILURE);
        }
        global.out[i].run = dlsym(global.out[i].handle, "output_run");
        if(global.out[i].run == NULL) {
            LOG("%s\n", dlerror());
            exit(EXIT_FAILURE);
        }









        split_parameters(global.out[i].param.parameters, &global.out[i].param.argc, global.out[i].param.argv);

        global.out[i].param.global = &global;
        global.out[i].param.id = i;





    }

    /* start to read the input, push pictures into global buffer */
    DBG("starting %d input plugin\n", global.incnt);
    for(i = 0; i < global.incnt; i++) {
        syslog(LOG_INFO, "starting input plugin %s", global.in[i].plugin);
        if(global.in[i].run(i)) {
            LOG("can not run input plugin %d: %s\n", i, global.in[i].plugin);
            closelog();
            return 1;
        }
    }







    /* wait for signals */
    pause();

    return 0;
}
