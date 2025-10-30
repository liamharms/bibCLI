# bibCLI
## A (very) useful and simple command line Bible tool
v 0.0.5

## Features
- Multiple translations
- Query any verse
- Search by keywords

## Installation
### On Linux:
1. `git clone https://liamharms/bibCLI`
2. `cd bibCLI`
3. `make`
### Other Systems:
- You'll have to manually compile it. It's super simple though; [`main.c`](./src/main.c) is literally the only file you have to compile. (you'll need the `-lsqlite3` flag) 

## Usage
`./bibCLI -h` will give you a help dialog that explains everything.

## Plans for the future
- Verse of the Day
- Support for commentaries, primarily targeting Early Church fathers and Reformers
- Support for confessions and creeds
- A TUI for no-arg calls (i.e. `./bibCLI`)

## Credits
- Huge thanks to [KastningBrandon](https://github.com/KastningBrandon) for the [strong's csv](https://github.com/KastningBrandon/python3-tools-for-strongs-bible-dictionary_03_2024/blob/master/data_processed/strongs.csv).
- Even bigger thanks to the crosswire team for their [SWORD project](https://www.crosswire.org/sword/index.jsp). Getting translations, especially with Strong's numbering, would've been bordline impossible without them.
 - I think, after looking through the versions I used, I shouldn't run into copyright or license issues 😅. Please let me know if I do.