## clash-wars
Multi-player tapping game for Android created using LibGDX.

### Introduction
This is a multi-player tapping game for Android, where players use Dragon Ball Z characters to face off against their opponents in a reverse tug-of-war-style match determined by tapping. The winner is the player with the faster tapping speed or the player who is ahead when the countdown timer reaches zero. Each client character sprite has a beam emanating from their hands and clashes with the opponent’s beam in the center. Every time a tap is registered from a client, the beam is pushed slightly toward the opponent. This continues until one player’s beam reaches their opponent’s character sprite.

###### Gameplay

<img src="https://i.imgur.com/LuK1RgL.png" alt="drawing" width="475px" />

The client is implemented with Java (+ the LibGDX framework), and runs on Android devices. The server is written in C++ and runs in a UNIX environment, hosted on Amazon Web Services.

### Implementation
The implementation uses seven types of threads: `MainThread`, `connectionThread`, `timeoutCheck`, `triggerShutdown`, `GameThread`, `clientThread`, `triggerClientShutdown`. The flow of how each thread is created is outlined in the flowchart at the bottom of this page.

The initial thread, `MainThread`, spawns a new thread called `connectionThread` and then waits on user input to signify that it should shut down. If, at anytime, the user sends a kill signal to the `MainThread`, the thread handles the graceful termination of itself and the rest of the threads that are created during the lifecycle of the program.

###### Main Thread
```cpp
int main() {
    bool kill = false;          // Used to terminate all active threads
    std::string killResponse;   // Used to initate termination process

    // The main thread only spawns one thread that manages all connections to
    // the server by dynamically spawning "response threads"
    std::thread *connection = new std::thread(connectionThread, &kill);

    do {
        std::cout << "Type 'kill' to close server: ";
        std::cin >> killResponse;
        if (killResponse == "kill") {
            kill = true;
            break;
        }
    } while (true);

    // Wait for the connection thread
    connection->join();
    delete connection;
    return 0;
}
```

Each `GameThread` on the server accesses a tracker variable, shared amongst the two `connectionThread`s. This tracker variable keeps track of the percentage from one client’s perspective and is updated after each tap. The percentage is sent back to the connected clients through the 2 stored sockets which triggers a game re-render.

Since only one thread can access the tracker variable at once, a mutex is used for synchronization. The two `clientThread`s wait for the mutex to be unlocked before updating the tracker variable.

The `connectionThread` handles new connections from clients as they attempt to establish communication with the server. Each new client gets pushed onto a deque. Once, at least, two clients have been added to this deque, the `connectionThread` creates a `GameThread`. Before it actually spawns the thread, it does a heartbeat check on the two clients that are passed to the `GameThread` to ensure that an active connection is still present. The heartbeat check is implemented by spawning `timeoutCheck` threads for each client that function in similarly to a Linux “watchdog”. That is, they consist of a timer that must be stopped. Otherwise, it closes the socket to the client that fails to provide a response to the heartbeat check.

###### Heartbeat Timeout Check
```cpp
void timeoutCheck(Communication::Socket* client1, int timeout, bool *killVar) {
    // Wait for a specified timeout period before closing communication with client
    // Used to verify client heartbeat incase client has disconnected while in queue
    std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
    if (*killVar == false) {
        // Close socket
        client1->Close();
    }
}
```

After the server verifies both client’s heartbeats, it spawns a `GameThread` to handle communication with the two clients. To facilitate communication with both clients, the `GameThread` spawns two `clientThread`s to read from each of the client sockets and each `clientThread` spawns a `triggerClientShutdown` thread to respond to the kill trigger from the `GameThread`.

The `GameThread` keeps track of the game by using a tracker variable that both the `clientThread`s modify during the course of the game. This tracker variable is protected using a mutex and is used to track the position of the beam (shared resources). Once the tracker variable reaches a specified minimum or maximum value, the game is considered finished and a win or loss code is sent back to each client and the `GameThread` terminates after joining all threads it spawned.

When a client disconnects from the server, the `clientThread` checks for a dead socket right before it attempts to block on the read. This allows the game to continue as if the disconnected player was not playing.

###### Handling Client Disconnection Mid-game
```cpp
while (!*killClient) {
    try {
        // Block in case client leaves mid game (Stops thread from proceeding
        // to Read(message))
        while (!(client->open) && !*killClient) {   // Checks for termination
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }

        // Attempt to retrieve message from client
        client->Read(message);

    } catch (std::exception e) {
        // Break loop in case of error
        break;
    }
    ...
}
```

The flowchart below shows all the threads that compose the server. Red fields designate shutdown threads, yellow correspond to threads responsible for checking client heartbeats and blue are logic threads that do various functions.

###### Server Concurrency Flowchart

<img src="https://i.imgur.com/63ZFix6.png" alt="drawing" width="475px" />

### Configuration

The client can be modified to communicate with a server by changing the Connection.java file: 

###### Connection Configuration
```cpp
public class Connection {
    // Server variables
    public static final String SERVER = "XX.XXX.XXX.X";
    public static final int PORT = 2200;
    ...
}
```
