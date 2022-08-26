/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jleroux <marvin@42lausanne.ch>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/26 11:09:08 by jleroux           #+#    #+#             */
/*   Updated: 2022/08/26 16:02:21 by jleroux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "pipex.h"
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#define STDIN 0
#define STDOUT 1

void	err_msg_exit(char *msg)
{
	write(2, msg, ft_strlen(msg));
	exit(127);
}

static int	exec_nth_cmd(int ncmds, char ***cmds, char **envp, int io[2]);

static void	exec_prev_cmd(int ncmds, char ***cmds, char **envp, int io[2])
{
	pid_t	pid;
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
		if (exec_nth_cmd(ncmds - 1, cmds, envp, io) == 127)
			return ;
	}
	dup2(input[0], STDIN);
	close(input[0]);
	close(input[1]);
}

static int	exec_nth_cmd(int ncmds, char ***cmds, char **envp, int io[2])
{
	if (ncmds == 0)
		dup2(io[0], STDIN);
	if (ncmds > 0)
		exec_prev_cmd(ncmds, cmds, envp, io);
	if (execve(get_path(cmds[ncmds][0], envp), cmds[ncmds], envp) == -1)
	{
		perror(NULL);
		return (127);
	}
	return (1);
}

static int	run_pipeline(int ac, char ***cmds, char **envp, int io[2])
{
	int		i;
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid == 0)
		return (exec_nth_cmd(ac - 4, cmds, envp, io));
	waitpid(pid, &status, 0);
	i = -1;
	while (++i < ac - 3)
		ft_free_tab(cmds[i]);
	free(cmds);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (0);
}

int	main(int ac, char **av, char **envp)
{
	char	***cmds;
	int		io[2];
	int		i;

	if (ac != 5)
		err_msg_exit(
			"Usage: ./pipex infile \"cmd1 args[]\" \"cmd2 args[]\" outfile\n");
	if (check_io(io, av[1], av[ac - 1]) == 1)
		return (1);
	cmds = malloc((ac - 3) * sizeof(char ***));
	i = -1;
	while (++i < ac - 3)
		cmds[i] = ft_split(av[i + 2], ' ');
	if (dup2(io[1], STDOUT) == -1)
		return (1);
	return (run_pipeline(ac, cmds, envp, io));
	return (0);
}
