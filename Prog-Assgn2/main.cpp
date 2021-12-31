#include<iostream>
#include<sys/socket.h>
#include<fstream>
#include<stdlib.h>
#include <unistd.h>
#include<pthread.h>
#include<sys/time.h>
#include <algorithm>
#include<string.h>
#include<vector>
#include<math.h>
#include <errno.h>
#include <arpa/inet.h>  
#include <sys/types.h>
#include <netinet/in.h>
#include <signal.h>
using namespace std; 
#define TRUE 1  
#define FALSE 0
#define PORT 8000

pthread_mutex_t output_lock, sock_lock;
ofstream output;
default_random_engine generator;

string convertTime(time_t epoch_time){
	tm *t = localtime(&epoch_time);
	string ct = to_string(t->tm_hour)+":"+to_string(t->tm_min)+":"+to_string(t->tm_sec);
	return ct;
}


struct arg_struct_main {
    // arg_struct_main() = default;
    int Processes;
    int color; //0 - white, 1 - red, 2 - blue
    int pid;
    int peers[3];
    double exp_mean_send;
    double exp_mean_reply;
    double exp_mean_request;
    double exp_mean_drift;
};

struct arg_struct_client {
    // arg_struct_client() = default;
    int myPid;
    int Processes;
    int* color;
    int peers[3];
    // local_clock Cl;
    double exp_mean_send;
    // double exp_mean_request;
};

struct arg_struct_server {
    // arg_struct_server() = default;
    int myPid;
    int peers[3];
    int Processes;
    int* color;
    // local_clock Cl;
    // double exp_mean_send;
    // double exp_mean_reply;
};

// struct arg_struct_drift {
//     // arg_struct_drift() = default;
//     int exit_call;
//     local_clock Cl;
//     double exp_mean_drift;
// };

// void sync_request(local_clock localClock, int sendPid) {
//     T1 = localClock.read();
//     T2, T3 = request(pid);
//     T4 = localClock.read();
//     optimal_delta = ;
//     localClock.update(optimal_delta);
//     nanosleep();
// }

void sync_reply(local_clock localClock, int sendPid, exponential_distribution<double> reply_dist, exponential_distribution<double> send_dist) {
    long long int T2 = localClock.read();
    struct timespec temp = {0,long(reply_dist(generator))};
    nanosleep(&temp, NULL);
    long long int T3 = localClock.read();
    char buffer1[64];
    // memset(buffer, 0, sizeof(buffer));
    string buffer = to_string(T2).append(",").append(to_string(T3));
    strcpy(buffer1,buffer.c_str());
    temp = {0,long(send_dist(generator))};
    nanosleep(&temp, NULL);
    send(sendPid,buffer1,sizeof(buffer),0);
    // reply(pid, T2, T3);
}

// long long int computeOffset(long long int T1, long long int T2, long long int T3, long long int T4) {
//     double offset = T2+T3-T1-T4;
//     offset/=2;
//     return offset;
// }

