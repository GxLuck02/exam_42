/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_popen.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ttreichl <ttreichl@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/03 16:30:20 by ttreichl          #+#    #+#             */
/*   Updated: 2024/12/03 17:31:56 by ttreichl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define BUFFER_SIZE 1024

char *get_next_line(int fd) {
    static char buffer[BUFFER_SIZE];
    static size_t start = 0, end = 0;
    char *line = NULL;
    size_t len = 0;

    if (fd < 0 || BUFFER_SIZE <= 0) {
        return NULL;
    }

    while (1) {
        if (start >= end) { // Recharge le tampon si nécessaire
            ssize_t bytes_read = read(fd, buffer, BUFFER_SIZE);
            if (bytes_read <= 0) { // Fin du fichier ou erreur
                if (len > 0) { // Retourne la dernière ligne partielle
                    line = realloc(line, len + 1);
                    line[len] = '\0';
                    return line;
                }
                return NULL;
            }
            start = 0;
            end = bytes_read;
        }

        for (; start < end; start++) {
            if (buffer[start] == '\n') { // Fin de ligne trouvée
                line = realloc(line, len + 2);
                line[len++] = buffer[start++];
                line[len] = '\0';
                return line;
            }

            // Copie le caractère actuel dans la ligne
            line = realloc(line, len + 1);
            line[len++] = buffer[start];
        }
    }
}

int	ft_popen(char const *file, char *const argv[], char type)
{
	pid_t	pid;
	int		fd[2];

	if (type != 'r' && type != 'w')
		return (-1);
	if (argv[0] != NULL && file != NULL)
	{
		if (pipe(fd) == -1)
			return (-1);
		pid = fork();
		if (pid == -1)
		{
			close(fd[0]);
			close(fd[1]);
			return (-1);
		}
		if (pid == 0)//child
		{
			if (type == 'r')
			{
				close(fd[0]);
				if (dup2(fd[1], STDOUT_FILENO) == -1)
					return (-1);
			}
			else if (type == 'w')
			{
				close(fd[1]);
				if (dup2(fd[0], STDIN_FILENO) == -1)
					return (-1);
			}
			else
				return (-1);
			execvp(file, argv);
		}
		else//parent
		{
			if (type == 'r')
			{
				close(fd[1]);
				return (fd[0]);
			}
			else if (type == 'w')
			{
				close(fd[0]);
				return (fd[1]);
			}
			else
			close(fd[0]);
    		close(fd[1]);
    		return (-1);
		}
	}
	return (-1);
}

int main() {
    char *line_ft, *line_std;
    FILE *fp_std;
    int fd_ft;

    // Commande à tester
    char const *command = "ls";
    char * const args[] = {"ls", NULL};

    // Utilisation de ft_popen
    fd_ft = ft_popen(command, args, 'r');
    if (fd_ft == -1) {
        perror("ft_popen failed");
        return 1;
    }

    // Utilisation de popen
    fp_std = popen("ls", "r");
    if (fp_std == NULL) {
        perror("popen failed");
        close(fd_ft);
        return 1;
    }

    // Lecture ligne par ligne et comparaison
    printf("Comparaison des sorties :\n");
    while (1) {
        line_ft = get_next_line(fd_ft);
        char buffer[1024];
        line_std = fgets(buffer, sizeof(buffer), fp_std);

        if (line_ft == NULL && line_std == NULL) {
            break; // Fin des deux flux
        }

        printf("ft_popen: %s", line_ft ? line_ft : "(null)");
        printf("popen:    %s", line_std ? line_std : "(null)");

        if (line_ft && line_std && strcmp(line_ft, line_std) != 0) {
            printf("Les lignes sont différentes !\n");
        }

        free(line_ft);
    }

    // Fermeture des descripteurs
    close(fd_ft);
    pclose(fp_std);

    return 0;
}

/*
int main() {
    // Test avec 'r' (lecture)
    printf("Test avec type 'r' (lecture)\n");

    char *args[] = {"ls", "-l", NULL};
    int fd_read = ft_popen("ls", args, 'r');

    if (fd_read == -1) {
        perror("ft_popen failed (reading)");
        return 1;
    }

    char buffer[1024];
    ssize_t bytes;
    while ((bytes = read(fd_read, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes] = '\0'; // Null-terminate la chaîne
        printf("%s", buffer); // Affiche la sortie de la commande `ls -l`
    }
    close(fd_read);

    // Test avec 'w' (écriture)
    printf("\nTest avec type 'w' (écriture)\n");

    char *message = "Hello from parent to child!\n";
    int fd_write = ft_popen("cat", args, 'w');  // Utilisation de "cat" comme exemple

    if (fd_write == -1) {
        perror("ft_popen failed (writing)");
        return 1;
    }

    write(fd_write, message, strlen(message));  // Écrire dans le pipe
    close(fd_write);

    return 0;
}*/