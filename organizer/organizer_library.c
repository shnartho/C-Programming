#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "organizer_library.h"

#define PATH "data.bin"
#define MAX_CHARS 257
#define HELP "--help"
#define TASK "task"
#define CREATE "create"
#define P "-p"
#define T "-t"
#define S "-s"
#define D "-d"
#define LS "ls"
#define STATUS "--status"
#define CONCLUDED "concluded"
#define PENDING "pending"
#define RM "rm"
#define CONCLUDE "conclude"
#define REWIND "rewind"
#define ASC "asc"
#define ORDER "--order"
#define PRIORITY "priority"
#define DESC "desc"
#define DUE_DATE "due_date"

int leap_year(int year) {
    if (year % 4 != 0) {
        return 0;
    } else if (year % 25 != 0) {
        return 1;
    } else if (year % 16 != 0) {
        return 0;
    } else {
        return 1;
    }
}

int last_day(int month, int year) {
    switch (month) {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            return 31;
        case 4:
        case 6:
        case 9:
        case 11:
            return 30;
        case 2:
            return leap_year(year) ? 29 : 28;
        default:
            return -1;
    }
}

int is_valid(int day, int month, int year) {
    char dayvalid = day >= 1 && day <= last_day(month, year);
    char monthvalid = month >= 1 && month <= 12;
    char yearvalid = year >= 1900;
    return dayvalid && monthvalid && yearvalid;
}

long size_of(char path[]) {
    FILE *fp = fopen(path, "rb");
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fclose(fp);
    return size;
}

void give_id(Task *tasks, char *path) {
    int id_num = 0;
    int new_id;
    FILE *fp = fopen(path, "a+b");
    if (size_of(path) == 0) {
        tasks->id = id_num;
    } else {
        fseek(fp, -sizeof(Task), SEEK_END);
        fread(&new_id, sizeof(int), 1, fp);
        tasks->id = new_id + 1;
    }
    fclose(fp);
}

void registration_date(Task *tasks) {
    Date date;
    time_t currentTime;
    time(&currentTime);
    struct tm *date_today = localtime(&currentTime);
    date.day = date_today->tm_mday;
    date.month = date_today->tm_mon + 1;
    date.year = date_today->tm_year + 1900;
    tasks->registration_date = date;
}

int check_if_name_exists(char *path, char *argv[], int arg_num) {
    Task read_value;
    FILE *fp = fopen(path, "rb");
    while (fread(&read_value, sizeof(Task), 1, fp) == 1) {
        if (strcmp(read_value.title, argv[arg_num + 1]) == 0)
            return 0;
    }
    fclose(fp);
    return 1;
}

int give_title(char *argv[], Task *tasks, int arg_num) {
    if (check_if_name_exists(PATH, argv, arg_num) == 1)
        strcpy(tasks->title, argv[arg_num + 1]);
    else if (check_if_name_exists(PATH, argv, arg_num) == 0) {
        printf("- task already exists, please try another name!\n");
        return -1;
    }
    return 1;
}

int give_priority(char *argv[], Task *tasks, int arg_num) {
    short p = atoi(argv[arg_num + 1]);
    if (arg_num == -1) {
        tasks->priority = 1;
        return 1;
    }
    if (p >= 1 && p <= 10) {
        tasks->priority = p;
        return 1;
    } else {
        printf("- invalid priority! priority must be between 1 - 10\n");
        return -1;
    }
}

int give_status(char *argv[], Task *tasks, int arg_num) {
    if (arg_num == -1) {
        tasks->status = 'P';
        return 1;
    }
    if (strlen(argv[arg_num + 1]) != 1) {
        printf("- invalid status, please use p or c!\n");
        return -1;
    } else {
        char ch = argv[arg_num + 1][0];
        if (ch == 'P' || ch == 'C' || ch == 'p' || ch == 'c') {
            switch (ch) {
                case 'P':
                case 'p':
                    tasks->status = 'P';
                    break;
                case 'C':
                case 'c':
                    tasks->status = 'C';
                    break;
            }
            return 1;
        } else {
            printf("invalid status, please use p or c!\n");
            return -1;
        }
    }
}

int check_due_date_str(char *due_date) {
    int length_of_string = strlen(due_date);
    return length_of_string > 7 || length_of_string < 11 && (due_date[2] == '-' && due_date[5] == '-') ||
           (due_date[1] == '-' && due_date[3] == '-') || (due_date[2] == '-' && due_date[4] == '-') ||
           (due_date[1] == '-' && due_date[4] == '-');
}

