#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "file.h"

//functia care returneaza indexul comenzii date
int create_index(char *command)
{
	if (!strcmp(command, "create fs"))
		return 1;
	if (!strncmp(command, "touch ", 6))
		return 2;
	if (!strncmp(command, "mkdir ", 6))
		return 3;
	if (!strcmp(command, "ls"))
		return 4;
	if (!strcmp(command, "pwd"))
		return 5;
	if (!strncmp(command, "cd ", 3))
		return 6;
	if (!strcmp(command, "tree"))
		return 7;
	if (!strncmp(command, "rm ", 3))
		return 8;
	if (!strncmp(command, "rmdir ", 6))
		return 9;
	if (!strncmp(command, "find", 4))
		return 10;
	if (!strcmp(command, "delete fs"))
		return 0;
	return -1;
}

//alocarea memoriei pentru root
void create_fs(Directory **root)
{
	(*root) = malloc(sizeof(**root));
	(*root)->name = (char *)malloc(2 * sizeof(char));
	strcpy((*root)->name, "/");
	(*root)->name[1] = '\0';
	(*root)->file_lst = NULL;
	(*root)->dir_lst = NULL;
	(*root)->parentDir = NULL;
}

//eliberarea memoriei ocupate de root
void delete_fs(Directory **root)
{
	free((*root)->name);
	free(*root);
}

//alocarea memoriei pentru un fisier nou
File *new_file(char *filename, char *content, Directory *dir)
{
	File *new_file = malloc(sizeof(*new_file));

	new_file->name = malloc((strlen(filename) + 1) * sizeof(char));
	strcpy(new_file->name, filename);
	new_file->name[strlen(new_file->name)] = '\0';
	new_file->data = malloc((strlen(content) + 1) * sizeof(char));
	strcpy(new_file->data, content);
	new_file->data[strlen(new_file->data)] = '\0';
	new_file->size = strlen(content);
	new_file->dir = dir;
	return new_file;
}

//alocarea memoriei pentru un director nou
Directory *new_directory(char *directoryname, Directory *parent)
{
	Directory *new_directory = malloc(sizeof(*new_directory));

	new_directory->name = malloc((strlen(directoryname)
	+ 1) * sizeof(char));
	strcpy(new_directory->name, directoryname);
	new_directory->name[strlen(new_directory->name)] = '\0';
	new_directory->file_lst = NULL;
	new_directory->dir_lst = NULL;
	new_directory->parentDir = parent;
	return new_directory;
}

/*adaugarea unui nod de lista in lista fisierelor din directorul curent,
 *dupa alocarea memoriei pentru fisierul din nodul respectiv
 */
void touch(char *filename, char *content, Directory *curr)
{
	file_node *new_file_n = malloc(sizeof(*new_file_n));

	new_file_n->file = new_file(filename, content, curr);
	if (curr->file_lst == NULL) {
		new_file_n->next = NULL;
		new_file_n->prev = NULL;
		curr->file_lst = new_file_n;
		return;
	}
	file_node *aux = curr->file_lst;
	file_node *prev = aux;

	if (strcmp(aux->file->name, filename) >= 0) {
		new_file_n->prev = NULL;
		new_file_n->next = prev;
		curr->file_lst = new_file_n;
		return;
	}
	while ((aux != NULL) && strcmp(aux->file->name, filename) <= 0) {
		prev = aux;
		aux = aux->next;
	}
	if (aux != NULL) {
		new_file_n->prev = prev;
		new_file_n->next = aux;
		aux->prev = new_file_n;
		prev->next = new_file_n;
	} else {
		new_file_n->next = NULL;
		new_file_n->prev = prev;
		prev->next = new_file_n;
	}
}

/*adaugarea unui nod de lista in lista directoarelor din directorul curent,
 *dupa alocarea memoriei pentru directorul din nodul respectiv
 */
