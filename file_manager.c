#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h> //this is for open, close 

char * file_list[10];
pthread_t thread_list[5];
pthread_mutex_t mutex;

bool fileListControl (char * file_name, int islem) { //islem 0 = ekleme, islem 1 = çıkarma

    bool res = false;
    bool oDosyaVarMi = false;

    for (int j = 0; j < 10; j++) { //aynı isimde dosya olup olmadığını kontrol eden döngü
        if (file_list[j] == NULL) {
            continue;
        } else if(strcmp(file_list[j], file_name) == 0) {
            oDosyaVarMi = true;
        }
    }

    for(int i = 0; i< 10; i++) { //file list içini kontrol etmek için döngü

        if(islem == 0) { //ekleme

            if (oDosyaVarMi) {
                printf("ayni isimde dosya olduğundan oluşturulamadı..\n");
                return res;

            } else if(file_list[i] == NULL) {
                char *file_name2 = malloc(sizeof(file_name2));
                strcpy(file_name2, file_name);
                file_list[i] = file_name2;
                printf("added.\n");
                res = true;
                return res;
            }
            if (i == 9) {
                printf("yer yok.\n");
                return res;
            } 

        } else { //çıkarma - silme

            if(file_list[i] == NULL) {
                continue;

            } else if(strcmp(file_list[i], file_name) == 0) {
                file_list[i] = NULL;
                printf("deleted.\n");
                res = true;
                return res;
            }

            if (i == 9) {
                printf("yanliş file ismi\n");
                return res;
            } 
        }
    }
    printf("bir şeyler ters gitti ve dosya işlemin başarılı olmadı..\n");
    return res;
}

void create_file(char *file_name) { //file liste ekleme ve dosyayı oluşturma
    char response[50];
    char * resp;
    bool kontrol = false;

    if(fileListControl(file_name, 0)) {

        printf("create_file için dosya açılıyor..\n");
        FILE *file = fopen(file_name, "w");
    
        if(file == NULL) {
            resp = " Dosya açılamadı..\n";
            perror("error.\n");
    
        } else {
        
            printf("create_file için dosya açıldı..\n");
            resp = " dosya oluşturuldu..\n";
            kontrol = true;
        }
        fclose(file);

    } else {
        printf("file oluşturulamadı..\n");
        resp = " fosya oluşturulamadı..\n";
    }
    int fd;
    fd = open("file_manager_named_pipe", O_WRONLY);
    if (fd == -1) {
        perror("named_pipe yazma için açılırken hata...\n");
    }
    
    strcpy(response, "Create");
    strcat(response, resp);
    if(kontrol) {   
        strcat(response, "Yaratılan Dosya: ");
        strcat(response, file_name);
    }
    printf("named_pipe yazma için açıldı..\n");
    if(write(fd, response, sizeof(response)) == -1) {
        printf("yazılırken hata..\n");
    }
    printf("Comment sent!\n");
    close(fd);  
} 

void delete_file(char *file_name) { //file listten ve sistemden dosyayı silme
    
    char response[50];
    char * resp;
    bool kontrol = false;
    if(fileListControl(file_name, 1)){
         printf("dosya siliniyor..\n");
        if (remove(file_name) == 0){
            printf("dosya silindi..\n");
            resp = " Dosya silindi.\n";
            kontrol = true;
        } else {
            printf("Unable to delete the file\n");
            resp = " dosyaya erişilemiyor..\n";
        }
    } else {
        printf("file silinemedi..\n");
        resp = " file silinemedi..\n";
    }
    int fd;
    fd = open("file_manager_named_pipe", O_WRONLY);
    if (fd == -1) {
        perror("named_pipe yazma için açılırken hata...\n");
    }
    
    strcpy(response, "Delete ");
    strcat(response, resp);
    if (kontrol) {
        strcat(response, "Silinen Dosya: ");
        strcat(response, file_name);
    }
    
    printf("named_pipe yazma için açıldı..\n");
    if(write(fd, response, sizeof(response)) == -1) {
        printf("yazılırken hata..\n");
    }
    printf("Comment sent!\n");
    close(fd);  
}

void read_file(char *file_name) { //dosyayı okuma

    char data[100];
    char response[50];
    char *resp;
    bool kontrol = false;
    printf("read_file için dosya açılıyor..\n");

    FILE *file = fopen(file_name, "r");
    if(file == NULL) {
        perror("error.");
        resp = " Dosya Açılırken Hata\n";
    }else {
        printf("read_file için dosya açıldı..\n");

        while( fgets ( data, 100, file ) != NULL ) {
            // Print the dataToBeRead
            printf( "%s" , data );
        }
        resp = " Dosyadan Okundu.\n";
        kontrol = true;
        fclose(file);
    }
    int fd;
    fd = open("file_manager_named_pipe", O_WRONLY);
    if (fd == -1) {
        perror("named_pipe yazma için açılırken hata...\n");
        resp = " named_pipe yazma için açılırken hata...\n ";
    }
    
    strcpy(response, "Read");
    strcat(response, resp);
    if (kontrol) {
        strcat(response, "Okunan Data: ");
        strcat(response, data);
    } 
    printf("named_pipe yazma için açıldı..\n");
    if(write(fd, response, sizeof(response)) == -1) {
        printf("yazılırken hata..\n");
    }
    printf("Comment sent!\n");
    close(fd);   
}