void due_date_str_to_int(char *due_date, int *date) {
    char date_separation[5];
    int i = 0, j, k = 0, l;
    while (i < 3) {
        for (j = k, l = 0; due_date[j] != '\0'; j++, l++) {
            date_separation[l] = due_date[j];
            if (due_date[j] == '-')
                break;
        }
        date_separation[l] = '\0';
        date[i] = atoi(date_separation);
        k = j + 1;
        i++;
    }
}

void due_date_initiation(char *due_date_str, Task *tasks, int *date) {
    Date due_date;
    due_date_str_to_int(due_date_str, date);
    due_date.day = date[0];
    due_date.month = date[1];
    due_date.year = date[2];
    tasks->due_date = due_date;
}

int give_due_date(char *argv[], Task *tasks, int arg_num) {
    int date[3];
    if (check_due_date_str(argv[arg_num + 1]) == 0) {
        printf("- invalid date!\n");
        return -1;
    }
    due_date_str_to_int(argv[arg_num + 1], date);
    if (is_valid(date[0], date[1], date[2])) {
        due_date_initiation(argv[arg_num + 1], tasks, date);
        return 1;
    } else {
        printf("- invalid date!\n");
        return -1;
    }
}

int check_if_given_task_exists(char *path, int id) {
    Task read_value;
    FILE *fp = fopen(path, "rb");
    while (fread(&read_value, sizeof(Task), 1, fp) == 1) {
        if (read_value.id == id)
            return 1;
    }
    fclose(fp);
    return 0;
}

int check_if_any_status_found(char ch, char *path) {
    Task read;
    FILE *fp = fopen(path, "rb");
    while (fread(&read, sizeof(Task), 1, fp) == 1) {
        if (read.status == ch)
            return 1;
    }
    fclose(fp);
    return 0;
}

int check_if_priority_given(int argc, char *argv[]) {
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], P) == 0)
            return 1;
    }
    return 0;
}

int check_if_status_given(int argc, char *argv[]) {
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], S) == 0)
            return 1;
    }
    return 0;
}

int check_if_title_given(int argc, char *argv[]) {
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], T) == 0)
            return 1;
    }
    return 0;
}

int check_if_due_date_given(int argc, char *argv[]) {
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], D) == 0)
            return 1;
    }
    return 0;
}

int check_if_basic_command(int argc, char *argv[]) {
    return argc > 3 && check_if_title_given(argc, argv) == 0 && check_if_due_date_given(argc, argv) == 0 &&
           check_if_status_given(argc, argv) == 0 && check_if_priority_given(argc, argv) == 0;
}

int check_if_title_due_date_given(int argc, char *argv[]) {
    int t = 0, d = 0;
    if (argc >= 4 && argc <= 11) {
        for (int i = 3; i < argc; i++) {
            if (strcmp(argv[i], T) == 0)
                t = 1;
            if (strcmp(argv[i], D) == 0)
                d = 1;
        }
    }
    if (t == 0 && d == 0) {
        printf("- title and due date not entered!\n");
        return -1;
    }

    if (t == 0 && d == 1) {
        printf("- title not entered!\n");
        return -1;
    }
    if (d == 0 && t == 1) {
        printf("- due date not entered!\n");
        return -1;
    }

    return t == d == 1;
}

void basic_command(int argc, char *argv[], Task *tasks, int *ppriority, int *ptitle, int *pstatus, int *pdue_date) {
    if (argc <= 7) {
        *ptitle = give_title(argv, tasks, 2);
        *pdue_date = give_due_date(argv, tasks, 3);
        if (argc == 5)
            *ppriority = give_priority(argv, tasks, -1);
        else
            *ppriority = give_priority(argv, tasks, 4);
        if (argc == 5 || argc == 6)
            *pstatus = give_status(argv, tasks, -1);
        else
            *pstatus = give_status(argv, tasks, 5);
    }
}

void secondary_command(int argc, char *argv[], Task *tasks, int *ppriority, int *ptitle, int *pstatus, int *pdue_date) {
    int i = 3;
    if (check_if_priority_given(argc, argv) == 0)
        *ppriority = give_priority(argv, tasks, -1);
    if (check_if_status_given(argc, argv) == 0)
        *pstatus = give_status(argv, tasks, -1);

    if (check_if_title_due_date_given(argc, argv) == 1) {
        while (i < argc) {
            if (strcmp(argv[i], P) == 0)
                *ppriority = give_priority(argv, tasks, i);
            if (strcmp(argv[i], T) == 0)
                *ptitle = give_title(argv, tasks, i);
            if (strcmp(argv[i], S) == 0)
                *pstatus = give_status(argv, tasks, i);
            if (strcmp(argv[i], D) == 0) {
                *pdue_date = give_due_date(argv, tasks, i);
            }
            i += 2;
        }
    }
}

