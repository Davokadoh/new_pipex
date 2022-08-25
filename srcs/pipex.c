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

void	ft_free(char *str)
{
	if (str)
		free(str);
}

void	free_all(char **new_av, char *path)
{
	free_tab(new_av);
	ft_free(path);
}

int	free_all_error(char **new_av, char *path, int error_code)
{
	free_all(new_av, path);
	return (error_code);
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

char	*get_path(char *program_name, char **envp)
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

void	infile(int io[2], int pipes[2][2])
{
	dup2(io[0], STDIN);
	dup2(pipes[0][WRITE], STDOUT);
}

void	outfile(int i, int io[2], int pipes[2][2])
{
	if (i % 2)
	{
		close(pipes[0][WRITE]);
		dup2(pipes[0][READ], STDIN);
	}
	else
	{
		close(pipes[1][WRITE]);
		dup2(pipes[1][READ], STDIN);
	}
	dup2(io[1], STDOUT);
}

int	child(int i, int max, int io[2], int ends[2])
{
	if (i == 0)
		infile();
	else if (i = max - 4)
		outfile();
	dup2(input[0], 0);
	close(ends[0]);
	close(ends[1]);
	return (1);
}

int	execute(int i, int max, int io[2], int ends[2])
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid == -1)
		return (-1);
	if (pid == 0)
		return (child(i, max, io, ends)); //Should return 1
	waitpid(pid, &status, 0);
    dup2(ends[1], 1);
    close(ends[0]);
    close(ends[1]);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (0);
}

static int	run_pipeline(int ncmds, char ***cmds)
{
	pid_t	pid;

	pid = fork();
	if (pid < 0)
		err_msg("Failed to fork");
	else if (pid == 0)
		exec_nth_command(ncmds, cmds);
	else
		if (WIFEXITED(status))
			return (WEXITSTATUS(status));
		else
			return (0);
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

	if (ac < 5)
		return (err_msg("Usage: ./pipex infile \"cmd1 args[]\" \"cmd2 args[]\" outfile"));
	if (check_io(io, av[1], av[ac - 1]))
		return (1);
	ncmds = ac - 4;
	i = -1
	while (++i < ncds)
		cmds[i] = ft_split(av[i + 2], ' ');
	return (run_pipeline(ncmds, cmds));
}

/*
int	main(int ac, char **av, char **envp)
{
	int		i;
	char	*path;
	char	**new_av;
	int		io[2];
	int		ends[2];
	int		status;

	status = 1;
	if (ac < 5)
		return (1);
	if (check_io(io, av[1], av[ac - 1]))
		return (1);
	i = -1;
	while (++i < ac - 3)
	{
		if (pipe(ends) == -1)
			return (1);
		new_av = ft_split(av[i + 2], ' ');
		if (!new_av || !*new_av || !**new_av)
			return (free_all_error(new_av, path, 1));
		path = get_path(new_av[0], envp);
		if (!path || !*path)
			return (free_all_error(new_av, path, 1));
		status = execute(i, ac, io, ends);
		if (status)
			if (execve(path, new_av, envp) == -1)
			{
				perror(NULL);
				return(free_all_error(new_av, path, 1));
			}
		free_all(new_av, path);
	}
	close(io[0]);
	close(io[1]);
	return (status);
}
*/
