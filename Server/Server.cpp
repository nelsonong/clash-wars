#include "socketserver.h"
#include "socket.h"
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <exception>
#include <mutex>
#include <algorithm>
#include <deque>

int PORT = 2200; // Port to listen for active connections

// Function used to respond to a specific client
void GameThread(Communication::Socket *client1 , Communication::Socket *client2, bool *kill);

// Function used to spawn threads that run GameThread()
void connectionThread(bool* kill);

// Function used to trigger the SocketServer to shutdown
void triggerShutdown(bool *kill, Communication::SocketServer *c);

// Handles regular Socket shutdown.
void triggerClientShutdown(bool *kill, Communication::Socket *c);

// Client handler.
void clientThread(int clientNo, Communication::Socket *client, int *tracker, bool *killClient, std::mutex *tapMutex);

int main(){
    bool kill = false; // Used to terminate all active threads
    std::string killResponse; // Used to initate termination process

    // The main thread only spawns one thread that manages all connections to
    // the server by dynamically spawning "response threads"
    std::thread *connection = new std::thread(connectionThread, &kill);

    do{
        std::cout<<"Do you want to close the server?\n";
        std::cin >> killResponse;
        if(killResponse == "yes"){
            kill = true;
            break;
        }
    } while(true);

    connection->join();
    delete connection;
    return 0;
}

void GameThread(Communication::Socket *client1, Communication::Socket *client2, bool *kill) {
    std::cout << "game thread created\n";
    std::vector<std::thread *> clientThreads;
    int *tracker = new int(50);
    const int MAX_TRACKER = 100;
    const int MIN_TRACKER = 0;
    const int UPDATE_TIME = 250;
    bool *killClient = new bool(false);
    std::mutex *tapMutex = new std::mutex();


    clientThreads.push_back(new std::thread(clientThread, 1, client1, tracker, killClient, tapMutex));
    clientThreads.push_back(new std::thread(clientThread, 2, client2, tracker, killClient, tapMutex));

    while(!*kill){
        if(*tracker <= MIN_TRACKER){
            tapMutex->lock();
            client1->Write(Communication::ByteArray("998\n"));
            client2->Write(Communication::ByteArray("999\n"));
            tapMutex->unlock();
            break;
        }
        else if(*tracker >= MAX_TRACKER){
            tapMutex->lock();
            client2->Write(Communication::ByteArray("998\n"));
            client1->Write(Communication::ByteArray("999\n"));
            tapMutex->unlock();
            break;
        }
        else{
            tapMutex->lock();
            client1->Write(Communication::ByteArray(std::to_string(*tracker)+"\n"));
            client2->Write(Communication::ByteArray(std::to_string(MAX_TRACKER - *tracker)+"\n"));
            tapMutex->unlock();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_TIME));
    }
    *killClient = true;

    // Join all response threads in rThreads
    for(auto&& thread : clientThreads)
        thread->join();

     // Clean up allocated memory
     while(!clientThreads.empty()){
         delete clientThreads.back();
         clientThreads.pop_back();
     }

    client1->Close();
    client2->Close();
}

void clientThread(int clientNo, Communication::Socket *client, int *tracker, bool *killClient, std::mutex *tapMutex){
    int response;
    const int INCREMENT = 4;
    Communication::ByteArray message("");

    // Create a shutdown thread to terminate a possible block with Accept()
    std::thread shutdown_thread(triggerClientShutdown, killClient, client);

    while(!*killClient){
        try{
            client->Read(message);
            std::cout<< message.ToString();
        } catch(std::exception e){
            if(clientNo == 1){
                tapMutex->lock();
                *tracker = 0;
                tapMutex->unlock();
            } else {
                tapMutex->lock();
                *tracker = 100;
                tapMutex->unlock();
            }

            *killClient = true;
            break;
        }

        if(clientNo == 1){
            tapMutex->lock();
            *tracker += INCREMENT;
            tapMutex->unlock();
            std::cout<< "c1 INCREMENT\n";
        }
        else if(clientNo == 2){
            tapMutex->lock();
            *tracker -= INCREMENT;
            tapMutex->unlock();
            std::cout<< "c2 INCREMENT\n";
        }
    }
    // Join shutdown thread
    shutdown_thread.join();
}

void connectionThread(bool* kill){
    std::vector<std::thread *> rThreads; // Currently active response threads
    std::deque<Communication::Socket *> openSockets; //Sockets currently waiting.
    // Start listening for incoming connections on specified port
    Communication::SocketServer connectionSocket(PORT);

    // Create a shutdown thread to terminate a possible block with Accept()
    std::thread shutdown_thread(triggerShutdown, kill, &connectionSocket);

    while(true){ // Manage incoming connections loop
        // Create a temporary Communication::Socket to pass to GameThread
        Communication::Socket *socket;

        try{
            // Blocks until a connection is attempted
            socket = new Communication::Socket(connectionSocket.Accept());
            openSockets.push_back(socket);

        } catch(int e) {
            if(e == 2) // Catch TerminationException
                break; // Exit loop
            else
                continue; // Try again without creating new response thread
        } catch(std::string ex){
            continue;
        }

        if(openSockets.size() >= 2){
            std::cout << "Found two clients\n";
            // Create a game thread when there are more than two clients waiting.
            // Assign the first two sockets to local variables and remove them
            // from the deque.
            Communication::Socket *client1 = openSockets.front();
            openSockets.pop_front();
            bool kill = false;
            std::thread * check_thread = new std::thread(timeoutCheck, client1, 3000, kill);
            std::cout << "client 1 write\n";
            client1->Write(Communication::ByteArray("200\n")); //Messages client1.
            if (client1->Read() == 0) {
                continue;       //If client terminates before game starts, look for another client.
            }
            kill = true;
        
            Communication::Socket *client2 = openSockets.front();
            openSockets.pop_front();
            bool kill2 = false;
            std::thread * check_thread2 = new std::thread(timeoutCheck, client2, 3000, kill2);
            std::cout << "client 2 write\n";
            client2->Write(Communication::ByteArray("200\n")); //Messages client2.
            if (client2->Read() == 0) {
                openSockets.push_front(client1);    //If 2nd client terminates before game starts, add 1st client back to deque.
                continue;                           // Continue looking for another client.
            }
            kill2 = true;
            std::thread *new_thread = new std::thread(GameThread, client1, client2, kill);
            rThreads.push_back(new_thread);
        }
    }

    // Join shutdown thread
    shutdown_thread.join();

    // Join all response threads in rThreads
    for(auto&& thread : rThreads)
        thread->join();

    // Clean up allocated memory
    while(!rThreads.empty()){
        delete rThreads.back();
        rThreads.pop_back();
    }

    std::cout << "Closed Connection. Exiting...\n";
}

void triggerShutdown(bool *kill, Communication::SocketServer *c){
    int refresh_interval = 500; // refresh interval to check for termination
    while(!(*kill)) // Checks for termination
        std::this_thread::sleep_for(std::chrono::milliseconds(refresh_interval));

    // Shutdown SocketServer
    c->Shutdown();
}

void triggerClientShutdown(bool *kill, Communication::Socket *c){
    int refresh_interval = 500; // refresh interval to check for termination
    while(!(*kill)) // Checks for termination
        std::this_thread::sleep_for(std::chrono::milliseconds(refresh_interval));

    // Shutdown SocketServer
    c->Close();
}
