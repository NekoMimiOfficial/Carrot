# Discord RPC on Carrot  
This module provides an asynchronous Discord RPC experience running in the background so you never have to deal with coro tasks since everything is handled in the background.  
This module has been fully rewritten to use the new and improved ~~3DS XL~~ **Discord Game SDK** (replacing the ancient `discord-rpc` library cuz i just found out it was archived qwq) AND THEN I FOUND OUT THE GAME SDK IS ALSO DEPRECATED SO I HAD TO INCLUDE A MODIFIED VERSION OF THE OLD AND DEPROVED BUT IMPOROVED `discord-rpc` LIBRARY qwq  

# Building  
welp... I now include the `discord-rpc` modified source so the installation is a bit easier:  

1. Head into `discord-rpc` and build it (regular cmake build like any other project)  
2. Copy libdiscord_rpc.a from build/src to lib (or use the precompiled one)  
3. Make a build directory for the module  
4. cmake build it  

# What can this do?  
Let's look at a cool example with the new features:  
```js
let d = loadmodule("@discord_rpc");
let rpc = d.createDiscordRPC("YOUR_APP_CLIENT_ID_PLS");

let imgURL = "https://nekomimi.nekos.team/res/misc/bafkreic343yqfehtx7epkxkz4ugydskriry4lqgq6fmmgkuequlgkcfgca.webp";

rpc.updatePresence(
    state="You can (not) save me", 
    details="Neko is listening to", 
    largeImageKey=imgURL, 
    startTimestamp=clock(),
    button1_label="Hello, world!",
    button1_url="https://nekomimi.nekos.team"
);
```

Some of the `updatePresence` fields contains:  
| Field | Description |
| - | - |
| `state` | Second line in the presence |
| `details` | First line in the presence |
| `type` | Activity type (0=Playing, 1=Streaming, 2=Listening, 3=Watching, 4=Custom, 5=Competing) |
| `buttons` | Array of up to 2 instances with `label` and `url` fields |
| `largeImageKey` | Large image key or URL |
| `largeImageText` | Hover text for large image |
| `smallImageKey` | Small image key or URL |
| `smallImageText` | Hover text for small image |
| `partyId` | Identifier to group discord users into the same party |
| `partySize` | Number of players in party |
| `partyMax` | Max number of people in party |
| `joinSecret` | Secret string to be sent when clicking the ask to join button |
| `matchSecret` | Honestly... isn't this the same as `spectateSecret`? |
| `spectateSecret` | Secret string to be sent when clicking the... spectate button..?? that exists?? |
| `instance` | I umm... think this has to do with telling discord if you're in a game lobby or not, 0/1 values |
| `startTimestamp` | The value you were looking for |
| `endTimestamp` | The value you were also looking for |
| `button1_label` | (finally) label for first RPC button |
| `button2_label` | (finally) label for second RPC button |
| `button1_url` | (finally) url for first RPC button |
| `button2_url` | (finally) url for second RPC button |

looks neat right? :3c  
we also provide `rpc.clearPresence()`, `rpc.stop()`, `rpc.isConnected()`, `rpc.isRunning()`  

# Yay! We finally got a better version! :3
I mean.... we dont have custom activity types yet but... you can use buttons at least!  
And until `discord-presence` gets fixed up i cant migrate there yet...  
