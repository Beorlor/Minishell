#include "src/header/minishell.h"

void start_terminal(int ac, char **av, char **env)
{
   char *input;
    
    while (1) 
    {
        input = readline(VERT"→ "BLEU"FUTUR/CHEMIN/MINISHELL"VIOLET" > "RESET);
        if (!input) 
        {
            perror("readline");
            exit(EXIT_FAILURE);
        }
        free(input);
    }
}

int	main(int ac, char **av, char **env)
{
    init_terminal(env);
    start_terminal(ac, av, env);
    return 0;
}