void* clientThread(void* argst) {
    struct arg_struct_client *args = (struct arg_struct_client*)argst;
    int numProcesses = args->Processes;
    // int numRounds = args->rounds;
    int myId = args->myPid;
    int p[3] = args->peers;
    local_clock Clock = args->Cl;
    vector<int> connections;
    struct sockaddr_in servaddr;
    int n, len;
    int sockfd;
    int ports[3], Ids[3] = , cross = 0;
    for(int i=0;i<numProcesses;i++) {
        ports[i] = 0;
    }
    exponential_distribution<double> send_distribution(args->exp_mean_send);
    exponential_distribution<double> request_distribution(args->exp_mean_request);
    int i = 0;
    while(connections.size()<4) {
        if(ports[i] == 0) {
            pthread_mutex_lock(&sock_lock);
            if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                printf("socket creation failed");
                sleep(1);
                // exit(0);
            }
            else {
                memset(&servaddr, 0, sizeof(servaddr));
                servaddr.sin_family = AF_INET;
                servaddr.sin_port = htons(PORT+p[i]);
                servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
                if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
                    // printf("\n Error : Connect Failed \n");
                }
                else {
                    ports[i] = -1;
                    connections.push_back(sockfd);
                }
            }
            pthread_mutex_unlock(&sock_lock);
        }
        if(i==numProcesses-1) {i = 0;}
        else {i++;}
    }
    sleep(3);
    // cout<<args->myPid<<":All connected";
    struct timespec temp;
    // int tot = numRounds;
    struct timeval outTime;
    while(tot--) {
        for(int i=0;i<numProcesses;i++) {
            char buffer[64];
            char sendBuffer[64];
            memset(sendBuffer, 0, sizeof(buffer));
            strcpy(sendBuffer, "Syncrequest");
            long long int T1 = Clock.read();
            temp = {0,long(send_distribution(generator))};
            nanosleep(&temp, NULL);
            send(connections[i],sendBuffer,sizeof(buffer),0);
            // gettimeofday(&outTime,NULL);
            // pthread_mutex_lock(&output_lock);
            // output<<"Server"<<myId+1<<" requests "<<numRounds-tot<<" Clock synchronization request to Server"<<Ids[i]<<" at "<<convertTime(outTime.tv_sec)<<"\n";
            // pthread_mutex_unlock(&output_lock);
            // int len = recv(connections[i], buffer, sizeof(buffer),0);
            // gettimeofday(&outTime,NULL);
            // pthread_mutex_lock(&output_lock);
            // output<<"Server"<<myId+1<<" receives "<<numRounds-tot<<" Clock synchronization response from Server"<<Ids[i]<<" at "<<convertTime(outTime.tv_sec)<<"\n";
            // pthread_mutex_unlock(&output_lock);
            // long long int T4 = Clock.read();
            // buffer[len] = '\0';
            // string buf(buffer);
            // cout<<buf.substr(0,buf.find(","))<<"\n";
            // long long int T2 = stoll(buf.substr(0,buf.find(",")),nullptr,10);
            // cout<<buf.substr(buf.find(",")+1,len-1-buf.find(","))<<"\n";
            // long long int T3 = stoll(buf.substr(buf.find(",")+1,len-1-buf.find(",")),nullptr,10);
            // pthread_mutex_lock(&output_lock);
            // output<<"Computing "<<numRounds-tot<<" delta between server"<<myId+1<<" and server"<<Ids[i]<<"\n";
            // pthread_mutex_unlock(&output_lock);
            // long long int theta = computeOffset(T1, T2, T3, T4);
            // Clock.update(theta);
            // pthread_mutex_lock(&output_lock);
            // output<<"Updating "<<numRounds-tot<<" delta between server"<<myId+1<<" and server"<<Ids[i]<<"\n";
            // pthread_mutex_unlock(&output_lock);
            // output<<args->myPid<<":"<<numRounds<<":"<<Clock.read() <<"\n";
            // temp = {0,long(request_distribution(generator))};
            nanosleep(&temp, NULL);
        }
        // nanosleep();
    }
    // pthread_mutex_lock(&output_lock);
    // output<<Clock.read() <<" ";
    // pthread_mutex_unlock(&output_lock);
    for(i=0;i<connections.size();i++) {
        close(connections[i]);
    }
}

