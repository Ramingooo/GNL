/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adelkahl <adelkahl@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/15 18:37:48 by adelkahl          #+#    #+#             */
/*   Updated: 2026/11/15 18:37:48 by adelkahl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

static char	*ft_read_to_backup(int fd, char *backup)
{
	char	*buf;
	int		bytes_read;

	buf = malloc(BUFFER_SIZE + 1);
	if (!buf)
		return (NULL);
	bytes_read = 1;
	while (!ft_strchr(backup, '\n') && bytes_read > 0)
	{
		bytes_read = read(fd, buf, BUFFER_SIZE);
		if (bytes_read == -1)
		{
			free(buf);
			free(backup);
			return (NULL);
		}
		buf[bytes_read] = '\0';
		backup = ft_strjoin(backup, buf);
	}
	free(buf);
	return (backup);
}

static char	*ft_extract_line(char *backup)
{
	char	*line;
	size_t	i;
	size_t	len;

	if (!backup || !backup[0])
		return (NULL);
	len = 0;
	while (backup[len] && backup[len] != '\n')
		len++;
	if (backup[len] == '\n')
		len++;
	line = malloc(len + 1);
	if (!line)
		return (NULL);
	i = -1;
	while (++i < len)
		line[i] = backup[i];
	line[i] = '\0';
	return (line);
}

static char	*ft_update_backup(char *backup)
{
	char	*new_backup;
	size_t	i;

	i = 0;
	while (backup[i] && backup[i] != '\n')
		i++;
	if (!backup[i])
	{
		free(backup);
		return (NULL);
	}
	new_backup = ft_strdup(backup + i + 1);
	free(backup);
	return (new_backup);
}

char	*get_next_line(int fd)
{
	static char	*backup[MAX_FD];
	char		*line;

	if (fd < 0 || fd >= MAX_FD || BUFFER_SIZE <= 0)
		return (NULL);
	backup[fd] = ft_read_to_backup(fd, backup[fd]);
	if (!backup[fd])
		return (NULL);
	line = ft_extract_line(backup[fd]);
	backup[fd] = ft_update_backup(backup[fd]);
	return (line);
}
