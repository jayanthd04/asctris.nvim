local asctris = require('asctris')
local path = require("asctris.path")
local plugin_path = path.plugin_dir()
-- try to compile with -lncursesw
local function compileFile()
    local windowsCompile = os.execute("g++ ".. plugin_path .."/source/tetris.cpp -lncursesw -o ".. plugin_path .. "/source/asctrisExec")
    if windowsCompile ~= 0 then 
        local linuxCompile = os.execute("g++ ".. plugin_path .."/source/tetris.cpp -lncurses -o ".. plugin_path .. "/source/asctrisExec")
        if linuxCompile ~= 0 then
            error("Cannot compile file")
        end 
    end 
end 
-- print(path.exec_file_exists())
if(path.exec_file_exists()== false) then
    compileFile()
end
local function asctris_dispatcher(args)
    local func_name = args.fargs[1]
    if func_name and asctris[func_name] then 
        asctris[func_name]()
    else 
        print("Function ".. func_name .. " is invalid for Asctris")
    end 
end 
vim.api.nvim_create_user_command('Asctris',asctris_dispatcher,{nargs = 1})
