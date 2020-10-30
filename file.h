#ifndef __FILE_H__
#define __FILE_H__

typedef struct Directory {
// The name of the directory
char *name;

// TODO: The list of files of the current directory
struct File_node *file_lst;

// TODO: The list of directories of the current directory
struct Directory_node *dir_lst;

// The parent directory of the current directory (NULL for the root directory)
struct Directory *parentDir;
} Directory;

// DO NOT MODIFY THIS STRUCTURE
typedef struct File {
// The name of the file
char *name;

// The size of the file
int size;

// The content of the file
char *data;

// The directory in which the file is located
Directory *dir;
} File;
typedef struct Directory_node {

Directory *dir;
struct Directory_node *prev;
struct Directory_node *next;
} dir_node;

typedef struct File_node {

File *file;
struct File_node *prev;
struct File_node *next;
} file_node;

#endif /* __FILE_H__ */
