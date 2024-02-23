#include "../header/minishell.h"

void	clear_terminal(char **env)
{
	pid_t	pid;
	char *args[] = {"clear", NULL};

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}
	else if (pid == 0)
	{
		if (execve("/usr/bin/clear", args, env) == -1)
		{
			perror("execve");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		waitpid(pid, NULL, 0);
	}
}

void  write_terminal_title()
{
  printf(VIOLET " __  __  _____  _   _  _____   "VIOLET_FONCE
                "_____  _    _  ______  _       _\n" RESET
         VIOLET "|  \\/  ||_   _|| \\ | ||_   _| "VIOLET_FONCE
                "/ ____|| |  | ||  ____|| |     | |\n" RESET
         VIOLET "| \\  / |  | |  |  \\| |  | |  "VIOLET_FONCE
         "| (___  | |__| || |__   | |     | |\n" RESET
         VIOLET "| |\\/| |  | |  | . ` |  | |  "VIOLET_FONCE
         " \\___ \\ |  __  ||  __|  | |     | |\n" RESET
         VIOLET "| |  | | _| |_ | |\\  | _| |_  "VIOLET_FONCE
         "____) || |  | || |____ | |____ | |____\n" RESET
         VIOLET "|_|  |_||_____||_| \\_||_____|"VIOLET_FONCE
         "|_____/ |_|  |_||______||______||______|\n\n" RESET);
}

void  edit_shlvl(char **env)
{
    while (*env) 
    {
        if (ft_strncmp(*env, "SHLVL=", 6) == 0) 
        {
            ft_strcpy(*env + 6, ft_itoa(ft_atoi(*env + 6) + 1));
            break;
        }
        env++;
    }
}

void  init_terminal(char **env)
{
  clear_terminal(env);
  edit_shlvl(env);
  write_terminal_title();
}
