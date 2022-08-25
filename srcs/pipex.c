#include "libft.h"
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#define STDIN 0
#define STDOUT 1
#define READ 0
#define WRITE 1

char	*ft_append(char *s1, char *s2)
{
	char	*str;

	if (!s1 || !s2)
		return (NULL);
	str = (char *) malloc((ft_strlen(s1) + ft_strlen(s2)) * sizeof(*s1));
	if (!str)
		return (NULL);
	ft_strlcpy(str, s1, ft_strlen(s1) + 1);
	ft_strlcpy(str + ft_strlen(s1), s2, ft_strlen(s2) + 1);
	free(s1);
	return (str);
}

void	free_tab(char **tab)
{
	int	i;

	i = -1;
	while (tab[++i])
		if (tab[i])
			free(tab[i]);
	free(tab);
}

int	path_error(char **paths, int i)
{
	if (!paths[i])
	{
		perror("path error");
		free_tab(paths);
		return (1);
	}
	return (0);
}

static char	*get_path(char *program_name, char **envp)
{
	int		i;
	char	*path;
	char	**paths;

	i = -1;
	while (envp[++i])
	{
		if (ft_strnstr(envp[i], "PATH=", 5))
			paths = ft_split(envp[i] + 5, ':');
	}
	i = -1;
	while (paths[++i])
	{
		paths[i] = ft_append(paths[i], "/");
		paths[i] = ft_append(paths[i], program_name);
		if (access(paths[i], F_OK | X_OK) == 0)
			break ;
	}
	if (path_error(paths, i))
		return (NULL);
	path = ft_strdup(paths[i]);
	free_tab(paths);
	return (path);
}

static int	exec_nth_command(int ncmds, char ***cmds, char **envp)
{
	pid_t	pid;
	int		status;
	int		input[2];

    if (ncmds < 0)
		return (1);
    if (ncmds >= 0) {
        if (pipe(input) != 0)
            err_sysexit("Failed to create pipe");
        if ((pid = fork()) < 0)
            err_sysexit("Failed to fork");
        if (pid == 0 && ncmds > 1)
            exec_previous_command(ncmds, cmds, input);
		wait(&pid);
        dup2(input[0], 0);
        close(input[0]);
        close(input[1]);
    }
    if (execve(get_path(cmds[ncmds][0]), cmds[ncmds], envp) == -1)
	    err_sysexit("Failed to exec %s", cmds[ncmds][0]);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (1);
}

static void exec_previous_command(int ncmds, char ***cmds, char **envp, int output[2]) {
    dup2(output[1], 1);
    close(output[0]);
    close(output[1]);
    exec_nth_command(ncmds - 1, cmds, envp);
}

static int check_io(int io[2], char *infile, char *outfile)
{
	io[0] = open(infile, O_RDONLY);
	io[1] = open(outfile, O_CREAT | O_RDWR | O_TRUNC, 0644);
	if (io[0] == -1 | io[1] == -1)
		return (1);
	return (0);
}

int	main(int ac, char **av, char **envp)
{
	char	***cmds = [ac - 3];
	int		ncmds;

	if (ac < 5)
		return (err_msg("Usage: ./pipex infile \"cmd1 args[]\" \"cmd2 args[]\" outfile"));
	if (check_io(io, av[1], av[ac - 1]))
		return (1);
	ncmds = ac - 4;
	//malloc cmds;
	i = -1
	while (++i < ncds)
		cmds[i] = ft_split(av[i + 2], ' ');
	return (exec_nth_cmd(ncmds, cmds, envp));
}
