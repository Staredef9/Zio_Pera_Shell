/////////////////////////
//Un Job puo' avere sottoprocessi
//Molteplici sottoprocessi sono linkati lista
//Molteplici Job anche
//operazioni di retrieval e check su queste liste
//
////////////////////////
#include "sh_utils.h"

//modificato, bisogna passare per reference la
//lista anche non solo il pid perche' non e' extern
job	*find_job( job **first_job, pid_t pgid){
	job *j;

	for (j = *first_job; j; j = j->next){
		if(j->pgid == pgid)
			return j;
	}
	return NULL;
}

int	job_is_stopped(job *j){

	process *p;
	for (p = j->first_process; p; p = p->next){
	if (!p->completed && !p->stopped)
		return 0;
	}
	return 1;
}

int	job_is_completed(job *j){

	process *p;
	for (p = j->first_process; p; p = p->next){
	if (!p->completed)
		return 0;
	}
	return 1;
}