void save(Task *tasks, char path[]) {
    FILE *fp = fopen(path, "ab");
    if (fp == NULL)
        printf("- Error, data.bin missing!\n");
    else
        fwrite(tasks, sizeof(Task), 1, fp);
    fclose(fp);
}

int check_arguments(int argc, char *argv[], Task *tasks) {

    int priority, title, status, due_date;
    int *ppriority = &priority, *ptitle = &title, *pstatus = &status, *pdue_date = &due_date;

    if (check_if_basic_command(argc, argv) == 1)
        basic_command(argc, argv, tasks, ppriority, ptitle, pstatus, pdue_date);
    else
        secondary_command(argc, argv, tasks, ppriority, ptitle, pstatus, pdue_date);
    return priority == 1 && title == 1 && status == 1 && due_date == 1;
}

int longest_string(char *path) {
    Task read_value;
    FILE *fp = fopen(path, "rb");
    int l_str_len = -1;
    while (fread(&read_value, sizeof(Task), 1, fp) == 1) {
        int len = strlen(read_value.title);
        if (l_str_len == -1) {
            l_str_len = len;
        }
        if (len > l_str_len) {
            l_str_len = len;
        }
    }
    return l_str_len;
}

int longest_specified_string(char *path, char ch) {
    Task read_value;
    FILE *fp = fopen(path, "rb");
    int l_str_len = -1;
    while (fread(&read_value, sizeof(Task), 1, fp) == 1) {
        if (read_value.status == ch) {
            int len = strlen(read_value.title);
            if (l_str_len == -1) {
                l_str_len = len;
            }
            if (len > l_str_len) {
                l_str_len = len;
            }
        }
    }
    return l_str_len;
}

void put_char(int n, char ch) {
    for (int i = 0; i < n; i++)
        putchar(ch);
}

void put_table_h(int longest_strlen) {
    int i;
    if (longest_strlen > 4)
        i = 63 + longest_strlen;
    else
        i = 67;
    put_char(i, '=');
    printf("\n| ID  | Task");
    if (longest_strlen > 4)
        put_char(longest_strlen - 4, ' ');
    printf(" | Priority | Status |  Due date  | Registration date |\n");
    put_char(i, '=');
    putchar('\n');
}

void put_table_t(int longest_strlen) {
    if (longest_strlen <= 4)
        put_char(67, '-');
    else
        put_char(63 + longest_strlen, '-');
    putchar('\n');
}

void put_table_bdy(int longest_strlen, Task read_value) {
    printf("| %03d ", read_value.id);
    printf("| %s", read_value.title);
    if (longest_strlen < 4)
        put_char(4 - longest_strlen, ' ');
    else
        put_char(longest_strlen - strlen(read_value.title), ' ');
    printf(" |    %02d    ", read_value.priority);
    printf("|   %c    ", read_value.status);
    printf("| %02d-%02d-%d ", read_value.due_date.day, read_value.due_date.month, read_value.due_date.year);
    printf("|    %02d-%02d-%d     |\n", read_value.registration_date.day, read_value.registration_date.month,
           read_value.registration_date.year);
}

int list_all(char *path) {
    Task read;
    FILE *fp = fopen(path, "rb");
    if (fp == NULL) {
        printf("- Error, data.bin missing!\n");
        return -1;
    }
    if (size_of(path) == 0) {
        printf("- no tasks found!\n");
        return -1;
    }

    int longest_strlen = longest_string(PATH);
    put_table_h(longest_strlen);
    while (fread(&read, sizeof(Task), 1, fp) == 1) {
        put_table_bdy(longest_strlen, read);
    }
    fclose(fp);
    put_table_t(longest_strlen);
    return 0;
}

int give_total_task_num(char *path) {
    long total_file_size = sizeof(path);
    return (long) total_file_size / sizeof(Task);
}

void task_array(char *path, Task tasks[]) {
    int i = 0;
    Task read_value;
    FILE *fp = fopen(path, "rb");
    while (fread(&read_value, sizeof(Task), 1, fp) == 1) {
        tasks[i++] = read_value;
    }
    fclose(fp);
}

