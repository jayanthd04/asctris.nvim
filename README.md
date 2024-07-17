# asctris.nvim
Take a quick break from coding by playing tetris right from your favorite text-editor. 
## What is an Asctris? 
Asctris is a port of the popular arcade game Tetris that is rendered purely using ASCII characters hence its name. Though functionally Asctris is very similar to Tetris, it does share some minor quirks such as the fact that there is no leveling mechanic that makes the blocks fall faster as the game progresses, and unlike its predecessor, once a block has been locked in, it cannot be moved. 
![Asctris Demo](/assets/asctris_demo_high_def.gif)
## Requirements
* Neovim >= 0.9.0  
* C++11 or higher 
* Ncurses for C++

## Installation

Using vim-plug

```viml
Plug 'jayanthd04/asctris.nvim'
```

Using packer.nvim 

```lua
use 'jayanthd04/asctris.nvim'
```

Using lazy.nvim

```lua
'jayanthd04/asctris.nvim'
```

After installation, you can continue to read on further on how to use Asctris here or you can run the command `:help asctris` at anytime right from Neovim to learn about how to use asctris.nvim. 
## Usage
Run the command `:Asctris run_game` to launch a new game of Asctris. 
### Controls 
* Press the left or right arrows to move blocks to the left or right respectively. 
* Press the down arrow key to make the block fall down faster. 
* Press the up arrow key to rotate the block 90 degrees clockwise.  
* You can exit the game whenever you want by simply pressing `Ctrl+c`
