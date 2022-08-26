/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jleroux <marvin@42lausanne.ch>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/26 11:09:08 by jleroux           #+#    #+#             */
/*   Updated: 2022/08/26 13:59:15 by jleroux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#define STDIN 0
#define STDOUT 1

static void	err_msg_exit(char *msg)
{
	write(2, msg, ft_strlen(msg));
	exit(1);
}

static int	path_error(char **paths, int i)
{
	if (!paths[i])
	{
		perror("path error");
		ft_free_tab(paths);
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
	ft_free_tab(paths);
	return (path);
}

static int	exec_nth_cmd(int ncmds, char ***cmds, char **envp, int io[2]);

static void	exec_prev_cmd(int ncmds, char ***cmds, char **envp, int io[2])
{
	pid_t	pid;
	int		status;
	int		input[2];

	if (pipe(input) != 0)
		err_msg_exit("Failed to create pipe\n");
	pid = fork();
	if (pid < 0)
		err_msg_exit("Failed to fork\n");
	if (pid == 0 && ncmds > 0)
	{
		dup2(input[1], STDOUT);
		close(input[0]);
		close(input[1]);
		exec_nth_cmd(ncmds - 1, cmds, envp, io);
	}
	waitpid(pid, &status, 0);
	dup2(input[0], STDIN);
	close(input[0]);
	close(input[1]);
}

static int	exec_nth_cmd(int ncmds, char ***cmds, char **envp, int io[2])
{
	if (ncmds > 0)
		exec_prev_cmd(ncmds, cmds, envp, io);
	if (ncmds == 0)
		dup2(io[0], 0);
	if (execve(get_path(cmds[ncmds][0], envp),
		cmds[ncmds], envp) == -1)
		err_msg_exit("Failed on exec\n");
	return (1);
}

static int	check_io(int io[2], char *infile, char *outfile)
{
	io[0] = open(infile, O_RDONLY);
	io[1] = open(outfile, O_CREAT | O_RDWR | O_TRUNC, 0644);
	if (io[0] == -1 | io[1] == -1)
		return (1);
	return (0);
}

int	main(int ac, char **av, char **envp)
{
	char	***cmds;
	pid_t	pid;
	int		status;
	int		io[2];
	int		i;

	if (ac < 5)
		err_msg_exit(
			"Usage: ./pipex infile \"cmd1 args[]\" \"cmd2 args[]\" outfile\n");
	if (check_io(io, av[1], av[ac - 1]) == -1)
		return (1);
	//close(io[0]);
	cmds = malloc((ac - 3) * sizeof(char ***));
	i = -1;
	while (++i < ac - 3)
		cmds[i] = ft_split(av[i + 2], ' ');
	pid = fork();
	if (dup2(io[1], STDOUT) == -1)
		return (1);
	if (pid == 0)
		return (exec_nth_cmd(ac - 4, cmds, envp, io));
	waitpid(pid, &status, 0);
	i = -1;
	while (++i < ac - 3)
		ft_free_tab(cmds[i]);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (0);
}