void sort_priority_asc(Task tasks[], int total_task_num) {
    Task read_value;
    for (int i = 0; i < total_task_num; i++) {
        for (int j = 0; j < total_task_num; j++) {
            if (tasks[j].priority > tasks[i].priority) {
                read_value = tasks[i];
                tasks[i] = tasks[j];
                tasks[j] = read_value;
            }
        }
    }
}

void sort_priority_desc(Task tasks[], int total_task_num) {
    Task read_value;
    for (int i = 0; i < total_task_num; i++) {
        for (int j = 0; j < total_task_num; j++) {
            if (tasks[j].priority < tasks[i].priority) {
                read_value = tasks[i];
                tasks[i] = tasks[j];
                tasks[j] = read_value;
            }
        }
    }
}

void display_sorted_tasks(Task tasks[], int total_task_num, int longest_strlen) {
    put_table_h(longest_strlen);
    for (int i = 0; i < total_task_num; i++)
        put_table_bdy(longest_strlen, tasks[i]);
    put_table_t(longest_strlen);
}

int show_priority_order(char ch) {
    int total_task_num = give_total_task_num(PATH);
    int longest_strlen = longest_string(PATH);
    Task tasks[total_task_num];
    task_array(PATH, tasks);
    FILE *fp = fopen(PATH, "rb");
    if (fp == NULL) {
        printf("- Error, data.bin missing!\n");
        return -1;
    }
    fclose(fp);
    if (size_of(PATH) == 0) {
        printf("- no task found!\n");
        return -1;
    }
    if (ch == 'a') {
        sort_priority_asc(tasks, total_task_num);
    } else if (ch == 'd') {
        sort_priority_desc(tasks, total_task_num);
    }
    display_sorted_tasks(tasks, total_task_num, longest_strlen);
    return 0;
}

void task_list_file_priority(int argc, char *argv[]) {
    if (argc == 6) {
        if (strcmp(argv[5], ASC) == 0)
            show_priority_order('a');
        else if (strcmp(argv[5], DESC) == 0)
            show_priority_order('d');
    } else
        printf("- command not recognized!\n");
}

int date_conversion(Task task) {
    int year = (task.due_date.year - 1900) * 365;
    int month = (task.due_date.month - 1) * 31;
    int day = task.due_date.day;
    return year + month + day;
}

void sort_due_date_asc(Task tasks[], int total_task_num) {
    Task read_value;
    for (int j = 0; j < total_task_num; j++) {

        for (int k = 0; k < total_task_num; k++) {
            if (date_conversion(tasks[k]) > date_conversion(tasks[j])) {
                read_value = tasks[j];
                tasks[j] = tasks[k];
                tasks[k] = read_value;
            }
        }
    }
}

void sort_due_date_desc(Task tasks[], int total_task_num) {
    Task read_value;
    for (int j = 0; j < total_task_num; j++) {
        for (int k = 0; k < total_task_num; k++) {
            if (date_conversion(tasks[k]) < date_conversion(tasks[j])) {
                read_value = tasks[j];
                tasks[j] = tasks[k];
                tasks[k] = read_value;
            }
        }
    }
}

int show_due_date_order(char *path, char ch) {
    int total_task_num = give_total_task_num(path);
    int longest_strlen = longest_string(path);
    Task tasks[total_task_num];

    task_array(path, tasks);
    FILE *fp = fopen(path, "rb");
    if (fp == NULL) {
        printf("- Error, data.bin missing!\n");
        return -1;
    }
    if (size_of(path) == 0) {
        printf("- no tasks found!\n");
        return -1;
    }
    fclose(fp);
    if (ch == 'd') {
        sort_due_date_desc(tasks, total_task_num);
    } else if (ch == 'a') {
        sort_due_date_asc(tasks, total_task_num);
    }

    display_sorted_tasks(tasks, total_task_num, longest_strlen);
    return 0;
}

void task_list_file_due_date(int argc, char *argv[]) {
    if (argc == 6) {
        if (strcmp(argv[5], ASC) == 0)
            show_due_date_order(PATH, 'a');
        else if (strcmp(argv[5], DESC) == 0)
            show_due_date_order(PATH, 'd');
    } else
        printf("- command not recognized!\n");
}

