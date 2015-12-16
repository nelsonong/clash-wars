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

// Checks to see if client threads are still active.
void timeoutCheck(Communication::Socket* client1,int timeout, bool *killVar);

int main(){
    bool kill = false; // Used to terminate all active threads
    std::string killResponse; // Used to initate termination process

    // The main thread only spawns one thread that manages all connections to
    // the server by dynamically spawning "response threads"
    std::thread *connection = new std::thread(connectionThread, &kill);

    do{
        std::cout<<"Type 'kill' to close server: ";
        std::cin >> killResponse;
        if(killResponse == "kill"){
            kill = true;
            break;
        }
    } while(true);

    // Wait for the connection thread
    connection->join();
    delete connection;
    return 0;
}

void GameThread(Communication::Socket *client1, Communication::Socket *client2, bool *kill) {
    std::vector<std::thread *> clientThreads;
    int *tracker = new int(50);
    const int MAX_TRACKER = 100;
    const int MIN_TRACKER = 0;
    const int UPDATE_TIME = 250;
    bool *killClient = new bool(false);
    std::mutex *tapMutex = new std::mutex();

    // Create client threads to manage communication with each client
    clientThreads.push_back(new std::thread(clientThread, 1, client1, tracker, killClient, tapMutex));
    clientThreads.push_back(new std::thread(clientThread, 2, client2, tracker, killClient, tapMutex));

    // Keep sending client the current value of the tracker based on UPDATE_TIME
    while(!*kill){
        if(*tracker <= MIN_TRACKER){
            // Case where client1 loses
            tapMutex->lock();
            client1->Write(Communication::ByteArray("998\n"));
            client2->Write(Communication::ByteArray("999\n"));
            tapMutex->unlock();
            break;
        }
        else if(*tracker >= MAX_TRACKER){
            // Case where client2 loses
            tapMutex->lock();
            client2->Write(Communication::ByteArray("998\n"));
            client1->Write(Communication::ByteArray("999\n"));
            tapMutex->unlock();
            break;
        }
        else{
            // Case where the game is still in progress
            tapMutex->lock();
            client1->Write(Communication::ByteArray(std::to_string(*tracker)+"\n"));
            client2->Write(Communication::ByteArray(std::to_string(MAX_TRACKER - *tracker)+"\n"));
            tapMutex->unlock();
        }

        // Sleep the thread before next check of the tracker
        std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_TIME));
    }

    // Signal all client threads to shutdown
    *killClient = true;

    // Join all response threads in rThreads
    for(auto&& thread : clientThreads)
        thread->join();

     // Clean up allocated memory
     while(!clientThreads.empty()){
         delete clientThreads.back();
         clientThreads.pop_back();
     }

     // Game termination message sent to each client in case the game has ended on server but not on client
     tapMutex->lock();
     client2->Write(Communication::ByteArray("-1\n"));
     client1->Write(Communication::ByteArray("-1\n"));
     tapMutex->unlock();
}

void clientThread(int clientNo, Communication::Socket *client, int *tracker, bool *killClient, std::mutex *tapMutex){
    int response;
    const int INCREMENT = 2; // Value to move the tracker by each tap
    Communication::ByteArray message("");

    // Create a shutdown thread to terminate a possible block with Read(message)
    std::thread shutdown_thread(triggerClientShutdown, killClient, client);

    while(!*killClient){
        try{
            // Block in case client leaves mid game (Stops thread from proceeding
            // to Read(message))
        	while(!(client->open) && !*killClient){ // Checks for termination
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
            }

            // Attempt to retrieve message from client
            client->Read(message);

        } catch(std::exception e){
            // Break loop in case of error
            break;
        }

        // Update tracker based on client and repeat loop until an exit condition is reached
        if(clientNo == 1){
            tapMutex->lock();
            *tracker += INCREMENT;
            tapMutex->unlock();
        }
        else if(clientNo == 2){
            tapMutex->lock();
            *tracker -= INCREMENT;
            tapMutex->unlock();
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
                continue; // Try again without creating a game thread
        } catch(std::string ex){
            continue; // Try again without creating a game thread
        }

        if(openSockets.size() >= 2){ // Check if queue has enough to start a game

            // First each of the clients must be tested incase one or both has left the queue
            Communication::ByteArray test("");
            Communication::Socket *client1 = openSockets.front();
            openSockets.pop_front();

            bool *test_kill = new bool(false); // Used to test if client is still connected
            std::thread * check_thread = new std::thread(timeoutCheck, client1, 3000, test_kill);
            client1->Write(Communication::ByteArray("200\n")); // Messages client1 (Looking for heartbeat)
            if (client1->Read(test) == 0) {
                continue;       // If client terminates before game starts, look for another client.
            }
            *test_kill = true; // If client responds before the timeoutCheck thread expires, the check is passed

            // Repeat for client 2
            Communication::Socket *client2 = openSockets.front();
            openSockets.pop_front();
            *test_kill = false;
            std::thread * check_thread2 = new std::thread(timeoutCheck, client2, 3000, test_kill);
            client2->Write(Communication::ByteArray("200\n")); // Messages client2 (Looking for heartbeat)
            if (client2->Read(test) == 0) {
                openSockets.push_front(client1);    // If 2nd client terminates before game starts, add 1st client back to deque.
                continue;                           // Continue looking for another client.
            }
            *test_kill = true;

            // Join all threads then delete them
            check_thread->join();
            check_thread2->join();
            delete check_thread;
            delete check_thread2;

            // Clean up other memory components
            delete test_kill;

            // After both clients have passed the check, spawn the Game Thread
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

    // Close socket
    c->Close();
}

void timeoutCheck(Communication::Socket* client1,int timeout, bool *killVar){
    // Wait for a specified timeout period before closing communication with client
    // Used to verify client heartbeat incase client has disconnected while in queue
	std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
	if(*killVar == false){
        // Close socket
		client1->Close();
    }
}
