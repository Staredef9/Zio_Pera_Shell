#ifndef SH_UTILS_H
#define SH_UTILS_H
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>


//TODO: implementare la logica del parsing e del passaggio di argv a process
typedef struct process{
	char 	**argv;
	char	completed;
	char	stopped;
	pid_t	pid;
	int	status;
	struct	process *next;
} process;


typedef struct job{
	char	*command;
	process	*first_process;
	pid_t	pgid;
	char	notified;
	struct	termios tmodes;
	int	stdin;
	int	stdout;
	int	stderr;
	struct job	*next;
}job;


job	*find_job(job **first_job, pid_t pgid);
int	job_is_stopped(job *j);
int	job_is_completed(job *j);


#endif
