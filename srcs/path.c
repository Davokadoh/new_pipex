/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jleroux <marvin@42lausanne.ch>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/26 15:39:18 by jleroux           #+#    #+#             */
/*   Updated: 2022/08/26 16:01:59 by jleroux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "pipex.h"
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int	path_error(char **paths, int i)
{
	if (!paths[i])
	{
		ft_free_tab(paths);
		err_msg_exit("command not found\n");
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
	ft_free_tab(paths);
	return (path);
}

int	check_io(int io[2], char *infile, char *outfile)
{
	io[0] = open(infile, O_RDONLY);
	if (io[0] == -1)
		perror(infile);
	io[1] = open(outfile, O_CREAT | O_RDWR | O_TRUNC, 0644);
	if (io[1] == -1)
		perror(outfile);
	if (io[0] == -1 || io[1] == -1)
		return (1);
	return (0);
}
