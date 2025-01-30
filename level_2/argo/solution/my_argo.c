#include "argo.h"
#include <string.h>

int	peek(FILE *stream)
{
	int	c = getc(stream);
	ungetc(c, stream);
	return c;
}

void	unexpected(FILE *stream)
{
	if (peek(stream) != EOF)
		printf("unexpected token '%c'\n", peek(stream));
	else
		printf("unexpected end of input\n");
}

int	accept(FILE *stream, char c)
{
	if (peek(stream) == c)
	{
		(void)getc(stream);
		return 1;
	}
	return 0;
}

int	expect(FILE *stream, char c)
{
	if (accept(stream, c))
		return 1;
	unexpected(stream);
	return 0;
}

int	get_int(json *dst, FILE *stream)
{
	int value = 0;

	fscanf(stream, "%d", &value);
	dst->type = INTEGER;
	dst->integer = value;
	return (1);
}

void print_json(json *data) {
    if (data->type == MAP) {
        printf("{ ");
        for (size_t i = 0; i < data->map.size; i++) {
            printf("\"%s\": ", data->map.data[i].key);
            if (data->map.data[i].value.type == INTEGER) {
                printf("%d", data->map.data[i].value.integer);
            } else {
                printf("\"%s\"", data->map.data[i].value.string);
            }
            if (i < data->map.size - 1) printf(", ");
        }
        printf(" }\n");
    }
}

char	*get_str(FILE *stream)
{
	char	*ret = calloc(4096, sizeof(char));
	int		i = 0;
	char	c = getc(stream);

	while (1)
	{
		c = getc(stream);

		if (c == '"')
			break;
		if (c == EOF)
		{	
			unexpected(stream);
			return (NULL);
		}
		if (c == '\\')//permet le gestion de \ et " et '
			c = getc(stream);
		ret[i++] = c;
	}
	return (ret);
}

int	parse_map(json *dst, FILE *stream)
{
	dst->type = MAP;
	dst->map.size = 0;
	dst->map.data = NULL;

	char	c = getc(stream);

	if (peek(stream) == '}')
		return (1);
	while (1)
	{
		c = getc(stream);
    	while (c == ' ' || c == '\t' || c == '\n')
        	c = getc(stream);
    	ungetc(c, stream);
		c = peek(stream);
		if (c != '"')
			return (-1);
		dst->map.data = realloc(dst->map.data, (dst->map.size + 1) * sizeof(pair));
		pair *current = &dst->map.data[dst->map.size];
		current->key = get_str(stream);
		dst->map.size++;
		if (current->key == NULL)
			return (-1);
		if (expect(stream, ':') == 0)
			return (-1);
		c = peek(stream);
		if (c == ' ' || c == '\t' || c == '\n')
		{
			while ((c == ' ' || c == '\t' || c == '\n'))
        		c = getc(stream);
			ungetc(c, stream);
		}
		if (argo(&current->value, stream) == -1)
			return (-1);
		c = peek(stream);
		if (c == '}')
		{
			accept(stream, c);
			break;
		}
		else if (c == ',')
		{
			accept(stream, c);
		}
		else
		{
			unexpected(stream);
			return (-1);
		}
	}
	return (1);
}

int	parse(json *dst, FILE *stream)
{
	int c = peek(stream);

	if (c == EOF)
	{
		unexpected(stream);
		return (-1);
	}
	if (isdigit(c))
		return(get_int(dst, stream));
	else if (c == '"')
	{
		dst->type = STRING;
		dst->string = get_str(stream);
		if (dst->string == NULL)
			return (-1);
		return (1);
	}
	else if (c == '{')
	{
		parse_map(dst, stream);
		return (1);
	}
	else
	{
		unexpected(stream);
		return (-1);
	}
	return (1);
}

int argo(json *dst, FILE *stream)
{
	if (parse(dst, stream) == -1)
		return (-1);
	return (1);
}
