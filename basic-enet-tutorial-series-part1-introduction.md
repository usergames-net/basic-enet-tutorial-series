[home](./index.md) | [next](./basic-enet-tutorial-series-part2-chatapp.md)

# ENet Tutorial Series!

![alt text](./images/thumbnail.png "Usergames' Enet Tutorial Series")

[![Basic ENet Tutorial Series 1/3 - Server/Client Setup](./images/Basic_ENet_Tutorial_Series_1.webp)](https://www.youtube.com/embed/FxrKS_1zE9s)

## Welcome To ENet

Creating networked applications is very interesting indeed.

*"ENet's purpose is to provide a relatively thin, simple and robust network communication layer on top of UDP (User Datagram Protocol). The primary feature it provides is optional reliable, in-order delivery of packets." — [enet.bespin.org](http://enet.bespin.org/Tutorial.html)*

> ℹ️ **Please Note!** 
> 
> A chunk of the code and directives here has taken directly from the [ENet website](http://enet.bespin.org/Tutorial.html) itself, we suggest checking it out! It's a good basis before getting into creating more advanced things such as a multiplayer game.

## Setting Up

The first step to our grand ENet adventure is to install the darn thing. The process will different depending if you are on Windows, Mac or Linux.

### Microsoft Windows

ENet can be downloaded [here](enet.bespin.org/download/enet-1.3.14.tar.gz) or on [github](https://github.com/lsalzman/enet). You may simply use the included enet.lib or enet64.lib static libraries. However, if you wish to build the library yourself, then the following instructions apply.

There is an included MSVC 6 project (enet.dsp) which you may use to build a suitable library file. Alternatively, you may simply drag all the ENet source files into your main project.

You will have to link to the Winsock2 libraries, so make sure to add ws2_32.lib and winmm.lib to your library list (Project Settings | Link | Object/library modules).

Load the included enet.dsp. MSVC may ask you to convert it if you are on a newer version of MSVC - just allow the conversion and save the resulting project as "enet" or similar. After you build this project, it will output an "enet.lib" file to either the "Debug/" or "Release/" directory, depending on which configuration you have selected to build. By default, it should produce "Debug/enet.lib".

You may then copy the resulting "enet.lib" file and the header files found in the "include/" directory to your other projects and add it to their library lists. Make sure to also link against "ws2_32.lib" and "winmm.lib" as described above.

### Unix-like Operating Systems

If you are using an ENet release, then you should simply be able to build it by doing the following:

```console
./configure && make && make install
```

If you obtained the package from github, you must have automake and autoconf available to generate the build system first by doing the following command before using the above mentioned build procedure

```autoreconf -vfi```

### Linux

Linux makes things quite simple; write one the following commands based on your distribution.

#### Debian-based

```console
foo@bar:~# apt install enet
```

#### Arch-based

```console
btw@arch:~# pacman -S enet
```

## Code

### ./client/main.c

```cpp
/* ./client/main.c */

#include <stdio.h>
#include <enet/enet.h>

int main(int argc, char ** argv)
{
	if(enet_initialize() != 0)
	{
		fprintf(stderr, "An error occurred while initializing ENet!\n");
		return EXIT_FAILURE;
	}
	atexit(enet_deinitialize);
```

So, we've got our first two self-explanatory lines of code here. `enet_initialize();` returns an error code if things are to not initialize correctly, returning 0 if everything went well. As such, we check if the function doesn't return 0 and if it does we handle the error.

Next, we use `atexit(enet_deinitialize);` to setup proper deinitialization of enet to occur at the exit of the program. So far, so good.

Now we will create a client using `enet_host_create()`.

```cpp
ENetHost* client;
client = enet_host_create(NULL, 1, 1, 0, 0);

if(client == NULL)
{
	fprintf(stderr, "An error occurred while trying to create an ENet client host!\n");
	return EXIT_FAILURE;
}
```

Both clients and servers are constructed with `enet_host_create()`. When no address is specified to bind the host to, it will be a client. Otherwise it is a server.

Five parameters can be specified, in order they are:

1. `ENetAddress* address`
2. `size_t peerCount`
3. `size_t channelLimit`
4. `enet_uint32 incomingBandwidth`
5. `enet_uint32 outgoingBandwidth`

So, in my example here, we are creating the client that will connect to one peer (the server), that has channel limit of 1, and has both unlimited incoming and outgoing bandwidth.

```cpp
ENetAddress address;
ENetEvent event;
ENetPeer* peer;

enet_address_set_host(&address, "127.0.0.1");
address.port = 7777;

peer = enet_host_connect(client, &address, 1, 0);
if(peer == NULL)
{
	fprintf(stderr, "No available peers for initiating an ENet connection!\n");
	return EXIT_FAILURE;
}
```

Here we create various variables; `ENetAddress` to hold all ip/port data, `ENetEvent` to handle events, `ENetPeer` to hold the server within. Peers can be either servers or clients. A server hold a list of connected peers.

We setup the address using the `enet_address_set_host()` function with a reference to the address variable as the first paramater. The second being the ip of the desired server to connect to.

Afterwards, we use `enet_host_connect` to both connect to the server and set our peer accordingly. The parameters are as follows:

* `ENetHost* host`
* `const ENetAddress* address`
* `size_t channelCount`
* `enet_uint32 data`

```cpp
if(enet_host_service(client, &event, 5000) > 0 &&
	 event.type == ENET_EVENT_TYPE_CONNECT)
{
	puts("Connection to 127.0.0.1:7777 succeeded.");
}
else
{
	enet_peer_reset(peer);
	puts("Connection to 127.0.0.1:7777 failed.");
	return EXIT_SUCCESS;
}
```

To check for any events the server might be sending us, we use `enet_host_service` with the following paramaters:

* `ENetHost* host`
* `ENetEvent* event`
* `enet_uint32 timeout`

Events will be stored in our `ENetEvent event;` variable. Here we check if we received any events from the server and if so and if the event is of type `ENET_EVENT_TYPE_CONNECT` we print out the fact that we've managed to connect successfully to the server. Otherwise we reset our peer with `enet_peer_reset` print an error message and return the program. Because this is not a crash, I return 0 but I would suggest to simply return to a previous menu of the application instead of closing it altogether. 

```cpp
// [...Game Loop...]

while(enet_host_service(client, &event, 1000) > 0)
{
	switch(event.type)
	{
		case ENET_EVENT_TYPE_RECEIVE:
			printf ("A packet of length %u containing %s was received from %x:%u on channel %u.\n",
			event.packet -> dataLength,
			event.packet -> data,
			event.peer -> address.host,
			event.peer -> address.port,
			event.channelID);
			break;
	}
}
```

Somewhere within the game loop, you'll want to write this chunk of code. We're once again using `enet_host_service` to get some event from the server. This time I've put it in a while loop so that we may process every event that we received since our last call. I've set the timeout to 1000ms, but in a real time application such as an action video game, you might consider setting it to 0.

Next we switch the `event.type` to find out what event we are currently processing. Here we are simply checking for the case `ENET_EVENT_TYPE_RECEIVE` where we do nothing more than print our a bunch of values, used for testing at the moment.

```cpp
enet_peer_disconnect(peer, 0);

while(enet_host_service(client, &event, 3000) > 0)
{
	switch(event.type)
	{
		case ENET_EVENT_TYPE_RECEIVE:
			enet_packet_destroy(event.packet);
			break;
		case ENET_EVENT_TYPE_DISCONNECT:
			puts("Disconnection succeeded.");
			break;
	}
}

return EXIT_SUCCESS;
```

Finally, we call `enet_peer_disconnect` when we want to disconnect our client from the server. We call our faithful `enet_host_service` with a small timeout, 3 seconds in this case, to make sure the server is very much aware of the fact we are disconnecting! We do this by checking the received events and discarding events that are not of `ENET_EVENT_TYPE_DISCONNECT` and once we do receive that event we may print the fact we've successfully disconnected. And of course, we return 0 at the very end of our program.

## Last Few Steps!

We've seen nearly all the functions that we need to create a server! For this reason I will simply show you the server code, you'll see that it's very simple indeed!

> ⚠️ **Important Note!**
> 
> We never suggest anyone to do `while(true)`!
That said, we used it to showcase ENet code more and software architecture less.
Perhaps we will modify this in the future.

### ./server/main.c

```cpp
/* ./server/main.c */

#include <stdio.h>
#include <enet/enet.h>

int main (int argc, char ** argv)
{
	if (enet_initialize () != 0)
	{
		fprintf (stderr, "An error occurred while initializing ENet.\n");
		return EXIT_FAILURE;
	}
	atexit (enet_deinitialize);

	ENetEvent event;
	ENetAddress address;
	ENetHost* server;

	/* Bind the server to the default localhost.     */
	/* A specific host address can be specified by   */
	/* enet_address_set_host (& address, "x.x.x.x"); */
	address.host = ENET_HOST_ANY; // This allows
	/* Bind the server to port 7777. */
	address.port = 7777;



	server = enet_host_create (&address	/* the address to bind the server host to */,
					32	/* allow up to 32 clients and/or outgoing connections */,
					1	/* allow up to 1 channel to be used, 0. */,
					0	/* assume any amount of incoming bandwidth */,
					0	/* assume any amount of outgoing bandwidth */);

	if (server == NULL)
	{
		printf("An error occurred while trying to create an ENet server host.");
		return 1;
	}

	// gameloop
	while(true)
	{
		ENetEvent event;
		/* Wait up to 1000 milliseconds for an event. */
		while (enet_host_service (server, & event, 1000) > 0)
		{
			switch (event.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
					printf ("A new client connected from %x:%u.\n",
					event.peer -> address.host,
					event.peer -> address.port);
				break;

				case ENET_EVENT_TYPE_RECEIVE:
					printf ("A packet of length %u containing %s was received from %s on channel %u.\n",
						event.packet -> dataLength,
						event.packet -> data,
						event.peer -> data,
						event.channelID);
						/* Clean up the packet now that we're done using it. */
						enet_packet_destroy (event.packet);
				break;

				case ENET_EVENT_TYPE_DISCONNECT:
					printf ("%s disconnected.\n", event.peer -> data);
					/* Reset the peer's client information. */
					event.peer -> data = NULL;
			}
		}
	}

	enet_host_destroy(server);

	return 0;
}
```

## Not too hard to understand, right?

If you still have some trouble understanding, I'm sure you'll get the hang of it as we play around with it in this tutorial series.

[home](./index.md) | [next](./basic-enet-tutorial-series-part2-chatapp.md)

© Copyright 2020, Ezra Hradecky ([CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/)).
