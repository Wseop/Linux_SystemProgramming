#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

const int FILE_NUM = 10;
const int STR_LEN = 4096;

void ShowFiles(char (*files)[STR_LEN], int count) {
	for (int i = 0; i < count; i++) {
		printf("%s (%d)\n", files[i], i);
	}
}

int ToDigit(char* cmd) {
	if (!strcmp(cmd, "0") || !strcmp(cmd, "1") || !strcmp(cmd, "2") || !strcmp(cmd, "3") ||
			!strcmp(cmd, "4") || !strcmp(cmd, "5") || !strcmp(cmd, "6") || !strcmp(cmd, "7") ||
			!strcmp(cmd, "8") || !strcmp(cmd, "9")) {
		return atoi(cmd);
	}
	return -1;
}

int FindKeyword(char* file_name, char* keyword) {
	int file_len = strlen(file_name);
	int key_len = strlen(keyword);
	int matched = 1;

	for (int index = file_len - key_len; index >= 0; index--) {
		matched = 1;
		if (file_name[index] == '/') {
			matched = 0;
			break;
		}
		for (int i = 0; i < key_len; i++) {
			if (file_name[index + i] != keyword[i]) {
				matched = 0;
				break;
			}
		}
		if (matched) break;
	}
	return matched;
}

int main(int argc, char* argv[]) {
	FILE* fp;
	char file_list[FILE_NUM][STR_LEN];
	int file_count = 0;
	char cmd[STR_LEN];

	sprintf(cmd, "git ls-files `git rev-parse --show-toplevel` | grep %s", argv[1]);
	fp = popen(cmd, "r");
	if (fp == NULL) {
		printf("popen fail!\n");
		return -1;
	}
	while (fgets(file_list[file_count], STR_LEN, fp)) {
		file_list[file_count][strlen(file_list[file_count]) - 1] = '\0';
		file_count++;
		if (file_count == FILE_NUM) break;
	}
	pclose(fp);

	while (1) {
		int select = 0;
		int new_file_count = 0;

		ShowFiles(file_list, file_count);
		printf("Enter file shortcut (shown on the right) or keyword to further refine the search : ");
		scanf("%s", cmd);
		select = ToDigit(cmd);
		if (select != -1) {
			if (select >= file_count) {
				printf("Invalid Input, try again!\n");
				continue;
			}
			if (!execlp("/usr/bin/vim", "vim", file_list[select], NULL)) {
				printf("execlp fail!\n");
				return -1;
			}
		} else {
			for (int i = 0; i < file_count; i++) {
				if (FindKeyword(file_list[i], cmd)) {
					strcpy(file_list[new_file_count++], file_list[i]);
				}
			}
			file_count = new_file_count;
			if (file_count == 0) {
				printf("No matched File!\n");
				return 0;
			}
		}
	}

	return 0;
}
