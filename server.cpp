#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <bits/stdc++.h>
#include <netinet/in.h>
#include <cpuid.h>

#define MAXHOST 1024
#define MAXSERV 32

using namespace std;

static unsigned long long lastTotalUser, lastTotalUserLow, lastTotalSys, lastTotalIdle;

void init(){
    FILE* file = fopen("/proc/stat", "r");
    fscanf(file, "cpu %llu %llu %llu %llu", &lastTotalUser, &lastTotalUserLow,
        &lastTotalSys, &lastTotalIdle);
    fclose(file);
}

double getCPUValue(){
    double percent;
    FILE* file;
    unsigned long long totalUser, totalUserLow, totalSys, totalIdle, total;

    file = fopen("/proc/stat", "r");
    fscanf(file, "cpu %llu %llu %llu %llu", &totalUser, &totalUserLow,
        &totalSys, &totalIdle);
    fclose(file);

    if (totalUser < lastTotalUser || totalUserLow < lastTotalUserLow ||
        totalSys < lastTotalSys || totalIdle < lastTotalIdle){
        //Overflow detection. Just skip this value.
        percent = -1.0;
    }
    else{
        total = (totalUser - lastTotalUser) + (totalUserLow - lastTotalUserLow) +
            (totalSys - lastTotalSys);
        percent = total;
        total += (totalIdle - lastTotalIdle);
        percent /= total;
        percent *= 100;
    }

    lastTotalUser = totalUser;
    lastTotalUserLow = totalUserLow;
    lastTotalSys = totalSys;
    lastTotalIdle = totalIdle;

    return percent;
}

string getCPUName(){
    char line[512];
    FILE *file = popen("cat /proc/cpuinfo | grep 'model name' | head -n 1 | awk -F':' '{print substr($2,2)}'", "r");

    if(file == NULL){
        return NULL;
    }
    fgets(line,512,file);
    fclose(file);
    return line;

}

string getHostName(){
    char line[512];
    FILE *file = fopen("/proc/sys/kernel/hostname", "r");
    if (file == NULL){
        return NULL;
    }
    fgets(line,512,file);
    fclose(file);
    return line;
}

int main(int argc, char *argv[]){
    if(argc != 2){
        cerr << "INVALID PORT" << endl;
        exit(EXIT_FAILURE);
    }
    int port;
    try{
        port = stoi(argv[1]);

    }catch(...){
        cerr << "INVALID PORT" << endl;
        exit(EXIT_FAILURE);
    }
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sockfd == 0){
        cerr << "SOCKET FAILED" << endl;
        exit(EXIT_FAILURE);
    }
    int opt = 1;
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        cerr << "SETSOCKOPT" << endl;
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = INADDR_ANY;

    if(bind(sockfd, (struct sockaddr *)&address ,sizeof(address))<0){
        cerr << "BIND FAILED" << endl;
        exit(EXIT_FAILURE);
    }
        
    if(listen(sockfd,3) < 0){
        cerr << "LISTEN FAILED" << endl;
        exit(EXIT_FAILURE);
    }
    char buffer[4096];
    int new_socket;
    int count = 0;
    char *req_line;
    char arr[1024] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain;\r\n\r\n";
    string hostname = "/hostname";
    string cpu_name = "/cpu-name";
    string load = "/load";
    string result_str;
    while(true){
        new_socket = accept(sockfd, (struct sockaddr *)&address, &addrlen);
        
        read(new_socket, buffer, 4096);
        //Parse socket buffer into tokens
        char *token = strtok(buffer, " ");
        while(count < 2){
            req_line = token;
            token = strtok(NULL, " ");
            count++;
        }
        if(hostname.compare(req_line) == 0){
            result_str = getHostName();
            int res_size = result_str.length();
            char result[res_size + 1];
            strcpy(result, result_str.c_str());
            send(new_socket, arr, strlen(arr), 0);
            send(new_socket, result, strlen(result), 0);
            break;
            
        }
        else if(cpu_name.compare(req_line) == 0){
            result_str = getCPUName();
            int res_size = result_str.length();
            char result[res_size + 1];
            strcpy(result, result_str.c_str());
            send(new_socket, arr, strlen(arr), 0);
            send(new_socket, result, strlen(result), 0);
            break;
            
        }
        else if(load.compare(req_line) == 0){
            double load = getCPUValue();
            stringstream precised;
            precised << setprecision(3) << load;
            string tmp = precised.str();
            char const *result = tmp.c_str();
            send(new_socket, arr, strlen(arr), 0);
            send(new_socket, result, strlen(result), 0);
            send(new_socket, "%\r\n", strlen("%\r\n"), 0);
            break;
        }
        else{
            send(new_socket, arr, strlen(arr), 0);
            send(new_socket, "400 Bad Request\r\n", strlen("400 Bad Request\r\n"), 0);
            // close(new_socket);
            break;
        }
    }
    close(new_socket);
    close(sockfd);
    return 0;
}