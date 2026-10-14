#include <unistd.h>
/* Extracted from xview3-notify; see adjacent archived message for author. */
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <xview/notify.h>

typedef struct {
	int		as_pid;
	void	      (*as_outfn)();
	void	      (*as_diefn)();
	int		as_stdout;
	char	       *as_put;
	char	       *as_get;
	char		as_buf[BUFSIZ];
	char		as_end[4];
} async_t;

static Notify_value
my_inpfn(a, fd)
async_t	       *a;
int		fd;
{
	int		i, n;
	char	       *np;

	if (ioctl(fd, FIONREAD, &n) == 0)
		while (n > 0) {
			if (a->as_put == a->as_end) {
				i = a->as_get - a->as_buf;
				if (i) {
					bcopy(a->as_get, a->as_buf, i);
					a->as_get -= i;
					a->as_put -= i;
				} else {
					(*a->as_outfn)(a, a->as_get, a->as_put - a->as_get);
					a->as_get = a->as_put = a->as_buf;
				}
			}
			i = read(fd, a->as_put, a->as_end - a->as_put);
			if (i < 0)
				break;
			a->as_put += i;
			n -= i;
			*a->as_put = '\0';
			do {
				np = strchr(a->as_get, '\n');
				if (np) {
					*np = '\0';
					(*a->as_outfn)(a, a->as_get, np - a->as_get);
					a->as_get = np + 1;
				}
			} while (np);
		}
	return NOTIFY_DONE;
}

static Notify_value
my_wait3fn(a, pid, sts, res)
async_t	       *a;
int		pid;
int     *sts;
void	       *res;
{
	if (WIFEXITED(*sts)) {
		if (a->as_outfn) {
			notify_set_input_func((Notify_client)(a), NOTIFY_FUNC_NULL, a->as_stdout);
			close(a->as_stdout);
		}
		if (a->as_put > a->as_get)
			(*a->as_outfn)(a, a->as_get, a->as_put - a->as_get);
		if (a->as_diefn)
			(*a->as_diefn)(a, sts, res);
		free(a);
		return NOTIFY_DONE;
	}
	return NOTIFY_IGNORED;
}

/*
 * async_exec() creates a child process and execs the specified program
 * with the specified arguments.  The child is given a pipe for stdout
 * and any output it generates is buffered asynchronously using the
 * notifier untill a full line is accumulated at which time the user's
 * callback function, (*outfn)(), is called once for each line output
 * by the program.  The callback function should be defined as:
 *	void myoutfn(handle, buf, cnt)
 *	void   *handle;
 *	int	buf;
 *	int	cnt;
 * The buffer passed includes the newline (if present) but is not null
 * terminated.  a is the handle returned by async_exec() which the user
 * should treat as opaque.  
 *
 * When the child process terminates, the user's callback function,
 * (*diefn)(), is called with the exit status.  The callback function
 * should be defined as:
 *	void mydiefn(handle, status, usage)
 *	void   *handle;
 *	int *status;
 *	struct rusage *usage;
 * If the user doesn't care, either or both of the callback functions
 * may be NULL.
 */

void *
async_exec(path, argv, outfn, diefn)
char	       *path;		/* path of program to execute */
char	      **argv;		/* arguments to pass to program */
void	      (*outfn)();	/* function to call with program's output */
void	      (*diefn)();	/* function to call when program terminates */
{
	async_t	       *a;
	int		pinp[2];
	int		pout[2];
	int		i;

	a = (async_t *) calloc(1, sizeof (async_t));
	if (!a)
		return (void *) a;
	a->as_outfn = outfn;
	a->as_diefn = diefn;
	a->as_put = a->as_get = a->as_buf;
	if (outfn) {
		pipe(pout);
		a->as_stdout = pout[0];
	}
	switch (a->as_pid = fork()) {
	case -1:	/* fork failed */
		if (outfn) {
			close(pout[0]);
			close(pout[1]);
		}
		free(a);
		return (void *) 0;
	case 0:		/* the child */
		if (outfn) {
			dup2(pout[1], 1);
			dup2(pout[1], 2);
		}
		for (i = getdtablesize() - 1; i > 2; --i)
			close(i);
		close(0);
		for (i = 0; i < NSIG; ++i)
			signal(i, SIG_DFL);
		execvp(path, argv);
		perror(path);
		_exit(-1);
		break;
	default:	/* the parent */
		if (outfn) {
			close(pout[1]);
			notify_set_input_func((Notify_client)(a), my_inpfn, a->as_stdout);
		}
		notify_set_wait3_func((Notify_client)(a), my_wait3fn, a->as_pid);
		break;
	}
	return a;
}

/*
 * This function takes the opaque handle returned by async_exec() or
 * passed to the callback functions and returns the PID of the child.
 */
int
async_pid(a)
async_t	       *a;
{
	return a->as_pid;
}

/*
 * This function kills (or sends a signal to) the child associated with
 * the opaque handle returned by async_exec() or passed to the callback
 * functions.
 */
int
async_kill(a, sig)
async_t	       *a;
int		sig;
{
	return kill(a->as_pid, sig);
}