int list_specific_status(char ch) {
    Task read;
    char path[] = PATH;
    FILE *fp = fopen(path, "rb");
    if (fp == NULL) {
        printf("- Error, data.bin missing!\n");
        return -1;
    }
    if (size_of(path) == 0) {
        printf("- no tasks found!\n");
        return -1;
    }
    if (check_if_any_status_found(ch, path) == 0) {
        if (ch == 'P')
            printf("- no task found to be pending\n");
        if (ch == 'C')
            printf("- no task found to be concluded\n");
        return -1;
    }


    int longest_strlen = longest_specified_string(path, ch);
    put_table_h(longest_strlen);
    while (fread(&read, sizeof(Task), 1, fp) == 1) {
        if (read.status == ch) {
            put_table_bdy(longest_strlen, read);
        }
    }
    fclose(fp);
    put_table_t(longest_strlen);
    return 0;
}

void task_list_file(int argc, char *argv[], char *path) {
    if (argc == 3) {
        list_all(path);
    }
    else if (argc == 5 && strcmp(argv[3], STATUS) == 0) {
        if (strcmp(argv[4], CONCLUDED) == 0)
            list_specific_status('C');
        else if (strcmp(argv[4], PENDING) == 0)
            list_specific_status('P');
    }
    else if (argc == 6 && strcmp(argv[3], ORDER) == 0) {
        if (strcmp(argv[4], PRIORITY) == 0)
            task_list_file_priority(argc, argv);
        else if (strcmp(argv[4], DUE_DATE) == 0)
            task_list_file_due_date(argc, argv);
    } else
        printf("- command not recognized!\n");
}

int remove_tasks(int argc, char *argv[]) {
    Task read_value;
    char data[] = PATH;
    char temp[] = "temp.bin";
    int arg_num = 3;
    int count = 0;
    while (arg_num < argc) {
        FILE *tempp = fopen(temp, "wb");
        FILE *datap = fopen(data, "rb");
        if (datap == NULL) {
            printf("- Error, data.bin missing!\n");
            return -1;
        }
        if (check_if_given_task_exists(data, atoi(argv[arg_num])) == 0) {
            printf("- task %03d does not exist!\n", atoi(argv[arg_num]));
            arg_num++;
            continue;
        } else
            count++;

        while (fread(&read_value, sizeof(Task), 1, datap) == 1) {
            if (read_value.id != atoi(argv[arg_num]))
                fwrite(&read_value, sizeof(Task), 1, tempp);
        }
        fclose(datap);
        fclose(tempp);
        remove(data);
        rename(temp, data);
        arg_num++;
    }
    return count;
}

void task_remove_file(int argc, char *argv[]) {
    if (argc > 3) {
        int count = remove_tasks(argc, argv);
        if (count == 1)
            printf("- task removed successfully!\n");
        else if (count > 1)
            printf("- tasks removed successfully!\n");
    } else
        printf("- file id not entered!\n");
}

void task_conclude_file() {

}

void task_rewind_file() {

}

void task_files(int argc, char *argv[], Task tasks) {
    if (strcmp(argv[2], CREATE) == 0) {
        if (argc == 3)
            printf("- informations not entered!\n");
        if (argc > 3) {
            registration_date(&tasks);
            give_id(&tasks, PATH);
            if (check_arguments(argc, argv, &tasks) == 1) {
                save(&tasks, PATH);
                printf("- task saved successfully!\n");
            }
        }
    } else if (argc >= 3 && strcmp(argv[2], LS) == 0) {
        task_list_file(argc, argv, PATH);
    } else if (argc > 3 && strcmp(argv[2], RM) == 0) {
        task_remove_file(argc, argv);
    } else if (argc > 3 && strcmp(argv[2], CONCLUDE) == 0) {
        task_conclude_file();
    } else if (argc > 3 && strcmp(argv[2], REWIND) == 0) {
        task_rewind_file();
    } else printf("- command not recognized!\n");
}

void help_files() {
    FILE *help_file = fopen("C_project_help_file.txt", "r");
    char c;
    while ((c = fgetc(help_file)) != EOF) {
        fprintf(stdout, "%c", c);
    }
    putchar('\n');
    fclose(help_file);
}

void files_execution(int argc, char *argv[], Task task) {
    if (argc == 2 && strcmp(argv[1], HELP) == 0) {
        help_files();
    } else if (argc > 2 && strcmp(argv[1], TASK) == 0) {
        task_files(argc, argv, task);
    } else {
        fprintf(stdout, "- command not recognized!\n");
        return;
    }
}