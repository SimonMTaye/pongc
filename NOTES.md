## Choices I'm Debating
- *Macros vs Comments on generic functions*: I'm not sure if macros with preconfigured parameters on top of the generic routines is better than simply including the configs transparently and having comments explaining
## On Rewrite
- [ ] Make program not scale aware. In other, define fixed ways of drawing stuff and have them do scaling. Do not include scaling logic to other components of the game
- [ ] Use non-blocking input processing and only refresh ncurses from one thread. 
## Others
- Include more debuging lines and print them on scaling, end and stuff
- DO NOT USE MACRO RETURN VALUES
- Isolate funky coordinate maths to as little functions and reuse instead. Very buggy
- Rely on do doing a `doupdate` on a single thread and updating only virtual windows at all other times. This way, modifying windows doesn't have to be synced as the update will only happen once.
- In general pay extra attention to window refrehses. Like the funky math stuff, try to isolate this to a single funciton or a few funcitons so fixes and bugs are limited
