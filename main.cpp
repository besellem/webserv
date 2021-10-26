#include <iostream>
#include <string>

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

int main() {
	DIR				*dir = opendir("./www");
	struct dirent	*dirInfo;
	struct stat		statBuf;

	std::string		addPrefix;

	std::string		fileName;
	std::string		lastModTime;
	std::string		fileSize;

	std::string		content;
	
	/* begin html */
	content = "Content-type:text/html\r\n\r\n";
	content += "<html>\n";
	content += "<head><title>autoindex</title></head>\n";
	content += "<body>\n";

	/* create table */
	content += "<table width=\"100%\" border=\"0\">\n";
	content += "<tr>\n";
	content += "<th align=\"left\">Name</th>\n";
	content += "<th align=\"left\">Last modified</th>\n";
	content += "<th align=\"left\">size</th>\n";
	content += "</tr>\n";

	if (dir == NULL)
		return 0;

	/* create table content */
	while ((dirInfo = readdir(dir)) != NULL) {
		fileName = dirInfo->d_name;
		if (fileName == ".") {
			fileName.clear();
			continue;
		}

		/* get absolut path */
		addPrefix = "./www/";
		addPrefix += fileName;

		/* get file status */
		stat(addPrefix.c_str(), &statBuf);

		/* get file modify time */
		lastModTime = ctime(&statBuf.st_mtime);
		lastModTime.erase(lastModTime.end() - 1);

		/* get file size */
		fileSize = std::to_string(statBuf.st_size);

		/* begin of content */
		content += "<tr>\n";

		/* element 1: path access */
		content += "<td><a href=\"";
		if (fileName == "..")
			content += fileName;
		else
			content += addPrefix;
		content += "\">";
		content += fileName;
		if (S_ISDIR(statBuf.st_mode))
			content += "/";		
		content += "</a></td>\n";

		/* element 2: modify time */
		content += "<td>";
		content += lastModTime;
		content += "</td>";

		/* element 3: file size */
		content += "<td>";
		content += fileSize;
		content += "</td>";

		/* end of content */
		content += "</tr>\n";

		addPrefix.clear();
		fileName.clear();
	}

	closedir(dir);

	/* end of html */
	content += "</table>\n";
	content += "</body>\n";
	content += "</html>\n";

	std::cout << content << std::endl;

	return 0;
}