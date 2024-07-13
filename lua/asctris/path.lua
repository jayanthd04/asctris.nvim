local M = {}

local plugin_name = 'asctris'
M.plugin_dir = function ()
    -- local plugin_name = 'asctris'
    local rt_files = vim.api.nvim_get_runtime_file("lua/".. plugin_name .. "/init.lua",false)
    if #rt_files < 1 then 
        error("Can't find Asctris Plugin")
    end 
    local plugin_path = rt_files[1]
    -- local plugin_dir = vim.fn.fnamemodify(plugin_path,":h:h:h")
    -- plugin dir for now without cleaned up directory struct for neovim plugin 
    local plugin_dir = vim.fn.fnamemodify(plugin_path,":h:h:h")
    return plugin_dir
end 
M.exec_file_exists = function ()
    local plugin_path = M.plugin_dir()
    -- print(plugin_path)
    --local exec_file = vim.api.nvim_get_runtime_file(plugin_path .. "/source/tetrisNvim.*",false)
    local exec_file = vim.api.nvim_get_runtime_file("source/asctrisExec.*",false)
    print(exec_file[1])
    -- searching when files are added properly to nvim rtp 
    -- local exec_file = vim.api.nvim_get_runtime_file(plugin_path .. "/tetrisNvim.*",false)
    if #exec_file < 1 then 
        return false;
    end 
    return true; 
end 

return M