void mkdir(char *dir_name, Directory *curr)
{
	dir_node *new_dir_n = malloc(sizeof(*new_dir_n));

	new_dir_n->dir = new_directory(dir_name, curr);
	if (curr->dir_lst == NULL) {
		new_dir_n->next = NULL;
		new_dir_n->prev = NULL;
		curr->dir_lst = new_dir_n;
		return;
	}
	dir_node *aux = curr->dir_lst;
	dir_node *prev = aux;

	if (strcmp(aux->dir->name, dir_name) >= 0) {
		new_dir_n->prev = NULL;
		new_dir_n->next = prev;
		prev->prev = new_dir_n;
		curr->dir_lst = new_dir_n;
		return;
	}
	while ((aux != NULL) && strcmp(aux->dir->name, dir_name) <= 0) {
		prev = aux;
		aux = aux->next;
	}
	if (aux != NULL) {
		new_dir_n->prev = prev;
		new_dir_n->next = aux;
		aux->prev = new_dir_n;
		prev->next = new_dir_n;
	} else {
		new_dir_n->next = NULL;
		new_dir_n->prev = prev;
		prev->next = new_dir_n;
	}
}

/*afisarea in ordine lexicografica a fisierelor si directoarelor
 *din listele corespunzatoare directorului curent
 */
void ls(Directory *curr)
{
	file_node *flst = curr->file_lst;
	dir_node *dlst = curr->dir_lst;

	if (flst != NULL) {
		while (flst->next != NULL) {
			printf("%s ", flst->file->name);
			flst = flst->next;
		}
		if ((flst != NULL) && (dlst == NULL))
			printf("%s", flst->file->name);
		else
			printf("%s ", flst->file->name);
	}
	if (dlst != NULL) {
		while (dlst->next != NULL) {
			printf("%s ", dlst->dir->name);
			dlst = dlst->next;
		}
		if (dlst != NULL)
			printf("%s", dlst->dir->name);
	}
	printf("\n");
}

//afisarea caii directorului curent
void pwd(Directory *curr)
{
	if (curr->parentDir == NULL) {
		printf("/");
		return;
	}
		pwd(curr->parentDir);
		if (curr->parentDir->parentDir != NULL)
			printf("/%s", curr->name);
		else
			printf("%s", curr->name);
}

//schimbarea directorului curent
void cd(Directory **curr, char *directoryname)
{
	dir_node *dlst = (*curr)->dir_lst;

	if (strcmp(directoryname, "..") == 0) {
		*curr = (*curr)->parentDir;
		return;
	}
	while (dlst != NULL) {
		if (strcmp(dlst->dir->name, directoryname) == 0) {
			(*curr) = dlst->dir;
			return;
		}
		dlst = dlst->next;
	}
	printf("Cannot move to '%s': No such directory!\n", directoryname);
}

//afisarea cu o anumita indentare a numelor fisierelor dintr-o lista de fisiere
void list_file(file_node *fls, int indent)
{
	while (fls != NULL) {
		printf("%*s", indent, " ");
		printf("%s\n", fls->file->name);
		fls = fls->next;
	}
}

/*afisarea sub forma arborescenta a continutului subsistemului de
 *fisiere, cu indentari diferite in funtie de nivelurile de adancime
 */
void tree(Directory *curr, int indent)
{
	if (indent != 0)
		printf("%*s", indent, " ");
	printf("%s\n", curr->name);
	indent = indent + 4;
	int prev;
	Directory *init = curr;
	dir_node *dir_aux = curr->dir_lst;

	list_file(curr->file_lst, indent);
	while (dir_aux != NULL) {
		prev = indent;
		tree(dir_aux->dir, indent);
		indent = prev;
		dir_aux = dir_aux->next;
	}
	curr = init;
}

//eliberarea memoriei pentru un fisier
void free_file(File **file_del)
{
	free((*file_del)->name);
	free((*file_del)->data);
	(*file_del)->dir = NULL;
	free(*file_del);
}