void write_file(char *file_name, char * data) { //dosyanın içine veriyi yazma

    bool oDosyaVarMi = false;
    char response[50];
    char * resp;
    bool kontrol = false;
    for (int j = 0; j < 10; j++) { //aynı isimde dosya olup olmadığını kontrol eden döngü
        if (file_list[j] == NULL) {
            continue;
        } else if(strcmp(file_list[j], file_name) == 0) {
            oDosyaVarMi = true;
        }
    }

    if(oDosyaVarMi) {
        printf("write_file için dosya açılıyor..\n");

        FILE *file = fopen(file_name, "a");
        if(file == NULL) {
            perror("error.");
            resp = "Dosya Açma Hatası";
        }else {
            printf("write_file için dosya açıldı..\n");

            if ( strlen ( data ) > 0 ) {
                // writing in the file using fputs()
                printf("write_file data yazılıyor..\n");
                fputs(data, file) ;
                fputs("\n", file) ;
            }
            resp = " Dosyaya Yazıldı.";    
            kontrol = true;
            fclose(file);
        }
            
    } else {
        printf("önce dosyayı oluşturmalısın..");
        resp = "Önce Dosya Oluşturulmalı";
    }
    int fd;
    fd = open("file_manager_named_pipe", O_WRONLY);
    if (fd == -1) {
        perror("named_pipe yazma için açılırken hata...\n");
    }
    strcpy(response, "Write");
    strcat(response, resp);
    if (kontrol) {
        strcat(response, "Yazılan Data: ");
        strcat(response, data);
    }
    
    printf("named_pipe yazma için açıldı..\n");
    if(write(fd, response, sizeof(response)) == -1) {
        printf("yazılırken hata..\n");
    }
    printf("Comment sent!\n");
    close(fd);
}

void namedPipeOlustur() {

    printf("named_pipe oluşuyor..\n");
    if (mkfifo("file_manager_named_pipe", 0777) == -1) { //fifo is created or not.
        if (errno != EEXIST) { // If fifo is not created
            perror("Could not create fifo file\n");
        }
    }
    printf("named_pipe oluştu..\n");
}

void * listen() {

    namedPipeOlustur();
    int fd;

    while(1){
        
        pthread_mutex_lock(&mutex);
        char input[50]; 
        char *words[10];
        int i, j, k;
        for (i = 0; i < 10; i++)
            printf("%d. File: %s\n", (i+1), file_list[i]);
        
        
        printf("named_pipe okuma için açılıyor...\n");
        fd = open("file_manager_named_pipe", O_RDONLY); 
        if (fd == -1) {
            perror("named_pipe okuma için açılırken hata...\n");
            break;
        }

        printf("named_pipe okuma için açıldı..\n");
        if(read(fd, input, sizeof(input)) == -1) {
            perror("okunurken hata..\n");
            break;
        }

        char * token = strtok(input, " "); // strtok fonksiyonu kullanılarak cümle kelimelere ayrılır
        i = 0;
        while (token != NULL) { // kelime NULL olana kadar döngü
            words[i++] = token;
            token = strtok(NULL, " "); // sonraki kelime alınır
        }
        words[i-1] = strtok(words[i-1], "\n");

        if(strcmp(words[0], "Create") == 0) {
            
            create_file(words[1]);

        } else if(strcmp(words[0], "Delete") == 0){

            delete_file(words[1]);

        } else if(strcmp(words[0], "Read") == 0){
            
            read_file(words[1]);

        } else if(strcmp(words[0], "Write") == 0){
            
            write_file(words[1], words[2]);

        } else if(strcmp(words[0], "Exit") == 0){
            
            printf("cikis yapılıyor...");
            break;
        } else {
            printf("yanlis input girdiniz...\n");
        }

        close(fd);

        
        pthread_mutex_unlock(&mutex);
    }
}

int main () {
    
    pthread_mutex_init(&mutex, NULL);
    
    for(int i = 0; i < 5; i++) {
        if(pthread_create(&thread_list[i], NULL, &listen, NULL) != 0) {
            perror("thread oluşturulurken hata...");
        }
    }
    listen();

    for(int i = 0; i < 5; i++) {
        if(pthread_join(thread_list[i], NULL) != 0) {
            perror("Thread join olurken hata..");
        }
    }
    
    pthread_mutex_destroy(&mutex);

    return 0;
}