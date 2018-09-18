#include <assert.h>
#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "dir.h"

int dir_list_files_in_dir(queue_t* queue, const char* path, bool recursive)
{
	assert(queue);
	assert(path);
	assert(*path);

	// Try to open the directory
	DIR* dir = opendir(path);
	if ( dir == NULL )
		return fprintf(stderr, "levdist: error: could not open directory %s\n", path), 1;

	// Load the directory entries (contents) one by one
	struct dirent* entry;
	while ( (entry = readdir(dir)) != NULL )
	{
		// If the entry is a directory
		if ( entry->d_type == DT_DIR )
		{
			// Ignore hidden files and directories
			if ( *entry->d_name == '.' )
				continue;

			// If files should be listed recursively
			if ( recursive )
			{
				// Concatenate the directory to the path
				char relative_path[PATH_MAX];
				sprintf(relative_path, "%s/%s", path, entry->d_name);

				// Load files in the subdirectory
				dir_list_files_in_dir(queue, relative_path, recursive);
			}
		}
		else // The entry is not a directory
		{
			// Concatenate the directory to the path
			char relative_path[PATH_MAX];
			sprintf(relative_path, "%s/%s", path, entry->d_name);

			// Append the file or symlink to the queue
			queue_append(queue, strdup(relative_path));
		}
	}

	// Sucess
	closedir(dir);
	return 0;
}