//stregerea unui fisier cu numele dat din directorul curent
void rm(Directory *curr, char *filename)
{
	file_node *aux1;
	file_node *aux2;
	file_node *prev;
	file_node *next;

	aux1 = curr->file_lst;
	while (aux1 != NULL) {
		aux2 = aux1;
		if (!strcmp(aux1->file->name, filename)) {
			if (!(aux1->prev)) {
				if (aux1->next != NULL) {
					aux1 = aux1->next;
					aux1->prev = NULL;
				} else {
					free_file(&(aux1->file));
					free(aux1);
					curr->file_lst = NULL;
					return;
				}
				aux2->next = NULL;
				free_file(&(aux2->file));
				free(aux2);
				if (aux1 != NULL)
					curr->file_lst = aux1;
				else
					curr->file_lst = NULL;
				return;
			}
			prev = aux1->prev;
			if (aux1->next != NULL) {
				next = aux1->next;
				prev->next = next;
				next->prev = prev;
			} else
				prev->next = NULL;
			free_file(&(aux2->file));
			aux2->prev = NULL;
			aux2->next = NULL;
			free(aux2);
			return;
		}
		aux1 = aux1->next;
	}
	printf("Cannot remove '%s': No such file!\n", filename);
}

//dezalocarea memoriei pentru o lista de fisiere
void dezal_flst(file_node **flst)
{
	file_node *cp = *flst;
	file_node *cp2;

	while (cp) {
		cp2 = cp;
		cp = cp->next;
		free_file(&(cp2->file));
		free(cp2);
	}
}

//dezalocarea memoriei pentru un director
void free_dir(Directory *dir_del)
{
	free(dir_del->name);
	dezal_flst(&(dir_del->file_lst));
	dir_node *aux1 = dir_del->dir_lst;
	dir_node *aux2;

	while (aux1 != NULL) {
		aux2 = aux1;
		aux1 = aux1->next;
		if (aux1 != NULL) {
			aux1->prev = NULL;
			aux2->next = NULL;
			aux2->prev = NULL;
		}

		free_dir(aux2->dir);
		free(aux2->dir);
		free(aux2);
	}
}

/*stergerea din lista directorului curent si
 *eliberarea memoriei a unui director
 */
void rmdir(Directory *curr, char *dirname)
{
	dir_node *aux1;
	dir_node *aux2;
	dir_node *prev;
	dir_node *next;

	aux1 = curr->dir_lst;
	while (aux1 != NULL) {
		aux2 = aux1;
		if (!strcmp(aux1->dir->name, dirname)) {
			if (aux1->prev == NULL) {
				if (aux1->next != NULL) {
					aux1 = aux1->next;
					aux1->prev = NULL;
				} else {
					free_dir(aux1->dir);
					free(aux1->dir);
					free(aux1);
					curr->dir_lst = NULL;
					return;
				}
				aux2->next = NULL;
				free_dir(aux2->dir);
				free(aux2->dir);
				free(aux2);
				if (aux1 != NULL)
					curr->dir_lst = aux1;
				else
					curr->dir_lst = NULL;
				return;
			}
			prev = aux1->prev;
			if (aux1->next != NULL) {
				next = aux1->next;
				prev->next = next;
				next->prev = prev;
			} else {
				prev->next = NULL;
			}
			free_dir(aux2->dir);
			aux2->prev = NULL;
			aux2->next = NULL;
			free(aux2->dir);
			free(aux2);
			return;
		}
		aux1 = aux1->next;
	}
	printf("Cannot remove '%s': No such directory!\n", dirname);
}

//cautarea unui fisier care corespunde unor conditii
void find(Directory *curr, int max_depth, int min_size,
int max_size, char *content)
{
	file_node *fnode = curr->file_lst;
	dir_node *dnode = curr->dir_lst;

	if (max_depth < 0)
		return;
	while (fnode != NULL) {
		if (fnode->file->size <= max_size)
			if (fnode->file->size >= min_size)
				if (strstr(fnode->file->data, content) != NULL)
					printf("%s ", fnode->file->name);

		fnode = fnode->next;
	}
	while (dnode != NULL) {
		find(dnode->dir, max_depth - 1, min_size, max_size, content);
		dnode = dnode->next;
	}
}

