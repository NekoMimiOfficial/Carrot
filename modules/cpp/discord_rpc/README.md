# Discord RPC on Carrot  
This module provides an asyncronous Discord RPC experience running in the background so you never have to deal with coro tasks since everything is handled in the background  
This module is based on [discord-rpc](https://github.com/discord/discord-rpc/) (compiled as a static library with -fPIC) it is old and doesn't support a lot of features so until we get a good networking library this will have to do  

# Building  
Building this is a little bit complicated, first you have to compile `discord-rpc` yourself to allow position independant code, follow the steps below:  
```sh
git clone https://github.com/discordapp/discord-rpc.git --depth=1
cd discord-rpc
mkdir build
cd build
cmake .. -DCMAKE_POSITION_INDEPENDENT_CODE=ON
make -j4
```
you will now have a file called `src/libdiscord-rpc.a` this is the library file you need  
make a directory called `libs` and within it a directory called `discord_rpc`  
inside this directory you'll place the library in addition to `discord_rpc.h`  
you can now make a build directory and `cmake .. && make -j4` to build the module  

# What can this do?  
let's look at a simple example:  
```js
let d = loadmodule("@libcarrot_discordrpc.so");
let rpc = d.createDiscordRPC("YOUR_APP_CLIENT_ID_PLS");

rpc.updatePresence(state="You can (not) save me", details="Neko is listening to", largeImageKey="https://lastfm-img.freetls.fastly.net/i/u/500x500/b4a7d118c3ddcda4e6e19c691fa3dc7f.jpg", startTimestamp=clock());
```

Some of the `updatePresence` fields contains:  
| Field | Description |
| - | - |
| `state` | Second line in the presence |
| `details` | First line in the presence |
| `largeImageKey` | Large image key or URL |
| `largeImageText` | Hover text for large image |
| `smallImageKey` | Small image key or URL |
| `smallImageText` | Hover text for small image |
| `partyId` | Identifier to group discord users into the same party |
| `partySize` | Number of players in party |
| `partyMax` | Max number of people in party |
| `joinSecret` | Secret string to be sent when clicking the join button |
| `matchSecret` | Honestly... isn't this the same as `spectateSecret`? |
| `spectateSecret` | Secret string to be sent when clicking the... spectate button..?? that exists?? |
| `instance` | I umm... think this has to do with telling discord if you're in a game lobby or not, 0/1 values |
| `startTimestamp` | The value you were looking for |
| `endTimestamp` | The value you were also looking for |

looks neat right? :3c  
we also provide `rpc.clearPresence()`, `rpc.exit()`, `rpc.isConnected()` and `rpc.isRunning()`  

# Please... we need a better version qwq  
While this version can do most of the things you want but the type is hard-coded as "playing" so you cant change it unless you modify the main library, *we might do that* or we might just make our own from scratch but we need your help for that cause I'm left maintaining all of this single handedly again... bwaaaa  