void* ServerThread(void* argst) {
    struct arg_struct_server *args = (struct arg_struct_server*)argst;
    int numProcesses = args->Processes;
    int PORT_ = PORT + args->myPid;
    int opt = TRUE;
    int master_socket, addrlen, new_socket, client_socket[100], max_clients = 100, activity, i, valread, sd;
    int max_sd, numConns = 0, numDisConns = 0;
    struct sockaddr_in address;
    int parent_fd;
    fd_set readfds;
    for(i = 0; i < max_clients; i++) {
        client_socket[i] = 0;   
    }
    if((master_socket = socket(AF_INET , SOCK_STREAM , 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    if(setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0 ) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT_ );
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if(listen(master_socket, 3)<0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", PORT_);
    addrlen = sizeof(address);
    puts("Waiting for connections ...");
    while(TRUE) {
        FD_ZERO(&readfds);
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;
        for (i=0;i<max_clients;i++) {
            sd = client_socket[i];
            if(sd > 0)
                FD_SET(sd,&readfds);
            if(sd > max_sd)
                max_sd = sd;
        }
        activity = select(max_sd+1,&readfds, NULL, NULL, NULL);
        if ((activity < 0) && (errno!=EINTR)) {
            printf("select error");
        }
        if (FD_ISSET(master_socket, &readfds)) {
            // pthread_mutex_lock(&sock_lock);
            if ((new_socket = accept(master_socket,(struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            printf("%d: New connection\n", args->myPid);
            numConns++;
            if(address.sin_port == PORT + arg->parent) {
                parent_fd = new_socket;
            }
            for(i = 0; i < max_clients; i++) {
                if( client_socket[i] == 0 ) {
                    client_socket[i] = new_socket;
                    // printf("Adding to list of sockets as %d\n" , i);
                    break;
                }
            }
        }
        if(numConns==4) {
            char buffer[1025];
            int exit_cond = 0;
            
            for(i = 0; i < max_clients; i++) {
                sd = client_socket[i];
                if (FD_ISSET( sd , &readfds)) {
                    if ((valread = read( sd , buffer, 1024)) == 0) {
                        getpeername(sd , (struct sockaddr*)&address ,(socklen_t*)&addrlen);
                        printf("Host disconnected , ip %s , port %d \n" ,inet_ntoa(address.sin_addr) , ntohs(address.sin_port));   
                        close(sd);
                        client_socket[i] = 0;
                        numDisConns++;
                        // numProcesses--;
                        if(numDisConns==numProcesses-1) {exit_cond = 1;}
                        // return (void*)0;
                    }
                    else {
                        buffer[valread] = '\0';
                        if(buffer[0] == "M") {
                            if(args->color == 1 && buffer[0] == "2") {
                                args->color = 2;
                                send_token(parent_fd,)
                            }
                            if(args->color == 2 && buffer[0] == "1") {
                                args->color = 1;
                            }
                        }
                        sync_reply(Clock, sd, reply_distribution, send_distribution);
                    }
                }
            }
            if(exit_cond == 1) break;
        }

}

// void* driftThread(void* arg_struct) {
//     struct arg_struct_drift *args = (struct arg_struct_drift*)arg_struct;
//     exponential_distribution<double> drift_distribution(args->exp_mean_drift);
//     while(args->exit_call==0) {
//         long long int clock_drift = long(drift_distribution(generator));
//         args->Cl.IncrementDriftFactor(clock_drift);
//     }
// }

void* StartProcess(void* arg_struct) {
    struct arg_struct_main *args = (struct arg_struct_main*)arg_struct;
    int numProcesses = args->Processes;
    int myPid = args->pid;
    int color = 0;
    struct arg_struct_client client_arg;
    client_arg.myPid = myPid;
    client_arg.Processes = numProcesses;
    client_arg.color = color;
    client_arg.exp_mean_send = args->exp_mean_send;
    struct arg_struct_server server_arg;
    server_arg.myPid = myPid;
    server_arg.Processes = numProcesses;
    server_arg.color = color;

    pthread_t tidServer, tidClient;

    pthread_create(&tidClient,NULL,clientThread,(void*)&client_arg);
    pthread_create(&tidServer,NULL,ServerThread,(void*)&server_arg);
    pthread_join(tidClient,NULL);
    pthread_join(tidServer,NULL);

}


int main() {
    int N,K;
    double lp,lq,lsnd,ldrift;
    pthread_mutex_init(&output_lock, NULL);
    pthread_mutex_init(&sock_lock, NULL);
    output.open("out-log.txt");
    ifstream input;
    input.open("inp-params.txt");
    input>>N;
    input>>K;
    input>>lp;
    input>>lq;
    input>>lsnd;
    input>>ldrift;
    // for(int i=0;i<N;i++) {
    //     int temp;
    //     input>>temp;

    // }
    int nums[N];
    pthread_t tids[N];
    struct arg_struct_main args_main[N];
    for(int i=0;i<N;i++) {
        args_main[i].color = 0;
        args_main[i].Processes=N;
        args_main[i].pid=i+1;
        args_main[i].exp_mean_drift = ldrift;
        args_main[i].exp_mean_request = lp;
        args_main[i].exp_mean_reply = lq;
        args_main[i].exp_mean_send = lsnd;
        args_main[i].children = children[i];
        args_main[i].Numchildren = Nchileren[i];
        args_main[i].parent = parent[i];
        num[i] = i;
    }
    for(int i=0;i<N;i++) {
        pthread_create(&tids[i],NULL,StartProcess,(void *)&args_main[i]);
    }
    while(!termination) {
        random_shuffle(nums.begin(),nums.end());
        int randNumred = rand()%(N/10) + 1;
        for(int i=0;i<randNumred;i++) {
            args_main[nums[i]].color = 2;
        }
        random_shuffle(nums.begin(),nums.end());
        int randNumblue = rand()%(N/10 - randNumred + 1) + randNumred;
        for(int i=0;i<randNumlblue;i++) {
            args_main[nums[i]].color = 1;
        }
    }
    for(int i=0;i<N;i++) {
        pthread_join(tids[i],NULL);
    }
    output.close();
    return 0;
}