//salvarea comenzii primite de la tastatura si atribuirea indicelui acesteia
void command_f(char *command, char *cp, int *index_com)
{
	fgets(command, 100, stdin);
	command[strlen(command) - 1] = '\0';
	strcpy(cp, command);
	*index_com = create_index(command);
}

//extragerea numelui fisierului si a contentului pentru operatia touch
void pre_touch(char *cp, char *aux, char **filename, char **content)
{
	aux = strtok(cp, " ");
	aux = strtok(NULL, " ");
	free(*filename);
	*filename = strdup(aux);
	aux = strtok(NULL, "\n");
	free(*content);
	*content = strdup(aux);
}

//extragerea numelui directorului pentru operatiile mkdir, cd si rmdir
void dirname_extr(char *cp, char *aux, char **directoryname)
{
	aux = strtok(cp, " ");
	aux = strtok(NULL, "\n");
	free(*directoryname);
	*directoryname = strdup(aux);
}

//extragerea numelui fisierului pentru operatia rm
void pre_rm(char *cp, char *aux, char **filename)
{
	aux = strtok(cp, " ");
	aux = strtok(NULL, "\n");
	free(*filename);
	*filename = strdup(aux);
}

//extragerea parametrilor necesari operatiei find
void pre_find(char *cp, char *aux, char **max_depth,
	char **min_size, char **max_size, char **content)
{
	aux = strtok(cp, " ");
	aux = strtok(NULL, " ");
	free(*max_depth);
	*max_depth = strdup(aux);
	aux = strtok(NULL, " ");
	free(*min_size);
	*min_size = strdup(aux);
	aux = strtok(NULL, " ");
	free(*max_size);
	*max_size = strdup(aux);
	aux = strtok(NULL, "\n");
	free(*content);
	*content = strdup(aux);
}
int main(void)
{
	int index_com, depth, msize, Msize;
	Directory *root;
	Directory *curr;
	char *filename = (char *)malloc(30 * sizeof(char));
	char *content = (char *)malloc(30*sizeof(char));
	char *directoryname = (char *)malloc(20 * sizeof(char));
	char *aux;
	char *command = (char *)malloc(100 * sizeof(char));
	char *max_depth = (char *)malloc(100 * sizeof(char));
	char *min_size = (char *)malloc(100 * sizeof(char));
	char *max_size = (char *)malloc(100 * sizeof(char));
	char cp[100];

	command_f(command, cp, &index_com);
	aux = command;
	while (index_com != 0) {
		if (index_com == 1) {
			create_fs(&root);
			curr = root;
		}
		if (index_com == 2) {
			pre_touch(cp, command, &filename, &content);
			touch(filename, content, curr);
		}
		if (index_com == 3) {
			dirname_extr(cp, aux, &directoryname);
			mkdir(directoryname, curr);
		}
		if (index_com == 4)
			ls(curr);

		if (index_com == 5) {
			pwd(curr);
			printf("\n");
		}

		if (index_com == 6) {
			dirname_extr(cp, aux, &directoryname);
			cd(&curr, directoryname);
		}
		if (index_com == 7)
			tree(curr, 0);

		if (index_com == 8) {
			pre_rm(cp, aux, &filename);
			rm(curr, filename);
		}
		if (index_com == 9) {
			dirname_extr(cp, aux, &directoryname);
			rmdir(curr, directoryname);
		}
		if (index_com == 10) {
			pre_find(cp, aux, &max_depth, &min_size,
			&max_size, &content);
			depth = atoi(max_depth);
			msize = atoi(min_size);
			Msize = atoi(max_size);
			find(curr, depth, msize, Msize, content);
			printf("\n");
		}
	command_f(command, cp, &index_com);
	}

	delete_fs(&root);
	free(command);
	free(filename);
	free(content);
	free(directoryname);
	free(max_depth);
	free(min_size);
	free(max_size);
	return 0;
}
