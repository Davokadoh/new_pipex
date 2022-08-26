/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jleroux <marvin@42lausanne.ch>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/26 15:41:13 by jleroux           #+#    #+#             */
/*   Updated: 2022/08/26 16:01:23 by jleroux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

int		path_error(char **paths, int i);
char	*get_path(char *program_name, char **envp);
void	err_msg_exit(char *msg);
int		check_io(int io[2], char *infile, char *outfile);

#endif
