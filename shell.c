#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "sh_utils.h"

pid_t	shell_pgid;
struct	termios shell_tmodes;
int	shell_terminal;
int	shell_is_interactive;

void	init_shell(){
	shell_terminal = STDIN_FILENO;
	shell_is_interactive = isatty(shell_terminal);
	if(shell_is_interactive){
		while(tcgetpgrp(shell_terminal) != (shell_pgid = getpgrp()))
			kill(- shell_pgid, SIGTTIN);
		signal (SIGINT, SIG_IGN);
		signal (SIGQUIT, SIG_IGN);
		signal (SIGTSTP, SIG_IGN);
		signal (SIGTTIN, SIG_IGN);
		signal (SIGTTOU, SIG_IGN);
		signal (SIGCHLD, SIG_IGN);
		
		shell_pgid = getpid();
		if (setpgid(shell_pgid, shell_pgid) < 0){
			perror("couldn't put shell into its own process group\n");
			exit(1);	
		}
		tcsetpgrp(shell_terminal, shell_pgid);
		tcgetattr(shell_terminal, &shell_tmodes);
	}
}

void	launch_process(process *p, pid_t pgid, int infile,
		int outfile, int errfile, int foreground)
{
	pid_t pid;

	if (shell_is_interactive){
		pid = getpid();
		if (pgid == 0)
			pgid = pid;
		setpgid(pid,pgid);
		if(foreground)
			tcsetpgrp(shell_terminal, pgid);
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		signal(SIGTSTP, SIG_DFL);
		signal(SIGTTIN, SIG_DFL);
		signal(SIGTTOU, SIG_DFL);
		signal(SIGCHLD, SIG_DFL);
	}
	if (infile != STDIN_FILENO)
	{
		dup2(infile, STDIN_FILENO);
		close(infile);
	}
	if (outfile != STDOUT_FILENO)
	{
		dup2(outfile, STDOUT_FILENO);
		close(outfile);
	}
	if (errfile != STDERR_FILENO)
	{
		dup2(errfile, STDERR_FILENO);
		close(errfile);
	}
	execvp(p->argv[0], p->argv);
	perror("execvp");
	exit(1);	
}

void	launch_job(job *j, int foreground)
{
	process	*p;
	pid_t	pid;
	int 	mypipe[2];
	int	infile;
	int	outfile;

	infile = j->stdin;
	for(p = j->first_process; p; p = p->next){
		if (p->next){
			if(pipe(mypipe) < 0)
			{
				perror("pipe");
				exit(1);
			}
			outfile = mypipe[1];
		}
		else{
			outfile = j->stdout; 
		}
		pid = fork();
		if (pid == 0){
			launch_process(p, j->pgid, infile,
					outfile, j->stderr, foreground);
		} else if(pid < 0){
			perror("fork");
			exit(1);
		} else {
			p->pid = pid;
			if(shell_is_interactive)
			{
				if(!j->pgid)
					j->pgid = pid;
				setpgid(pid,j->pgid);
			}
		}
		if (infile != j->stdin)
			close(infile);
		if (outfile != j->stdout)
			close(outfile);
		infile = mypipe[0];
	}
	format_job_info(j, "launched");

	if(!shell_is_interactive)
		wait_for_job(j);
	else if (foreground)
		put_job_in_foreground(j,0);
	else
		put_job_in_background(j,0);
}



int main(){

	//printf("Hello from shell!\n");

	char *one_liner = "$ZIO_PERA> ";
	char *lineptr = NULL;
	size_t n = 0;

//	job *first_job = NULL;	
	
	printf("%s ", one_liner);
	getline(&lineptr, &n, stdin);

	printf("\n%s\n ", lineptr);
	free(lineptr);

	return 0;	

}
