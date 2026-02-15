# text-editor
A minimal Vim-like text editor written in C.

### What it can do 
- Normal mode and Insert mode
- Basic Vim keymaps
- Load and save files
- Terminal based

### Test it out
```bash
mkdir build/ 
cd build
cmake ..
make
```
To load a file :

```bash 
./editor <filename>
```
To create a test file run :
```bash
base64 /dev/urandom | head -c 500 > file.txt
```
change the 500 to a larger value to test with bigger files

## TODO

- [ ] Implement undo,redo functionality
- [x] Add Warnings
- [x] Working j,k keys
- [x] Add line number,col number to status bar
- [x] Scrolling,fix cursor 

This isnt trying to compete with Vim.Its just me building one to learn
