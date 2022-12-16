# Peer-to-Peer Pong

A simple implenetation of the classic pong game. Supports local and peer-to-peer
games.

Written in C, using ncurses for the UI

### Instructions
After compiling, run the `pong` executable to start the game.\
Running the executable should show a menu. The key in the parenthesis tells you the key needed to navigate. For example if you see `(P)lay`, it indicates that you should press `P` to play
In addition to the game itself, the game has a simple leaderboard and quit options in the menu.\

There are three ways of running the game:
- local mode
- server mode
- client mode


In Local Mode, two players play on the same computer. The left paddle (first player) controls their paddle with the `W` and `S` keys. The second player uses the up and down arrow.\
If you wish to change the controls, change the definitons in `constants.h` and recompile.

When one player scores 5 of points, the game is over and the main menu is shown. The threshold can also be changed in `constants.h`

For networked play, one instance must in client mode and the other in sever. The server will wait for 10s for an incoming connection. The client must enter the name of the server (`localhost` if running on the same computer) to connet to a server.\
The client will always control the right paddle and the server will control the left.\

**There is very severe input lag as of right for the client**
