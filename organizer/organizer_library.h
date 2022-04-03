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

typedef struct {
    int day, month, year;
} Date;

typedef struct {
    int id;
    char title[MAX_CHARS];
    short priority;
    char status;
    Date due_date;
    Date registration_date;
} Task;

int leap_year(int year) ;

int last_day(int month, int year) ;

int is_valid(int day, int month, int year) ;

long size_of(char path[]) ;

void give_id(Task *tasks, char *path) ;

void registration_date(Task *tasks) ;

int check_if_name_exists(char *path, char *argv[], int arg_num) ;

int give_title(char *argv[], Task *tasks, int arg_num) ;

int give_priority(char *argv[], Task *tasks, int arg_num) ;

int give_status(char *argv[], Task *tasks, int arg_num) ;

int check_due_date_str(char *due_date) ;

void due_date_str_to_int(char *due_date, int *date) ;

void due_date_initiation(char *due_date_str, Task *tasks, int *date) ;

int give_due_date(char *argv[], Task *tasks, int arg_num) ;

int check_if_given_task_exists(char *path, int id) ;

int check_if_any_status_found(char ch, char *path) ;

int check_if_priority_given(int argc, char *argv[]) ;

int check_if_status_given(int argc, char *argv[]) ;

int check_if_title_given(int argc, char *argv[]) ;

int check_if_due_date_given(int argc, char *argv[]) ;

int check_if_basic_command(int argc, char *argv[]) ;

int check_if_title_due_date_given(int argc, char *argv[]) ;

void basic_command(int argc, char *argv[], Task *tasks, int *ppriority, int *ptitle, int *pstatus, int *pdue_date) ;

void secondary_command(int argc, char *argv[], Task *tasks, int *ppriority, int *ptitle, int *pstatus, int *pdue_date) ;

void save(Task *tasks, char path[]) ;

int check_arguments(int argc, char *argv[], Task *tasks) ;

int longest_string(char *path) ;

int longest_specified_string(char *path, char ch) ;

void put_char(int n, char ch) ;

void put_table_h(int longest_strlen) ;

void put_table_t(int longest_strlen) ;

void put_table_bdy(int longest_strlen, Task read_value) ;

int list_all(char *path) ;

int give_total_task_num(char *path) ;

void task_array(char *path, Task tasks[]) ;

void sort_priority_asc(Task tasks[], int total_task_num) ;

void sort_priority_desc(Task tasks[], int total_task_num) ;

void display_sorted_tasks(Task tasks[], int total_task_num, int longest_strlen) ;

int show_priority_order(char ch) ;

void task_list_file_priority(int argc, char *argv[]) ;

int date_conversion(Task task) ;

void sort_due_date_asc(Task tasks[], int total_task_num) ;

void sort_due_date_desc(Task tasks[], int total_task_num) ;

int show_due_date_order(char *path, char ch) ;

void task_list_file_due_date(int argc, char *argv[]) ;

int list_specific_status(char ch) ;

void task_list_file(int argc, char *argv[], char *path) ;

int remove_tasks(int argc, char *argv[]) ;

void task_remove_file(int argc, char *argv[]) ;

void task_conclude_file() ;

void task_rewind_file() ;

void task_files(int argc, char *argv[], Task tasks) ;

void help_files() ;

void files_execution(int argc, char *argv[], Task task) ;